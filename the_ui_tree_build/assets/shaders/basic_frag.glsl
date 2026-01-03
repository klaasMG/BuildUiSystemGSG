#version 430 core
#include "C:/Users/klaas/PycharmProjects/BuildUiSystemGSG/the_ui_tree_build/assets/shaders/widget_builder.glsl"

uniform sampler2D uAtlas;

in vec2 vUV;
flat in int vIndex;
out vec4 FragColor;
flat in Widget widget;


layout(std430, binding = 0) buffer Position {
    int widget_pos[];
};

void main() {
    // haal kleur van widget en zet naar 0..1
    ivec4 colour_255 = widget.colour;
    vec4 colour = vec4(0);
    vec2 FragPos = gl_FragCoord.xy;
    int pixel_pos_x = 0;
    int pixel_pos_y = 0;
    if (widget.asset_id == -1){
        colour = vec4(
        col_to_ndc(colour_255.x),
        col_to_ndc(colour_255.y),
        col_to_ndc(colour_255.z),
        col_to_ndc(colour_255.w));
    }
    else {
        ivec2 FragPosInt = ivec2(int(FragPos.x),int(FragPos.y));
        int asset_id = widget.asset_id;
        int pixel_x = FragPosInt.x - widget.pos_one.x;
        int pixel_y = FragPosInt.y - widget.pos_one.y;
        pixel_pos_x = 256 * asset_id + pixel_x;
        int image_row = asset_id / 32;
        pixel_pos_x = pixel_pos_x - (image_row * 256);
        pixel_pos_y = pixel_y;
        pixel_pos_y = 8191 - pixel_pos_y;
        ivec2 pixel = ivec2(pixel_pos_x,pixel_pos_y);
        colour = texelFetch(uAtlas, pixel, 0);
    }

    if (FragPos.x < widget.pos_one.x) {
        colour = vec4(0,0,0,0);
    }
    if (FragPos.x > widget.pos_two.x) {
        colour = vec4(0,0,0,0);
    }
    if (FragPos.y < widget.pos_one.y) {
        colour = vec4(0,0,0,0);
    }
    if (FragPos.y > widget.pos_two.y) {
        colour = vec4(0,0,0,0);
    }

    FragColor = colour;
}