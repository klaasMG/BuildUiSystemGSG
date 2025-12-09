#version 430 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;

out vec2 vUV;
flat out int vIndex;
flat out Widget widget;
#include "C:/Users/klaas/PycharmProjects/BuildUiSystemGSG/the_ui_tree_build/assets/shaders/widget_builder.glsl"
void main() {
    vIndex = gl_VertexID;
    vUV = aUV;
    gl_Position = vec4(aPos, 0.0, 1.0);
}