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
    // haal kleur van widget en zet naar 0..1
    ivec4 colour_255 = widget.colour;
    vec4 colour = vec4(
        col_to_ndc(colour_255.x),
        col_to_ndc(colour_255.y),
        col_to_ndc(colour_255.z),
        col_to_ndc(colour_255.w)
    );

    vec2 FragPos = gl_FragCoord.xy;

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