from OpenGL.GL import *

class ShaderPassData:
    def __init__(self,frag_shader, vert_shader):
        self.frag_shader = frag_shader
        self.vert_shader = vert_shader
        self.program = None
        self.vbo = None
        self.vao = None
        self.fbo = None
        self.texture = None
    
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
        
    def set_uniform(self, name, reference):
        loc = glGetUniformLocation(self.program, name)
        print(loc)
        glUniform1i(loc, reference)
        
    def set_atlas(self):
        self.set_uniform("uAtlas", 1)
    

class Texture:
    def __init__(self, image):
        self.texture = glGenTextures(1)
        self.bind_texture(0)
        self.set_data()
        self.upload(image)

    def bind_texture(self, unit=0):
        glActiveTexture(GL_TEXTURE0 + unit + 1)
        glBindTexture(GL_TEXTURE_2D, self.texture)

    def set_data(self):
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)

    def upload(self, image):
        image = image.convert("RGBA")
        w, h = image.size
        pixels = image.tobytes()

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1)

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA8,
            w,
            h,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            pixels
        )

    def resend(self, image):
        self.bind_texture(0)
        self.upload(image)