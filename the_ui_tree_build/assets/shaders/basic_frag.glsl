#version 430 core
out vec4 FragColor;
flat in uint vWidgetIndex;
flat in int shape_x;
flat in int shape_y;

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
    vec4 colour = vec4(1.0);
    if (gl_FragCoord.x < pos_data[vWidgetIndex * 6 + 3] && gl_FragCoord.y < pos_data[vWidgetIndex * 6 + 4]){
        colour = vec4((col_data[vWidgetIndex * 4]) / 255, (col_data[vWidgetIndex * 4 + 1]) / 255, (col_data[vWidgetIndex * 4 + 2]) / 255, (col_data[vWidgetIndex * 4 + 3]) / 255);
    }
    else{
        colour = vec4(0.0);
    }
    FragColor = colour; // white
}
