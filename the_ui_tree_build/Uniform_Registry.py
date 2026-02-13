from enum import Enum, auto
from OpenGL.GL import *

class RegisterError(Exception):
    pass

class UniformTypes(Enum):
    Texture = auto(),
    Int = auto(),

class Uniform:
    def __init__(self, uniform_type):
        self.type = uniform_type
        self.binding: int | None = None
    
    def set_binding(self, binding):
        self.binding = binding

class UniformRegistry:
    def __init__(self):
        self.uniforms: dict[str, Uniform] = dict()
        self.next_uniform_id = 0
    
    def register_uniform(self, name: str, uniform_type: UniformTypes):
        uniform_new = Uniform(uniform_type)
        if uniform_type == UniformTypes.Texture:
            uniform_new.set_binding(self.next_uniform_id)
            self.next_uniform_id += 1
        self.uniforms[name] = uniform_new
        
    def set_uniform(self, name, program):
        print(";lfllfk")
        uniform = self.uniforms[name]
        print(uniform)
        reference = uniform.binding
        print(reference)
        if reference is None:
            raise RegisterError(f"This uniform {name} is not registered")
        glUseProgram(program)
        if uniform.type == UniformTypes.Texture:
            loc = glGetUniformLocation(program, name)
            glUniform1i(loc, reference)
            
    def get_binding(self, name: str)->int:
        uniform: Uniform = self.uniforms[name]
        if (uniform.type != UniformTypes.Texture) or (uniform.binding is None):
            return -1
        return uniform.binding
        
uniform_registry = UniformRegistry()