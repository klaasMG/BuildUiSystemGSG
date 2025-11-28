from PyQt5.QtWidgets import QOpenGLWidget
import numpy as np
from OpenGL.GL import *
from enum import Enum
from widget_data import WidgetDataType
from PIL import Image
from event_system import event_system, EventQueue, EventTypeEnum
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
    def __init__(self , GSG_gui_system):
        super().__init__()
        self.GSG_gui_system = GSG_gui_system
        self.assets = []
        self.text = []
        self.text_set = set()
        self.asset_path = set()
        self.buffers = {}  # name -> buffer id
        self.data = {}  # name -> numpy array
        self.assets_to_update = {}
        self.widget_max = self.GSG_gui_system.widget_max
        self.vertices = np.full((self.widget_max * 5) , 0.0 , dtype=np.float32)
        self.shader_pass_right = 0
        self.shader_pass_size = 0
        self.locations_shader_passes = []
        self.locations_size_passes = []
        self.render_queue: EventQueue = event_system.add_queue("renderer")
    
    def resizeGL(self, width, height):
        priority = 0
        destination = "ui_manager"
        event_type = EventTypeEnum.Resize
        if not isinstance(width, int):
            int(width)
        if not isinstance(height, int):
            int(height)
        event_data = (width, height)
        event = (priority, destination, event_type, event_data)
        self.render_queue.send_event(event)
        self.init_FBOs(width,height)
    
    def initializeGL(self):
        width = self.width()
        height = self.height()
        priority = 0
        destination = "ui_manager"
        event_type = EventTypeEnum.Resize
        if not isinstance(width , int):
            int(width)
        if not isinstance(height , int):
            int(height)
        event_data = (width , height)
        event = (priority , destination , event_type , event_data)
        self.render_queue.send_event(event)
        glEnable(GL_BLEND)
        glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA)
        glEnable(GL_DEPTH_TEST)  # turn on depth testing
        glDepthFunc(GL_LESS)
        glEnable(GL_PROGRAM_POINT_SIZE)
        
        self.init_shaders("assets/shaders")  # compile your vertex & fragment shaders
        self.init_geometry()  # VAO/VBO for quads, shapes
        self.init_assets(
            assets_to_load={1: (AssetDataType.IMAGE_ASSET , ("assets/map.png" , 0))})  # load map, UI assets
        self.init_FBOs(self.width() , self.height())  # create offscreen framebuffers for passes
        self.init_SSBOs(
            ssbo_specs={0: WidgetDataType.POSITION , 1: WidgetDataType.SHADER_PASS , 2: WidgetDataType.COLOUR ,
                        3: WidgetDataType.SHAPE , 4: WidgetDataType.ASSETS_ID , 5: WidgetDataType.TEXT_ID ,
                        6: WidgetDataType.PARENT})  # upload per-widget arrays: position, color, flags, etc.
    
    def paintGL(self):
        self.update_ssbo()
        self.update_assets(self.assets_to_update)
        self.update_geometry()
        self.shader_pass_uniform_reset_pass()
        self.reset_uniform_size()
        widget_count = self.GSG_gui_system.next_id
        # 1️⃣ Map pass
        glBindFramebuffer(GL_FRAMEBUFFER , self.fbos[ShaderPass.PASS_MAP])
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glUseProgram(self.map_shader_program)
        self.shader_pass_uniform_update_pass()
        self.set_uniform_size()
        glBindVertexArray(self.vao)
        glDrawArrays(GL_POINTS, 0, widget_count)
        
        # 2️⃣ Basic widgets
        glBindFramebuffer(GL_FRAMEBUFFER , self.fbos[ShaderPass.PASS_BASIC])  # render to screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glUseProgram(self.basic_widget_shader_program)
        self.shader_pass_uniform_update_pass()
        self.set_uniform_size()
        glBindVertexArray(self.vao)
        glDrawArrays(GL_POINTS, 0, widget_count)
        
        # 3️⃣ Complex widgets (offscreen first)
        glBindFramebuffer(GL_FRAMEBUFFER , self.fbos[ShaderPass.PASS_COMPLEX])
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glUseProgram(self.complex_widget_shader_program)
        self.shader_pass_uniform_update_pass()
        self.set_uniform_size()
        glBindVertexArray(self.vao)
        glDrawArrays(GL_POINTS, 0, widget_count)
        
        # 4️⃣ Text pass
        glBindFramebuffer(GL_FRAMEBUFFER , self.fbos[ShaderPass.PASS_TEXT])
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glUseProgram(self.text_shader_program)
        self.shader_pass_uniform_update_pass()
        self.set_uniform_size()
        glBindVertexArray(self.vao)
        glDrawArrays(GL_POINTS, 0, widget_count)
        
        glBindFramebuffer(GL_FRAMEBUFFER , self.defaultFramebufferObject())
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glUseProgram(self.final_composite_program)
        
        # bind all previous FBO textures for sampling
        for i , (pass_type , tex) in enumerate(self.textures.items()):
            glActiveTexture(GL_TEXTURE0 + i)
            glBindTexture(GL_TEXTURE_2D , tex)
            loc = glGetUniformLocation(self.final_composite_program , f"fbo_tex_{pass_type.name.lower()}")
            if loc != -1:
                glUniform1i(loc , i)
        
        # draw full-screen quad (your VAO already works)
        glBindVertexArray(self.vao)
        glDrawArrays(GL_TRIANGLES , 0 , 6)
        glBindVertexArray(0)
        
    def shader_pass_uniform_update_pass(self):
        self.shader_pass_right += 1
        glUniform1i(self.locations_shader_passes[self.shader_pass_right - 1],self.shader_pass_right)
        
    def shader_pass_uniform_reset_pass(self):
        self.shader_pass_right = 0
        
    def declare_uniform_pass(self, shader_program):
        pass_new_shader_loc = glGetUniformLocation(shader_program, "shader_pass_right")
        self.locations_shader_passes.append(pass_new_shader_loc)
    
    def declare_uniform_size(self, shader_program):
        shader_size_loc = glGetUniformLocation(shader_program, "screen_size")
        self.locations_size_passes.append(shader_size_loc)
    
    def set_uniform_size(self):
        self.shader_pass_size += 1
        size = np.array([self.width(), self.height()], dtype=np.uint32)
        location = self.locations_size_passes[self.shader_pass_size - 1]
        if location == -1:
            return
        glUniform2uiv(location,1,size)
    
    def reset_uniform_size(self):
        self.shader_pass_size = 0
    
    def update_ssbo(self):
        for name , arr in self.GSG_gui_system.widget_data.items():
            glBindBuffer(GL_SHADER_STORAGE_BUFFER , self.buffers[name])
            glBufferSubData(GL_SHADER_STORAGE_BUFFER , 0 , arr.nbytes , arr)
    
    def update_geometry(self):
        """
                Resets the VBO for the current frame.
                The vertex data is assumed to already be correct in self.vertices.
                """
        glBindBuffer(GL_ARRAY_BUFFER , self.vbo)
        glBufferSubData(GL_ARRAY_BUFFER , 0 , self.vertices.nbytes , self.vertices)
        glBindBuffer(GL_ARRAY_BUFFER , 0)
    
    def init_shaders(self , shader_dir: str):
        self.map_shader_program = self.load_shader_program(f"{shader_dir}/map_vert.glsl" ,
                                                           f"{shader_dir}/map_frag.glsl")
        self.declare_uniform_size(self.map_shader_program)
        self.declare_uniform_pass(self.map_shader_program)
        self.basic_widget_shader_program = self.load_shader_program(f"{shader_dir}/basic_vert.glsl" ,
                                                                    f"{shader_dir}/basic_frag.glsl")
        self.declare_uniform_size(self.basic_widget_shader_program)
        self.declare_uniform_pass(self.basic_widget_shader_program)
        self.complex_widget_shader_program = self.load_shader_program(f"{shader_dir}/complex_vert.glsl" ,
                                                                      f"{shader_dir}/complex_frag.glsl")
        self.declare_uniform_size(self.complex_widget_shader_program)
        self.declare_uniform_pass(self.complex_widget_shader_program)
        self.text_shader_program = self.load_shader_program(f"{shader_dir}/text_vert.glsl" ,
                                                            f"{shader_dir}/text_frag.glsl")
        self.declare_uniform_size(self.text_shader_program)
        self.declare_uniform_pass(self.text_shader_program)
        self.final_composite_program = self.load_shader_program(f"{shader_dir}/final_vert.glsl" ,
                                                                f"{shader_dir}/final_frag.glsl")
    
    def init_geometry(self):
        self.vao = glGenVertexArrays(1)
        glBindVertexArray(self.vao)
        
        self.vbo = glGenBuffers(1)
        glBindBuffer(GL_ARRAY_BUFFER , self.vbo)
        glBufferData(GL_ARRAY_BUFFER , self.vertices.nbytes , self.vertices , GL_DYNAMIC_DRAW)
        
        stride = 5 * 4  # 5 floats * 4 bytes
        glVertexAttribPointer(0 , 3 , GL_FLOAT , GL_FALSE , stride , ctypes.c_void_p(0))  # pos (x,y,z)
        glEnableVertexAttribArray(0)
        glVertexAttribPointer(1 , 2 , GL_FLOAT , GL_FALSE , stride , ctypes.c_void_p(12))  # uv
        glEnableVertexAttribArray(1)
        
        glBindVertexArray(0)
    
    def init_assets(self , assets_to_load: dict):
        for asset_type , (path_or_data , widget) in assets_to_load.items():
            if asset_type == AssetDataType.TEXT:
                if path_or_data not in self.text_set:
                    self.text_set.add(path_or_data)
                    self.text.append(path_or_data)
            if path_or_data not in self.asset_path:
                self.asset_path.add(path_or_data)
                if asset_type == AssetDataType.BINARY_ASSET:
                    file = open(path_or_data , "r+b")
                elif asset_type == AssetDataType.TEXT_ASSET:
                    file = open(path_or_data , "r+")
                elif asset_type == AssetDataType.IMAGE_ASSET:
                    file = Image.open(path_or_data).convert("RGBA")
                else:
                    file = "broken"
                self.assets.append(file)
    
    def update_assets(self , assets_to_load: dict):
        self.init_assets(assets_to_load=assets_to_load)
    
    def init_FBOs(self , width , height):
        self.fbos = {}
        self.textures = {}
        
        for pass_type in ShaderPass:
            fbo = glGenFramebuffers(1)
            glBindFramebuffer(GL_FRAMEBUFFER , fbo)
            
            tex = glGenTextures(1)
            glBindTexture(GL_TEXTURE_2D , tex)
            glTexImage2D(GL_TEXTURE_2D , 0 , GL_RGBA , width , height , 0 , GL_RGBA , GL_UNSIGNED_BYTE , None)
            glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR)
            glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR)
            
            glFramebufferTexture2D(GL_FRAMEBUFFER , GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D , tex , 0)
            
            rbo = glGenRenderbuffers(1)
            glBindRenderbuffer(GL_RENDERBUFFER , rbo)
            glRenderbufferStorage(GL_RENDERBUFFER , GL_DEPTH24_STENCIL8 , width , height)
            glFramebufferRenderbuffer(GL_FRAMEBUFFER , GL_DEPTH_STENCIL_ATTACHMENT , GL_RENDERBUFFER , rbo)
            
            if glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE:
                print(f"FBO {pass_type.name} incomplete")
            
            self.fbos[pass_type] = fbo
            self.textures[pass_type] = tex
        
        glBindFramebuffer(GL_FRAMEBUFFER , 0)
    
    def init_SSBOs(self , ssbo_specs: dict):
        """
        ssbo_specs: dict of {name: (size, dtype)}
        size = number of elements
        dtype = numpy dtype
        """
        max_ssbos = glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS)
        for binding, name in ssbo_specs.items():
            # Create numpy array for CPU-side storage
            arr = self.GSG_gui_system.widget_data[name]
            
            # Generate GPU buffer
            buf = glGenBuffers(1)
            self.buffers[name] = buf
            
            glBindBuffer(GL_SHADER_STORAGE_BUFFER , buf)
            glBufferData(GL_SHADER_STORAGE_BUFFER , arr.nbytes , arr , GL_DYNAMIC_DRAW)
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER , binding , buf)
            glBindBuffer(GL_SHADER_STORAGE_BUFFER , 0)
    
    def bind_fbo_textures(self , shader_program):
        """Bind all FBO textures to consecutive texture units for sampling in a shader."""
        glUseProgram(shader_program)
        for i , (pass_type , tex) in enumerate(self.textures.items()):
            glActiveTexture(GL_TEXTURE0 + i)
            glBindTexture(GL_TEXTURE_2D , tex)
            # optional: set a uniform in the shader (e.g., sampler2D array)
            loc = glGetUniformLocation(shader_program , f"fbo_tex_{pass_type.name.lower()}")
            if loc != -1:
                glUniform1i(loc , i)
    
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
    win.resize(800 , 600)
    win.show()
    
    # Auto-exit after 2 seconds to test stability
    QTimer.singleShot(2000 , app.quit)
    
    app.exec_()
    print("✅ Test finished without crash.")