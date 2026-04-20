import numpy as np
from OpenGL.GL import *
from Uniform_Registry import uniform_registry, UniformTypes
from enum import Enum
from PIL import Image

def debug_read_state():
    # 1. which FBO is bound for reading
    read_fbo = glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING)
    print("READ_FBO:", read_fbo)

    # 2. which attachment is selected
    read_buf = glGetIntegerv(GL_READ_BUFFER)
    print("READ_BUFFER:", read_buf)

    # 3. what object is attached there
    obj_type = glGetFramebufferAttachmentParameteriv(
        GL_FRAMEBUFFER,
        read_buf,
        GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE
    )
    obj_name = glGetFramebufferAttachmentParameteriv(
        GL_FRAMEBUFFER,
        read_buf,
        GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME
    )

    print("ATTACHMENT_TYPE:", obj_type)   # GL_TEXTURE / GL_RENDERBUFFER / NONE
    print("ATTACHMENT_ID:", obj_name)

    # 4. if it's a texture → check format
    if obj_type == GL_TEXTURE and obj_name != 0:
        glBindTexture(GL_TEXTURE_2D, obj_name)
        internal = glGetTexLevelParameteriv(
            GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT
        )
        print("INTERNAL_FORMAT:", internal)
    else:
        print("No texture bound to this attachment")

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