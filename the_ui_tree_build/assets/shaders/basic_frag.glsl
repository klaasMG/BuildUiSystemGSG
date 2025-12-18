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
    ivec4 colour_255 = widget.colour;
    vec4 colour = vec4(col_to_ndc(colour_255.x),col_to_ndc(colour_255.y),col_to_ndc(colour_255.z),col_to_ndc(colour_255.w));
    //colour = vec4(colour_255.x,colour_255.y,colour_255.z,colour_255.w);
    FragColor = vec4(colour);
}