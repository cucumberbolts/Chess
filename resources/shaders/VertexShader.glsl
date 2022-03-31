#version 450 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in float textureSlot;

out vec4 v_Colour;
out vec2 v_TexCoord;
out float v_TextureSlot;

uniform mat4 u_MVP;

void main() {
    gl_Position = u_MVP * position;
    v_TexCoord = texCoord;
    v_TextureSlot = textureSlot;
    v_Colour = colour;
}
