layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec2 in_uv;

uniform sampler2D fbo_tex_pass_map;
uniform sampler2D fbo_tex_pass_basic;
uniform sampler2D fbo_tex_pass_complex;
uniform sampler2D fbo_tex_pass_text;

out vec2 uv;

void main() {
    uv = in_uv;
    gl_Position = vec4(in_pos, 0.0, 1.0);
}