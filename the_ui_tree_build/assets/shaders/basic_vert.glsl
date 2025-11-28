#version 430 core
layout(location = 0) in vec3 aPos;
flat out uint vWidgetIndex;
flat out int shape_x;
flat out int shape_y;
uniform int shader_pass_right;
uniform uvec2 screen_size;
layout(std430, binding = 0) buffer WidgetDataPositiom{
    int pos_data[];
};
layout(std430, binding = 1) buffer WidgetDataShaderPass{
    int pass_data[];
};
layout(std430, binding = 6) buffer WidgetDataParent{
    int parent_data[];
};
void main() {
    uint screen_x = screen_size.x;
    uint screen_y = screen_size.y;
    uint id = gl_VertexID;      // auto counts 0..N-1
    vWidgetIndex = id;

    gl_Position = vec4(
    float(pos_data[vWidgetIndex * 6]) / float(screen_x),
    float(pos_data[vWidgetIndex * 6 + 1]) / float(screen_y),
    float(pos_data[vWidgetIndex * 6 + 2]) / 255.0,
    1.0
    );

    int shader_pass = pass_data[id];

    if (shader_pass_right - shader_pass == 0){
    int Widget_width = pos_data[id * 6 + 3] - pos_data[id * 6];
    int Widget_heigth = pos_data[id * 6 + 4] - pos_data[id * 6 + 1];

    int point_size;
    if (Widget_width > Widget_heigth){
        point_size = Widget_width;
    }
    else {
        point_size = Widget_heigth;
    }
    shape_x = Widget_width;
    shape_y = Widget_heigth;
    gl_Position = vec4(aPos, 1.0);
    gl_PointSize = float(point_size);
    }
}