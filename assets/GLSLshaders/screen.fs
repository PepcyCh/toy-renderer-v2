#version 330 core

in vec2 texcoord;

out vec4 frag_color;

uniform sampler2D color;

void main() {
    vec3 col = texture(color, texcoord).rgb;
    frag_color = vec4(col, 1.0f);
    // frag_color = vec4(texcoord, 0.5f, 1.0f);
}