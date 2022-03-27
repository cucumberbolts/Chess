#version 430 core

in vec2 v_TexCoord;

out vec4 colour;

uniform vec4 u_Colour;
uniform sampler2D u_Texture;

void main() {
    colour = texture(u_Texture, v_TexCoord);// * u_Colour;
}
