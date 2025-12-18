#version 430 core
#include "C:/Users/klaas/PycharmProjects/BuildUiSystemGSG/the_ui_tree_build/assets/shaders/widget_builder.glsl"
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;

out vec2 vUV;
flat out int vIndex;
flat out Widget widget;

layout(std430, binding = 0) buffer Position {
    int widget_pos[];
};

layout(std430, binding = 8) buffer Parent {
    int widget_parent[];
};

int get_position(int index,int offset){
    int position = widget_pos[index * 6 + offset];
    int parent = widget_parent[index];
    while (parent != 0){
        position += widget_pos[parent * 6 + offset];
        parent = widget_parent[parent];
    }
    return position;
}
void main() {
    vIndex = gl_VertexID;
    Widget w = Widget(ivec3(get_position(vIndex,0),get_position(vIndex,1),get_position(vIndex,2)),ivec3(get_position(vIndex,3),get_position(vIndex,4),get_position(vIndex,5)),1,ivec4(1,1,1,255),2, 3);
    widget = w;
    vUV = aUV;
    //gl_Position = vec4(pos_to_ndc(640,w.pos_one.x),pos_to_ndc(480,w.pos_one.y), 0.0, 1.0);
    gl_Position = vec4(0,0,0,0);
    gl_PointSize = 50.0;
}