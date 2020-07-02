#version 330 core

in vec4 frag_pos;

uniform vec3 cam_pos;

void main() {
    float far = 100.0f;
    float dist = length(frag_pos.xyz - cam_pos);
    gl_FragDepth = dist / far;
}