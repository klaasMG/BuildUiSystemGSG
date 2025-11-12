#version 430 core
layout(location = 0) in vec3 aPos;
flat out uint vWidgetIndex;
layout(std430, binding = 0) buffer WidgetDataPositiom {
    int pos_data[];
};

layout(std430, binding = 1) buffer WidgetDataShaderPass{
    int pass_data[];
};

layout(std430, binding = 2) buffer WidgetDataColour{
    int col_data[];
};

layout(std430, binding = 3) buffer WidgetDataShape{
    int shape_data[];
};

layout(std430, binding = 4) buffer WidgetDataAssetID{
    int asset_id_data[];
};

layout(std430, binding = 5) buffer WidgetDataTextId{
    int text_id_data[];
};

layout(std430, binding = 6) buffer WidgetDataParent{
    int parent_data[];
};
void main() {
    uint id = gl_VertexID;      // auto counts 0..N-1
    vWidgetIndex = id;
    gl_Position = vec4(aPos, 1.0);
}