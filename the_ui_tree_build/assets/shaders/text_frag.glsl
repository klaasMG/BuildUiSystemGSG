#version 430 core
out vec4 FragColor;
flat in uint vWidgetIndex;
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
    FragColor = vec4(1.0); // white
}
