#version 330 core

in vec3 direction;

out vec4 frag_color;

uniform samplerCube cubemap;

void main() {
    frag_color = texture(cubemap, direction);
    frag_color = pow(frag_color, vec4(1.0f / 2.2f));
}