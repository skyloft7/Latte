#version 460

layout(location = 0) in vec4 inputPos;


uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * transform * inputPos;
}