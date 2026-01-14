import time

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
from PassSystem import ShaderPassData, Texture

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

def unpack_u16(packed: int) -> tuple[int, int]:
    a = packed & 0xFFFF
    b = (packed >> 16) & 0xFFFF
    return a, b



class GSGRenderSystem(QOpenGLWidget):
    def __init__(self, GSG_gui_system):
        super().__init__()
        self.is_counting = True
        self.frame_times: list[float] = []
        self.real_time: list[float] = []
        self.height_texture = None
        self.fullscreen_vao = None
        self.fullscreen_vbo = None
        self.time = time.time()
        self.GSG_gui_system = GSG_gui_system
        self.assets = self.GSG_gui_system.assets
        self.text = self.GSG_gui_system.text
        self.text_set = self.GSG_gui_system.text_set
        self.asset_path = self.GSG_gui_system.asset_path
        self.asset_ids = self.GSG_gui_system.asset_ids
        self.text_ids = self.GSG_gui_system.text_ids
        self.texture_atlas = Image.open("assets/image_atlases/atlas.png")
        self.atlas_texture: Texture | None = None
        self.open_assets = set()
        self.buffers: dict[int, WidgetDataType] = {}  # name -> buffer id
        self.assets_to_update = {}
        self.widget_max = self.GSG_gui_system.widget_max
        self.vertices = np.full((self.widget_max * 4), 3.0, dtype=np.float32)
        self.quad = np.array(
            [-1.0, -1.0, 0.0, 0.0, 1.0, -1.0, 1.0, 0.0, -1.0, 1.0, 0.0, 1.0, -1.0, 1.0, 0.0, 1.0, 1.0, -1.0, 1.0, 0.0,
             1.0, 1.0, 1.0, 1.0, ], dtype=np.float32)
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
        for shader_type, shader_pass in self.shader_passes.items():
            if shader_type == ShaderPass.PASS_FINAL:
                pass
            else:
                self.init_FBOs(width, height, shader_pass)
        self.init_textures(width, height)
    
    def initializeGL(self):
        time_start = time.time()
        width = self.width()
        height = self.height()
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
        
        glEnable(GL_PROGRAM_POINT_SIZE)
        glDisable(GL_BLEND)
        glDisable(GL_DITHER)
        
        self.shader_passes[ShaderPass.PASS_BASIC] = ShaderPassData("assets/shaders/basic_frag.glsl",
                                                                   "assets/shaders/basic_vert.glsl")
        self.shader_passes[ShaderPass.PASS_FINAL] = ShaderPassData("assets/shaders/final_frag.glsl",
                                                                   "assets/shaders/final_vert.glsl")
        
        # --- build shader program ---
        self.init_shaders(self.shader_passes)
        for shader_pass_type, shader_pass in self.shader_passes.items():
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
        
        self.init_textures(height, width)
        
        self.atlas_texture = Texture(self.texture_atlas)
        
        self.init_SSBOs()
        time_finish = time_start - time.time()
        print(time_finish)
    
    def init_data(self):
        for data in self.GSG_gui_system.widget_data:
            self.buffers[data] = self.GSG_gui_system.widget_data[data]
    
    def paintGL(self):
        if self.is_counting:
            if len(self.real_time) > 1:
                self.frame_times.append(time.time() - self.real_time[-1])
                print(time.time() - self.real_time[-1])
            else:
                self.frame_times.append(0)
            self.real_time.append(time.time())
            if len(self.real_time) > 100:
                with open("frame_times.txt", "a") as f:
                    for t in self.frame_times:
                        f.write(f"{t}\n")
                self.real_time.clear()
                self.frame_times.clear()
            
        self.GSG_gui_system.pos_update()
        
        glClearBufferfv(GL_COLOR, 0, (0.0, 0.0, 0.0, 0.0))  # RGBA8
        glClearBufferuiv(GL_COLOR, 1, (0,))  # R32UI
        self.update_assets()
        self.atlas_texture.resend(self.texture_atlas)
        
        glClearColor(0.1, 0.1, 0.1, 1.0)
        glClear(GL_COLOR_BUFFER_BIT)
        
        for data_enum in self.buffers.keys():
            self.update_ssbo(data_enum)
        
        self.basic_render_pass()
        self.final_render_pass()
        clear_value = np.array([0], dtype=np.uint32)
        glClearTexImage(self.height_texture, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, clear_value)
    
    def final_render_pass(self):
        shader_pass = self.shader_passes[ShaderPass.PASS_FINAL]
        
        glBindFramebuffer(GL_FRAMEBUFFER, self.defaultFramebufferObject())
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
    
    def basic_render_pass(self):
        shader_pass = self.shader_passes[ShaderPass.PASS_BASIC]
        
        glBindFramebuffer(GL_FRAMEBUFFER, shader_pass.fbo)
        glViewport(0, 0, self.width(), self.height())
        
        glUseProgram(shader_pass.program)
        shader_pass.set_atlas()
        glBindVertexArray(shader_pass.vao)
        
        glDrawArrays(GL_POINTS, 0, self.widget_max)
        glBindVertexArray(0)
        
        pixel = np.zeros(1, dtype=np.uint32)
        
        glReadBuffer(GL_COLOR_ATTACHMENT1)  # info_map
        glReadPixels(
            350, 250, 1, 1,
            GL_RED_INTEGER,
            GL_UNSIGNED_INT,
            pixel
        )
        
        value = int(pixel[0])
        print(value)
        height, widget_id = unpack_u16(value)
        print(height)
        print(widget_id)
        
    
    def update_ssbo(self, data_enum):
        buffer_id = self.buffers.get(data_enum)
        if not buffer_id:
            return
        
        array = np.array(self.GSG_gui_system.widget_data[data_enum], dtype=np.int32)
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer_id)
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, array.nbytes, array)
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0)
    
    def init_shaders(self, shader_dir: dict):
        for shader_pass in shader_dir.values():
            shader_pass: ShaderPassData = shader_pass
            shader_pass.load(self)
    
    def init_assets(self):
        for asset in self.asset_ids:
            if asset not in self.open_assets:  # correct asset found
                asset_id = self.asset_ids[asset]
                if self.file_type(asset) == "text":
                    file = open(asset, "r")
                elif self.file_type(asset) == "binary":
                    file = open(asset, "r+b")
                elif self.file_type(asset) == "image":  # finds file type image
                    file = Image.open(asset).convert("RGBA")

                    assets_per_row = 32
                    tile_size = 256
                    col = asset_id % assets_per_row
                    row = asset_id // assets_per_row
                    paste_x = col * tile_size
                    paste_y = row * tile_size
                    self.texture_atlas.paste(file, (paste_x, paste_y), file)
                    self.texture_atlas.save("hy.png", format="PNG")
                else:
                    file = "broken"
                self.open_assets.add(asset)
                if len(self.assets) - 1 < asset_id:
                    over_shoot = asset_id - len(self.assets) + 1
                    while over_shoot > 0:
                        self.assets.append(None)
                        over_shoot -= 1
                self.assets[asset_id] = file
    
    def update_assets(self):
        self.init_assets()
        
    def init_textures(self, height, width):
        self.height_texture = glGenTextures(1)
        glBindTexture(GL_TEXTURE_2D, self.height_texture)
        
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_R32UI,  # internal format
            width,
            height,
            0,
            GL_RED_INTEGER,  # MUST be *_INTEGER
            GL_UNSIGNED_INT,
            None
        )
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)
        
        glBindTexture(GL_TEXTURE_2D, 0)
        glBindImageTexture(0,self.height_texture,0,GL_FALSE,0,GL_READ_WRITE,GL_R32UI)
    
    def init_FBOs(self, width, height, shader_pass):
        shader_pass.assign_fbo()
        glBindFramebuffer(GL_FRAMEBUFFER, shader_pass.fbo)
        
        shader_pass.assign_text()
        glBindTexture(GL_TEXTURE_2D, shader_pass.texture)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, None)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, shader_pass.texture, 0)
        
        shader_pass.assign_info_map()
        glBindTexture(GL_TEXTURE_2D, shader_pass.info_map)
        glTexImage2D(GL_TEXTURE_2D,0,GL_R32UI,width,height,0,GL_RED_INTEGER,GL_UNSIGNED_INT,None)
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                               GL_TEXTURE_2D, shader_pass.info_map, 0)
        
        glDrawBuffers(2, [GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1])
        
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
            array = np.array(parent_array, dtype=np.int32)
            glBufferData(GL_SHADER_STORAGE_BUFFER, array.nbytes, array, GL_DYNAMIC_DRAW)
            
            # Optional: binding point = enum value
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, data_enum.value, buffer_id)
            
            self.buffers[data_enum] = buffer_id
        
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0)
    
    @staticmethod
    def load_shader_program(vertex_path, fragment_path):
        # Helper to read file
        def read_file(path):
            with open(path, "r") as f:
                return f.read()
        
        def include_glsl(path, seen=None):
            if seen is None:
                seen = set()
            
            # avoid including the same file twice
            if path in seen:
                return ""
            seen.add(path)
            
            src = read_file(path)
            final = ""
            
            for line in src.splitlines(True):  # keep newlines
                stripped = line.strip()
                if stripped.startswith("#include"):
                    # get filename
                    inc = stripped.split()[1].strip('"<>')
                    final += include_glsl(inc, seen)
                else:
                    final += line
            
            return final
        
        # Compile a shader
        def compile_shader(source, shader_type):
            shader = glCreateShader(shader_type)
            glShaderSource(shader, source)
            glCompileShader(shader)
            
            if not glGetShaderiv(shader, GL_COMPILE_STATUS):
                log = glGetShaderInfoLog(shader).decode()
                kind = "Vertex" if shader_type == GL_VERTEX_SHADER else "Fragment"
                raise RuntimeError(f"{kind} shader compilation failed:\n{log}")
            return shader
        
        # Read sources
        vertex_src = include_glsl(vertex_path)
        fragment_src = include_glsl(fragment_path)
        
        # Compile shaders
        vertex_shader = compile_shader(vertex_src, GL_VERTEX_SHADER)
        fragment_shader = compile_shader(fragment_src, GL_FRAGMENT_SHADER)
        
        # Create program and link
        program = glCreateProgram()
        glAttachShader(program, vertex_shader)
        glAttachShader(program, fragment_shader)
        glLinkProgram(program)
        
        if not glGetProgramiv(program, GL_LINK_STATUS):
            log = glGetProgramInfoLog(program).decode()
            raise RuntimeError(f"Shader program link failed:\n{log}")
        
        # Cleanup
        glDetachShader(program, vertex_shader)
        glDetachShader(program, fragment_shader)
        glDeleteShader(vertex_shader)
        glDeleteShader(fragment_shader)
        
        return program
    
    def file_type(self, path):
        try:
            with open(path, "r", encoding="utf-8") as f:
                f.read(1)
            return "text"
        except:
            pass
        
        try:
            Image.open(path)
            return "image"
        except:
            pass
        
        return "binary"
    
    def render_update(self):
        self.assets = self.GSG_gui_system.assets
        self.text = self.GSG_gui_system.text
        self.text_set = self.GSG_gui_system.text_set
        self.asset_path = self.GSG_gui_system.asset_path
        self.asset_ids = self.GSG_gui_system.asset_ids
        self.text_ids = self.GSG_gui_system.text_ids
        self.update()
        
    def showEvent(self, event):
        self.update_widget_origin()
        super().showEvent(event)

    def resizeEvent(self, event):
        self.update_widget_origin()
        super().resizeEvent(event)

    def moveEvent(self, event):
        self.update_widget_origin()
        super().moveEvent(event)

    def update_widget_origin(self):
        pos = self.mapToGlobal(self.rect().topLeft())
        win_x_low = pos.x()
        win_y_low = pos.y()
        win_w = self.width()
        win_h = self.height()
        win_x_high = win_x_low + win_w
        win_y_high = win_y_low + win_h
        self.GSG_gui_system.window_top = (win_x_low,win_y_low)
        self.GSG_gui_system.window_bottom = (win_x_high,win_y_high)
    
    def focusInEvent(self, e):
        self.GSG_gui_system.capture_input = True
        super().focusInEvent(e)
    
    def focusOutEvent(self, e):
        self.GSG_gui_system.capture_input = False
        super().focusOutEvent(e)
    
    def showEvent(self, e):
        self.GSG_gui_system.capture_input = True
        super().showEvent(e)
    
    def hideEvent(self, e):
        self.GSG_gui_system.capture_input = False
        super().hideEvent(e)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    win = GSGRenderSystem(None)
    win.resize(800, 600)
    win.show()
    
    # Auto-exit after 2 seconds to test stability
    QTimer.singleShot(2000, app.quit)
    
    app.exec_()
    print("✅ Test finished without crash.")