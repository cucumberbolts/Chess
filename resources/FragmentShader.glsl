#version 330 core

out vec4 colour;

in vec4 v_Colour;

void main() {
    colour = v_Colour + 0.5;
}
