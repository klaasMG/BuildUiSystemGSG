from OpenGL.GL import *
from Uniform_Registry import uniform_registry, UniformTypes
from enum import Enum ,auto
from PIL import Image

class TextureType(Enum):
    RGBA = "RGBA"
    GREY_SCALE = "L"

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
        
    def set_uniform(self, name, program):
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

class PBODoubleBuffer:
    """This thing only works on the basic shader pass it rellies on the second image"""
    def __init__(self, width, height, data_bytes_num):
        self.width = width
        self.height = height
        self.data_bytes_num = data_bytes_num

        self.size = self.width * self.height * self.data_bytes_num
        self.pbos = glGenBuffers(2)
        self.index = 0

        for pbo in self.pbos:
            glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo)
            glBufferData(GL_PIXEL_PACK_BUFFER, self.size, None, GL_STREAM_READ)

        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0)

    def read_frame(self, x=0, y=0):
        read_pbo = self.pbos[self.index]
        map_pbo = self.pbos[1 - self.index]

        # 1. start async read into GPU buffer
        glBindBuffer(GL_PIXEL_PACK_BUFFER, read_pbo)
        glReadBuffer(GL_COLOR_ATTACHMENT1)
        glReadPixels(x, y, self.width, self.height, GL_RED_INTEGER, GL_UNSIGNED_INT, None)

        # 2. map previous buffer (CPU read)
        glBindBuffer(GL_PIXEL_PACK_BUFFER, map_pbo)
        ptr = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY)

        data = None
        if ptr:
            data = glGetBufferSubData(GL_PIXEL_PACK_BUFFER, 0, self.size)
            glUnmapBuffer(GL_PIXEL_PACK_BUFFER)

        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0)

        # swap
        self.index = 1 - self.index

        return data