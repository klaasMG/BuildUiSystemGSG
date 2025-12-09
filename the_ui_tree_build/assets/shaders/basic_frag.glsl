#version 430 core
#include "C:/Users/klaas/PycharmProjects/BuildUiSystemGSG/the_ui_tree_build/assets/shaders/widget_builder.glsl"
in vec2 vUV;
flat in int vIndex;
out vec4 FragColor;
flat in Widget widget;
layout(std430, binding = 0) buffer Position {
    int widget_pos[];
};

void main() {
    vec3 colour = vec3(0,0,0);
    if (widget_pos[6] != -1){
        colour = vec3(1,0,1);
    }
    else{
        colour = vec3(0,0,0);
    }
    FragColor = vec4(colour, 1.0);
}