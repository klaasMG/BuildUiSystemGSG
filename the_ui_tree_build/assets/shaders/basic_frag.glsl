#version 430 core
in vec2 vUV;
out vec4 FragColor;

layout(std430, binding = 0) buffer Position {
    int widget_pos[];
};

void main() {
    vec3 colour = vec3(0,0,0);
    if (widget_pos[6] != -1){
        colour = vec3(1,0,0);
    }
    else{
        colour = vec3(0,0,0);
    }
    FragColor = vec4(colour, 1.0);
}