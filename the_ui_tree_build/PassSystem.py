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