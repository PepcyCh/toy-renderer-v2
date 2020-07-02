#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 2) in vec2 texcoord_i;

out vec2 texcoord;

void main() {
    gl_Position = vec4(pos.x, pos.y, 0.0f, 1.0f);
    texcoord = texcoord_i;
}