#version 450 core

out vec4 o_Colour;

in vec4 v_Colour;
in vec2 v_TexCoord;
in float v_TextureSlot;

uniform sampler2D u_Textures[32];

void main() {
    int index = int(v_TextureSlot);
    if (index == 0) {
        o_Colour = v_Colour;
    } else {
        o_Colour = texture(u_Textures[index], v_TexCoord) * v_Colour;
    }
}
