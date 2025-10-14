from PyQt5.QtWidgets import QOpenGLWidget
import numpy as np
from OpenGL.GL import *
from enum import Enum,auto
from PIL import Image

Image.MAX_IMAGE_PIXELS = None

class ShaderPass(Enum):
    PASS_MAP = auto()
    PASS_BASIC = auto()
    PASS_COMPLEX = auto()
    PASS_TEXT = auto()
    
class WidgetDataType(Enum):
    POSITION = auto()
    SHADER_PASS = auto()
    COLOUR = auto()
    SHAPE = auto()
    ASSETS_ID = auto()
    TEXT_ID = auto()
    ASSETS = auto()
    TEXT = auto()

class AssetDataType(Enum):
    TEXT = 0
    ASSET = 1
    BINARY_ASSET = 2
    TEXT_ASSET = 3
    IMAGE_ASSET = 4

class GSGRenderSystem(QOpenGLWidget):
    def __init__(self):
        super().__init__()
        self.position = np.full(10000,-1, dtype=np.int32)
        self.shader_pass = np.full(10000,-1, dtype=np.int32)
        self.colour = np.full(10000,-1, dtype=np.int32)
        self.shape = np.full(10000,-1, dtype=np.int32)
        self.assets_id = np.full(10000,-1, dtype=np.int32)
        self.text_id = np.full(10000,-1, dtype=np.int32)
        self.assets = []
        self.text = []
        self.text_set = set()
        self.asset_path = set()
        self.buffers = {}  # name -> buffer id
        self.data = {}  # name -> numpy array
        self.widget_max = 10000
    
    def initializeGL(self):
        glEnable(GL_BLEND)
        glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA)
        glEnable(GL_DEPTH_TEST)  # turn on depth testing
        glDepthFunc(GL_LESS)
        
        self.init_shaders()  # compile your vertex & fragment shaders
        self.init_geometry()  # VAO/VBO for quads, shapes
        self.init_assets(assets_to_load={1:(AssetDataType.IMAGE_ASSET, ("assets/map", 0))})  # load map, UI assets
        self.init_FBOs(self.width,self.heigt)  # create offscreen framebuffers for passes
        self.init_SSBOs(ssbo_specs={1: (WidgetDataType.POSITION,(self.widget_max * 4,np.int64)), 2: (WidgetDataType.SHADER_PASS,(self.widget_max,np.uint8)),3: (WidgetDataType.COLOUR,(self.widget_max * 4,np.uint8)),
                                    4: (WidgetDataType.SHAPE,(self.widget_max, np.uint8)), 5: (WidgetDataType.ASSETS_ID,(self.widget_max, np.int32)), 6: (WidgetDataType.TEXT_ID,(self.widget_max, np.int32))})  # upload per-widget arrays: position, color, flags, etc.
    
    def paintGL(self):
        # 1️⃣ Map pass
        glBindFramebuffer(GL_FRAMEBUFFER , self.map_fbo)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        self.render_widgets(pass_type=ShaderPass.PASS_MAP)
        
        # 2️⃣ Basic widgets
        glBindFramebuffer(GL_FRAMEBUFFER , 0)  # render to screen
        self.render_widgets(pass_type=ShaderPass.PASS_BASIC)
        
        # 3️⃣ Complex widgets (offscreen first)
        glBindFramebuffer(GL_FRAMEBUFFER , self.complex_fbo)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        self.render_widgets(pass_type=ShaderPass.PASS_COMPLEX)
        self.blit_fbo(self.complex_fbo)  # composite to screen
        
        # 4️⃣ Text pass
        self.render_widgets(pass_type=ShaderPass.PASS_TEXT)
    
    def render_widgets(self , pass_type):
        pass
    
    def init_shaders(self):
        self.map_shader_program = self.load_shader_program("" , "")
        self.basic_widget_shader_program = self.load_shader_program("" , "")
        self.complex_widget_shader_program = self.load_shader_program("" , "")
        self.text_shader_program = self.load_shader_program("" , "")
    
    def init_geometry(self):
        vertices = np.array([
            # x, y, z,   u, v
            -1.0 , -1.0 , 0.0 , 0.0 , 0.0 ,
            1.0 , -1.0 , 0.0 , 1.0 , 0.0 ,
            1.0 , 1.0 , 0.0 , 1.0 , 1.0 ,
            -1.0 , 1.0 , 0.0 , 0.0 , 1.0
        ] , dtype=np.float32)
        
        indices = np.array([0 , 1 , 2 , 2 , 3 , 0] , dtype=np.uint32)
        
        self.vao = glGenVertexArrays(1)
        glBindVertexArray(self.vao)
        
        self.vbo = glGenBuffers(1)
        glBindBuffer(GL_ARRAY_BUFFER , self.vbo)
        glBufferData(GL_ARRAY_BUFFER , vertices.nbytes , vertices , GL_STATIC_DRAW)
        
        self.ebo = glGenBuffers(1)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER , self.ebo)
        glBufferData(GL_ELEMENT_ARRAY_BUFFER , indices.nbytes , indices , GL_STATIC_DRAW)
        
        stride = 5 * 4  # 5 floats * 4 bytes
        glVertexAttribPointer(0 , 3 , GL_FLOAT , GL_FALSE , stride , ctypes.c_void_p(0))  # pos (x,y,z)
        glEnableVertexAttribArray(0)
        glVertexAttribPointer(1 , 2 , GL_FLOAT , GL_FALSE , stride , ctypes.c_void_p(12))  # uv
        glEnableVertexAttribArray(1)
        
        glBindVertexArray(0)
    
    def init_assets(self, assets_to_load: dict):
        for asset_type ,(path_or_data, widget) in assets_to_load.items():
            if asset_type[0] == "text":
                if path_or_data in self.asset_path:
                    self.text_set.add(path_or_data)
                    self.text.append(path_or_data)
            elif asset_type[0] == "asset":
                if path_or_data in self.text_set:
                    self.asset_path.add(path_or_data)
                    if asset_type[1] == AssetDataType.BINARY_ASSET:
                        file = open(path_or_data ,"r+b")
                    elif asset_type == AssetDataType.TEXT_ASSET:
                        file = open(path_or_data,"r+")
                    elif asset_type == AssetDataType.IMAGE_ASSET:
                        file = Image.open(path_or_data).convert("RGBA")
                    else:
                        file = "broken"
                    self.assets.append(file)
    
    def init_FBOs(self , width , height):
        self.fbos = []
        self.textures = []
        
        for i in range(4):  # e.g. 4 passes
            fbo = glGenFramebuffers(1)
            glBindFramebuffer(GL_FRAMEBUFFER , fbo)
            
            tex = glGenTextures(1)
            glBindTexture(GL_TEXTURE_2D , tex)
            glTexImage2D(GL_TEXTURE_2D , 0 , GL_RGBA , width , height , 0 ,
                         GL_RGBA , GL_UNSIGNED_BYTE , None)
            glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR)
            glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR)
            
            glFramebufferTexture2D(GL_FRAMEBUFFER , GL_COLOR_ATTACHMENT0 ,
                                   GL_TEXTURE_2D , tex , 0)
            
            # optional depth buffer
            rbo = glGenRenderbuffers(1)
            glBindRenderbuffer(GL_RENDERBUFFER , rbo)
            glRenderbufferStorage(GL_RENDERBUFFER , GL_DEPTH24_STENCIL8 , width , height)
            glFramebufferRenderbuffer(GL_FRAMEBUFFER , GL_DEPTH_STENCIL_ATTACHMENT ,
                                      GL_RENDERBUFFER , rbo)
            
            if glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE:
                print("FBO" , i , "is not complete")
            
            self.fbos.append(fbo)
            self.textures.append(tex)
        
        glBindFramebuffer(GL_FRAMEBUFFER , 0)
    
    def init_SSBOs(self, ssbo_specs: dict):
        """
        ssbo_specs: dict of {name: (size, dtype)}
        size = number of elements
        dtype = numpy dtype
        """
        for name, (size, dtype) in ssbo_specs.items():
            # Create numpy array for CPU-side storage
            arr = np.zeros(size, dtype=dtype)
            self.data[name] = arr

            # Generate GPU buffer
            buf = glGenBuffers(1)
            self.buffers[name] = buf

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, buf)
            glBufferData(GL_SHADER_STORAGE_BUFFER, arr.nbytes, arr, GL_DYNAMIC_DRAW)
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, len(self.buffers)-1, buf)  # binding = index
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0)
    
    @staticmethod
    def load_shader_program(vertex_path , fragment_path):
        # Helper to read file
        def read_file(path):
            with open(path , "r") as f:
                return f.read()
        
        # Compile a shader
        def compile_shader(source , shader_type):
            shader = glCreateShader(shader_type)
            glShaderSource(shader , source)
            glCompileShader(shader)
            
            if not glGetShaderiv(shader , GL_COMPILE_STATUS):
                log = glGetShaderInfoLog(shader).decode()
                kind = "Vertex" if shader_type == GL_VERTEX_SHADER else "Fragment"
                raise RuntimeError(f"{kind} shader compilation failed:\n{log}")
            return shader
        
        # Read sources
        vertex_src = read_file(vertex_path)
        fragment_src = read_file(fragment_path)
        
        # Compile shaders
        vertex_shader = compile_shader(vertex_src , GL_VERTEX_SHADER)
        fragment_shader = compile_shader(fragment_src , GL_FRAGMENT_SHADER)
        
        # Create program and link
        program = glCreateProgram()
        glAttachShader(program , vertex_shader)
        glAttachShader(program , fragment_shader)
        glLinkProgram(program)
        
        if not glGetProgramiv(program , GL_LINK_STATUS):
            log = glGetProgramInfoLog(program).decode()
            raise RuntimeError(f"Shader program link failed:\n{log}")
        
        # Cleanup
        glDetachShader(program , vertex_shader)
        glDetachShader(program , fragment_shader)
        glDeleteShader(vertex_shader)
        glDeleteShader(fragment_shader)
        
        return program