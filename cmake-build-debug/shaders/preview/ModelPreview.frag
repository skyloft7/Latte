#version 460

layout(location = 2) in vec3 inputColor;


void main() {
    gl_FragColor = vec4(inputColor.xyz, 1.0);
}