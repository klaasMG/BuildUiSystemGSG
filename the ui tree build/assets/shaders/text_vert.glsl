#version 430 core
layout(location = 0) in vec3 aPos;
flat out uint vWidgetIndex;

void main() {
    uint id = gl_VertexID;      // auto counts 0..N-1
    vWidgetIndex = id;
    gl_Position = vec4(aPos, 1.0);
}
