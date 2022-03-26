#version 330 core

out vec4 colour;

uniform vec4 u_Colour;

void main() {
    colour = u_Colour;
}
