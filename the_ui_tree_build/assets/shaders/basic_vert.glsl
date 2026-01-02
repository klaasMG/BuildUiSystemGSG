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

layout(std430, binding = 1) buffer ShaderPass {
    int widget_shaderpass[];
};

layout(std430, binding = 2) buffer Colour {
    int widget_colour[];
};

layout(std430, binding = 3) buffer Shape{
    int widget_shape[];
};

layout(std430, binding = 4) buffer AssetId {
    int widget_assetid[];
};

layout(std430, binding = 5) buffer TextId {
    int widget_textid[];
};

layout(std430, binding = 8) buffer Parent {
    int widget_parent[];
};

int get_position(int index,int offset){
    int position = widget_pos[index * 6 + offset];
    int parent = widget_parent[index];
    while (parent > 0){
        position += widget_pos[parent * 6 + offset];
        parent = widget_parent[parent];
    }
    return position;
}

ivec4 get_colour(int index){
    return ivec4(widget_colour[index * 4],widget_colour[index * 4 + 1],widget_colour[index * 4 + 2],widget_colour[index * 4 + 3]);
}

void main() {
    vIndex = gl_VertexID;
    Widget w = Widget(ivec3(get_position(vIndex,0),get_position(vIndex,1),get_position(vIndex,2)),ivec3(get_position(vIndex,3),get_position(vIndex,4),get_position(vIndex,5)),
                      widget_shaderpass[vIndex],get_colour(vIndex),widget_shape[vIndex], widget_parent[vIndex],widget_assetid[vIndex],widget_textid[vIndex]);
    widget = w;
    vUV = aUV;
    vec4 Position = vec4(pos_to_ndc(640,center_pos(w.pos_one.x, w.pos_two.x)),pos_to_ndc(480,center_pos(w.pos_one.y, w.pos_two.y)), 0.0, 1.0);
    if (w.pos_one.x == -1){
        Position = vec4(0);
    }
    gl_Position = Position;
    //gl_Position = vec4(0,0,0,1);
    int x_lenght = get_length(w.pos_one.x, w.pos_one.y);
    int y_lenght = get_length(w.pos_two.x, w.pos_two.y);
    int point_size = max(x_lenght,y_lenght);
    gl_PointSize = float(point_size);
}