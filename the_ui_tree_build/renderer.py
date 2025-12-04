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
from PassSystem import ShaderPassData
Image.MAX_IMAGE_PIXELS = None

class ShaderPass(Enum):
    PASS_MAP = 0
    PASS_BASIC = 1
    PASS_TEXT = 2
    PASS_FINAL = 3


class AssetDataType(Enum):
    TEXT = 0
    ASSET = 1
    BINARY_ASSET = 2
    TEXT_ASSET = 3
    IMAGE_ASSET = 4


class GSGRenderSystem(QOpenGLWidget):
    def __init__(self , GSG_gui_system):
        super().__init__()
        self.fullscreen_vao = None
        self.fullscreen_vbo = None
        self.GSG_gui_system = GSG_gui_system
        self.assets = []
        self.text = []
        self.text_set = set()
        self.asset_path = set()
        self.buffers: dict[int,WidgetDataType] = {}  # name -> buffer id
        self.assets_to_update = {}
        self.widget_max = self.GSG_gui_system.widget_max
        self.vertices = np.full((self.widget_max * 4) , 0.0 , dtype=np.float32)
        self.vertices[0:12] = np.array([-1.0,  1.0, 0.0, 1.0, -1.0,  0.0, 0.0, 0.5,0.0,  1.0, 0.5, 1.0  ],dtype=np.float32)
        self.quad = np.array([-1.0, -1.0, 0.0, 0.0,1.0, -1.0, 1.0, 0.0,-1.0,  1.0, 0.0, 1.0,-1.0,  1.0, 0.0, 1.0,1.0, -1.0, 1.0, 0.0,1.0,  1.0, 1.0, 1.0,], dtype=np.float32)
        self.render_queue: EventQueue = event_system.add_queue("renderer")
        self.shader_passes: dict[ShaderPass, ShaderPassData] = {}
    
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
        for shader_pass in self.shader_passes.values():
            self.init_FBOs(width, height, shader_pass)
    
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
        
        self.shader_passes[ShaderPass.PASS_BASIC] = ShaderPassData("assets/shaders/basic_frag.glsl", "assets/shaders/basic_vert.glsl")
        self.shader_passes[ShaderPass.PASS_FINAL] = ShaderPassData("assets/shaders/final_frag.glsl", "assets/shaders/final_vert.glsl")
        
        # --- build shader program ---
        self.init_shaders(self.shader_passes)
        for shader_pass_type,shader_pass in self.shader_passes.items():
            # --- create VAO + VBO for your existing self.quad ---
            shader_pass.assign_vao()
            shader_pass.assign_vbo()
            if shader_pass_type == ShaderPass.PASS_FINAL:
                vertex_data = self.quad
            else:
                self.init_FBOs(width, height, shader_pass)
                vertex_data = self.vertices
            
            glBindVertexArray(shader_pass.vao)
            glBindBuffer(GL_ARRAY_BUFFER, shader_pass.vbo)
            glBufferData(GL_ARRAY_BUFFER, vertex_data.nbytes, vertex_data, GL_STATIC_DRAW)
            
            stride = 4 * vertex_data.itemsize
            
            # pos
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, ctypes.c_void_p(0))
            glEnableVertexAttribArray(0)
            
            # uv
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, ctypes.c_void_p(8))
            glEnableVertexAttribArray(1)
            
            glBindBuffer(GL_ARRAY_BUFFER, 0)
            glBindVertexArray(0)
        
    def init_data(self):
        for data in self.GSG_gui_system.widget_data:
            self.buffers[data] = self.GSG_gui_system.widget_data[data]
    
    def paintGL(self):
        glClearColor(0.1, 0.1, 0.1, 1.0)
        glClear(GL_COLOR_BUFFER_BIT)
        self.basic_render_pass()
        self.final_render_pass()
        
    def final_render_pass(self):
        shader_pass = self.shader_passes[ShaderPass.PASS_FINAL]
        
        glBindFramebuffer(GL_FRAMEBUFFER, self.defaultFramebufferObject())
        glViewport(0, 0, self.width(), self.height())
        
        glUseProgram(shader_pass.program)
        glBindVertexArray(shader_pass.vao)
        glDrawArrays(GL_TRIANGLES, 0, 6)
        glBindVertexArray(0)
        
    def basic_render_pass(self):
        shader_pass = self.shader_passes[ShaderPass.PASS_BASIC]
        
        glBindFramebuffer(GL_FRAMEBUFFER, shader_pass.fbo)
        glViewport(0, 0, self.width(), self.height())
        
        glUseProgram(shader_pass.program)
        glBindVertexArray(shader_pass.vao)
        
        prev_pass_tex = self.shader_passes[ShaderPass.PASS_BASIC].texture
        glActiveTexture(GL_TEXTURE0)
        glBindTexture(GL_TEXTURE_2D, prev_pass_tex)
        
        # Tell the shader that 'sampler2D uPrevPass' is bound to unit 0
        location = glGetUniformLocation(shader_pass.program, "uPrevPass")
        glUniform1i(location, 0)
        
        glDrawArrays(GL_TRIANGLES, 0, 6)
        glBindVertexArray(0)
        
    def update_ssbo(self, data_enum):
        buffer_id = self.buffers.get(data_enum)
        if not buffer_id:
            return
        
        array = np.array(self.GSG_gui_system.widget_data[data_enum], dtype=np.float32)
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer_id)
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, array.nbytes, array)
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0)
    
    def update_geometry(self):
        pass
    
    def init_shaders(self , shader_dir: dict):
        for shader_pass in shader_dir.values():
            shader_pass: ShaderPassData = shader_pass
            shader_pass.load(self)
    
    def init_geometry(self):
        pass
    
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
    
    def init_FBOs(self , width , height, shader_pass):
        shader_pass.assign_fbo()
        glBindFramebuffer(GL_FRAMEBUFFER, shader_pass.fbo)
        
        shader_pass.assign_text()
        glBindTexture(GL_TEXTURE_2D, shader_pass.texture)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, None)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, shader_pass.texture, 0)
        
        status = glCheckFramebufferStatus(GL_FRAMEBUFFER)
        if status != GL_FRAMEBUFFER_COMPLETE:
            glBindFramebuffer(GL_FRAMEBUFFER, 0)
            raise RuntimeError(f"FBO incomplete: {hex(status)}")
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0)

    def init_SSBOs(self):
        """
        Initializes SSBOs using the parent GSG_gui_system data.
        Each key in self.buffers comes from GSG_gui_system.widget_data.
        """
        for data_enum, parent_array in self.GSG_gui_system.widget_data.items():
            # skip if already initialized
            if data_enum in self.buffers and self.buffers[data_enum] is not None:
                continue
            
            buffer_id = glGenBuffers(1)
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer_id)
            
            # Ensure it's a contiguous float32 numpy array
            array = np.array(parent_array, dtype=np.float32)
            glBufferData(GL_SHADER_STORAGE_BUFFER, array.nbytes, array, GL_DYNAMIC_DRAW)
            
            # Optional: binding point = enum value
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, data_enum.value, buffer_id)
            
            self.buffers[data_enum] = buffer_id
        
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


if __name__ == "__main__":
    app = QApplication(sys.argv)
    win = GSGRenderSystem(None)
    win.resize(800 , 600)
    win.show()
    
    # Auto-exit after 2 seconds to test stability
    QTimer.singleShot(2000 , app.quit)
    
    app.exec_()
    print("✅ Test finished without crash.")