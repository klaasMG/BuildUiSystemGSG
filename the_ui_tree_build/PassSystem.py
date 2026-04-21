import numpy as np
from OpenGL.GL import *
from Uniform_Registry import uniform_registry, UniformTypes
from enum import Enum
from PIL import Image
from ui_debug import debug_func
from hold_lock import HoldLock

def gl_state_print():
    print("---- GL STATE ----")

    print("Program:", glGetIntegerv(GL_CURRENT_PROGRAM))
    print("VAO:", glGetIntegerv(GL_VERTEX_ARRAY_BINDING))

    print("Array Buffer:", glGetIntegerv(GL_ARRAY_BUFFER_BINDING))
    print("Element Buffer:", glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING))

    print("Framebuffer:", glGetIntegerv(GL_FRAMEBUFFER_BINDING))
    print("Read FBO:", glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING))
    print("Draw FBO:", glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING))

    print("Texture 2D:", glGetIntegerv(GL_TEXTURE_BINDING_2D))
    print("Active Texture:", glGetIntegerv(GL_ACTIVE_TEXTURE) - GL_TEXTURE0)

    print("Viewport:", glGetIntegerv(GL_VIEWPORT))

    print("Blend:", glIsEnabled(GL_BLEND))
    print("Depth Test:", glIsEnabled(GL_DEPTH_TEST))
    print("Cull Face:", glIsEnabled(GL_CULL_FACE))

    print("---- END ----")

debug_gl_state_print = debug_func(gl_state_print)

class TextureType(Enum):
    RGBA = "RGBA"
    GREY_SCALE = "L"

class PBODoubleBuffer:
    """This thing only works on the basic shader pass it rellies on the second image"""
    def __init__(self, width, height):
        self.width = width
        self.height = height

        self.size = self.width * self.height * 4
        self.pbos = glGenBuffers(2)
        self.index = 0

        for pbo in self.pbos:
            glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo)
            glBufferData(GL_PIXEL_PACK_BUFFER, self.size, None, GL_STREAM_READ)

        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0)

    def read_frame(self,x=0, y=0):
        read_pbo = self.pbos[self.index]
        map_pbo = self.pbos[1 - self.index]

        glBindBuffer(GL_PIXEL_PACK_BUFFER, read_pbo)

        glReadBuffer(GL_COLOR_ATTACHMENT1)
        try:
            #data = np.zeros((self.height, self.width), dtype=np.uint32)
            glReadPixels(x, y, self.width, self.height,
                     GL_RED_INTEGER, GL_UNSIGNED_INT, None)
        except Exception as e:
            print(self.width, self.height)
            print(e)

        glBindBuffer(GL_PIXEL_PACK_BUFFER, map_pbo)

        data = glGetBufferSubData(GL_PIXEL_PACK_BUFFER, 0, self.size)

        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0)

        self.index = 1 - self.index
        return data

def unpack_u16(packed: int) -> tuple[int, int]:
    a = packed & 0xFFFF
    b = (packed >> 16) & 0xFFFF
    return a, b

class CpuFrame:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.frame_data_height_id = np.zeros((self.width , self.height), dtype=np.uint32)
        self.frame_lock = HoldLock()

    def resize(self, width, height):
        self.width = width
        self.height = height
        locked = self.frame_lock.lock()
        if locked:
            self.frame_data_height_id = np.zeros(self.width * self.height, dtype=np.uint32)
        self.frame_lock.release()

    def get_pixel_data(self, x: int, y :int):
        locked = self.frame_lock.lock()
        height_id = 0
        if locked:
            height_id = self.frame_data_height_id[y, x]
        self.frame_lock.release()
        height ,widget_id = unpack_u16(height_id)
        return height, widget_id

    def read_in_frame(self):
        locked: bool = self.frame_lock.lock()
        if locked:
            glPixelStorei(GL_PACK_ALIGNMENT, 1)
            glReadBuffer(GL_COLOR_ATTACHMENT1)
            glReadPixels(0, 0, self.width, self.height,GL_RED_INTEGER, GL_UNSIGNED_INT, self.frame_data_height_id)
        self.frame_lock.release()

class ShaderPassData:
    def __init__(self,frag_shader, vert_shader):
        self.frag_shader = frag_shader
        self.vert_shader = vert_shader
        self.program = None
        self.vbo = None
        self.vao = None
        self.fbo = None
        self.texture = None
        self.info_map = None
        self.pbo_double_buffer: None | PBODoubleBuffer = None
        self.size: tuple[int, int] | None = None

    def set_size(self, width, height):
        self.size = (width, height)

    def set_pbo_double_buffer(self):
        self.pbo_double_buffer = PBODoubleBuffer(self.size[0], self.size[1])

    def load(self, renderer):
        """Use your renderer's loader function to compile and link the shader"""
        self.program = renderer.load_shader_program(self.vert_shader, self.frag_shader)

    def assign_vbo(self):
        self.vbo = glGenBuffers(1)

    def assign_vao(self):
        self.vao = glGenVertexArrays(1)

    def use_vbo(self, vbo):
        self.vbo = vbo

    def assign_fbo(self):
        self.fbo = glGenFramebuffers(1)

    def assign_text(self):
        self.texture = glGenTextures(1)

    def assign_info_map(self):
        self.info_map = glGenTextures(1)

    @staticmethod
    def set_uniform(name, program):
        uniform_registry.set_uniform(name, program)

    def set_atlas(self, name, program):
        self.set_uniform(name, program)
    
class NotATextureError(Exception):
    pass

def set_glActiveTexture(name: str):
    texture_binding = uniform_registry.get_binding(name)
    if texture_binding == -1:
        raise NotATextureError(f"this is {name} not a texture")
    glActiveTexture(GL_TEXTURE0 + texture_binding)

class Texture:
    def __init__(self, image, name, image_type):
        self.image_type: TextureType = image_type
        self.name = name
        uniform_registry.register_uniform(self.name, UniformTypes.Texture)
        self.texture = glGenTextures(1)
        self.bind_texture()
        self.set_data()
        self.upload(image)

    def bind_texture(self):
        set_glActiveTexture(self.name)
        glBindTexture(GL_TEXTURE_2D, self.texture)

    def set_data(self):
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)

    def upload(self, image: Image.Image):
        self.check_image_type(image)
        if self.image_type == TextureType.RGBA:
            internal = GL_RGBA
            fmt = GL_RGBA
        elif self.image_type == TextureType.GREY_SCALE:
            internal = GL_R8
            fmt = GL_RED
        else:
            raise NotImplementedError("This can not happen ever")
        w, h = image.size
        pixels = image.tobytes()
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1)

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            internal,
            w,
            h,
            0,
            fmt,
            GL_UNSIGNED_BYTE,
            pixels
        )
    
    def check_image_type(self , image: Image.Image):
        if self.image_type.value != image.mode:
            raise Exception(f"can not pass {image.mode} to this Texture with: {self.image_type.value}")
        
    def resend(self, image):
        self.bind_texture()
        self.upload(image)