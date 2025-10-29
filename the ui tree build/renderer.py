from PyQt5.QtWidgets import QOpenGLWidget
import numpy as np
from OpenGL.GL import *
from enum import Enum,auto
from widget_data import WidgetDataType
from PIL import Image



from PyQt5.QtWidgets import QApplication
from PyQt5.QtCore import QTimer
import sys

Image.MAX_IMAGE_PIXELS = None

class ShaderPass(Enum):
    PASS_MAP = 0
    PASS_BASIC = 1
    PASS_COMPLEX = 2
    PASS_TEXT = 3

class AssetDataType(Enum):
    TEXT = 0
    ASSET = 1
    BINARY_ASSET = 2
    TEXT_ASSET = 3
    IMAGE_ASSET = 4

class GSGRenderSystem(QOpenGLWidget):
    def __init__(self, GSG_gui_system):
        super().__init__()
        self.GSG_gui_system = GSG_gui_system
        self.assets = []
        self.text = []
        self.text_set = set()
        self.asset_path = set()
        self.buffers = {}  # name -> buffer id
        self.data = {}  # name -> numpy array
        self.assets_to_update = {}
        self.widget_max = 10000
    
    def initializeGL(self):
        glEnable(GL_BLEND)
        glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA)
        glEnable(GL_DEPTH_TEST)  # turn on depth testing
        glDepthFunc(GL_LESS)
        
        self.init_shaders("assets/shaders")  # compile your vertex & fragment shaders
        self.init_geometry()  # VAO/VBO for quads, shapes
        self.init_assets(assets_to_load={1:(AssetDataType.IMAGE_ASSET, ("assets/map.png", 0))})  # load map, UI assets
        self.init_FBOs(self.width(),self.height())  # create offscreen framebuffers for passes
        self.init_SSBOs(ssbo_specs={1: (WidgetDataType.POSITION,(self.widget_max * 4,np.int64)), 2: (WidgetDataType.SHADER_PASS,(self.widget_max,np.uint8)),3: (WidgetDataType.COLOUR,(self.widget_max * 4,np.uint8)),
                                    4: (WidgetDataType.SHAPE,(self.widget_max, np.uint8)), 5: (WidgetDataType.ASSETS_ID,(self.widget_max, np.int32)), 6: (WidgetDataType.TEXT_ID,(self.widget_max, np.int32)),
                                    7: (WidgetDataType.PARENT,(self.widget_max, np.int32))})  # upload per-widget arrays: position, color, flags, etc.
    
    def paintGL(self):
        self.update_ssbo()
        self.update_assets(self.assets_to_update)
        self.update_geometry()
        
        # 1️⃣ Map pass
        glBindFramebuffer(GL_FRAMEBUFFER , self.fbos[ShaderPass.PASS_MAP.value])
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        self.render_widgets(pass_type=ShaderPass.PASS_MAP)
        
        # 2️⃣ Basic widgets
        glBindFramebuffer(GL_FRAMEBUFFER , self.fbos[ShaderPass.PASS_BASIC.value])  # render to screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        self.render_widgets(pass_type=ShaderPass.PASS_BASIC)
        
        # 3️⃣ Complex widgets (offscreen first)
        glBindFramebuffer(GL_FRAMEBUFFER , self.fbos[ShaderPass.PASS_COMPLEX.value])
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        self.render_widgets(pass_type=ShaderPass.PASS_COMPLEX)
        
        # 4️⃣ Text pass
        glBindFramebuffer(GL_FRAMEBUFFER, self.fbos[ShaderPass.PASS_TEXT.value])
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        self.render_widgets(pass_type=ShaderPass.PASS_TEXT)
        
        
    def render_widgets(self , pass_type):
        pass
    
    def update_ssbo(self):
        for name , arr in self.data.items():
            glBindBuffer(GL_SHADER_STORAGE_BUFFER , self.buffers[name])
            glBufferSubData(GL_SHADER_STORAGE_BUFFER , 0 , arr.nbytes , arr)
    
    def update_geometry(self):
        pass
    
    def init_shaders(self,shader_dir: str):
        self.map_shader_program = self.load_shader_program(f"{shader_dir}/map_vert.glsl" , f"{shader_dir}/map_frag.glsl")
        self.basic_widget_shader_program = self.load_shader_program(f"{shader_dir}/basic_vert.glsl" , f"{shader_dir}/basic_frag.glsl")
        self.complex_widget_shader_program = self.load_shader_program(f"{shader_dir}/complex_vert.glsl" , f"{shader_dir}/complex_frag.glsl")
        self.text_shader_program = self.load_shader_program(f"{shader_dir}/text_vert.glsl" , f"{shader_dir}/text_frag.glsl")
    
    def init_geometry(self):
        self.vertices = np.full((self.widget_max * 5),0.0,dtype=np.float32)
        
        self.vao = glGenVertexArrays(1)
        glBindVertexArray(self.vao)
        
        self.vbo = glGenBuffers(1)
        glBindBuffer(GL_ARRAY_BUFFER , self.vbo)
        glBufferData(GL_ARRAY_BUFFER , self.vertices.nbytes , self.vertices , GL_STATIC_DRAW)
        
        stride = 5 * 4  # 5 floats * 4 bytes
        glVertexAttribPointer(0 , 3 , GL_FLOAT , GL_FALSE , stride , ctypes.c_void_p(0))  # pos (x,y,z)
        glEnableVertexAttribArray(0)
        glVertexAttribPointer(1 , 2 , GL_FLOAT , GL_FALSE , stride , ctypes.c_void_p(12))  # uv
        glEnableVertexAttribArray(1)
        
        glBindVertexArray(0)
    
    def init_assets(self, assets_to_load: dict):
        for asset_type ,(path_or_data, widget) in assets_to_load.items():
            if asset_type == AssetDataType.TEXT:
                if path_or_data not in self.text_set:
                    self.text_set.add(path_or_data)
                    self.text.append(path_or_data)
            if path_or_data not in self.asset_path:
                self.asset_path.add(path_or_data)
                if asset_type == AssetDataType.BINARY_ASSET:
                    file = open(path_or_data ,"r+b")
                elif asset_type == AssetDataType.TEXT_ASSET:
                    file = open(path_or_data,"r+")
                elif asset_type == AssetDataType.IMAGE_ASSET:
                    file = Image.open(path_or_data).convert("RGBA")
                else:
                    file = "broken"
                self.assets.append(file)
    
    def update_assets(self, assets_to_load: dict):
        self.init_assets(assets_to_load=assets_to_load)
    
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
        binding = 0
        for name, (size, dtype) in ssbo_specs.values():
            if not isinstance(name,int):
                name = name.value
            # Create numpy array for CPU-side storage
            arr = np.zeros(size, dtype=dtype)
            self.GSG_gui_system.data[name] = arr

            # Generate GPU buffer
            buf = glGenBuffers(1)
            self.buffers[name] = buf

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, buf)
            glBufferData(GL_SHADER_STORAGE_BUFFER, arr.nbytes, arr, GL_DYNAMIC_DRAW)
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, buf)  # binding = index
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0)
            binding += 1
    
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
if __name__ == "__main__":
    app = QApplication(sys.argv)
    win = GSGRenderSystem(None)
    win.resize(800, 600)
    win.show()

    # Auto-exit after 2 seconds to test stability
    QTimer.singleShot(2000, app.quit)

    app.exec_()
    print("✅ Test finished without crash.")