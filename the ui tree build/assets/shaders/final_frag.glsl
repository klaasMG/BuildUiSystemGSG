#version 430 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D fbo_tex_map;
uniform sampler2D fbo_tex_basic;
uniform sampler2D fbo_tex_complex;
uniform sampler2D fbo_tex_text;

void main() {
    vec4 mapCol     = texture(fbo_tex_map, TexCoord);
    vec4 basicCol   = texture(fbo_tex_basic, TexCoord);
    vec4 complexCol = texture(fbo_tex_complex, TexCoord);
    vec4 textCol    = texture(fbo_tex_text, TexCoord);

    // simple mix – replace with whatever blending you want
    FragColor = mapCol + basicCol + complexCol + textCol;
}
