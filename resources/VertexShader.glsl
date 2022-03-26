#version 330 core

layout(location = 0) in vec4 position;

out vec4 v_Colour;

void main() {
    gl_Position = position;
    v_Colour = gl_Position;
}
