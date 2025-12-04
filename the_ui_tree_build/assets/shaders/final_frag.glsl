#version 330 core
in vec2 vUV;
out vec4 FragColor;
uniform sampler2D uPrevPass;

void main() {
    vec4 prevColor = texture(uPrevPass, vUV);

    // if black, replace with blue
    if (prevColor.rgb == vec3(0.0)) {
        prevColor.rgb = vec3(0.0, 0.6, 1.0);
    }

    FragColor = prevColor;
}