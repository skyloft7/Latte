#version 460

layout(location = 0) in vec4 inputPos;
layout(location = 1) in vec3 inputColor;

layout(location = 2) out vec3 outputColor;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * transform * inputPos;
    outputColor = inputColor;
}