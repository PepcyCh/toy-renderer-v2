#version 330 core

in vec2 texcoord;

out vec4 frag_color;

// G buffers
uniform sampler2D g_normal;

void main() {
    // get values from G buffers
    vec3 normal = texture(g_normal, texcoord).rgb;

    // discard frag that with nothing on it
    if (length(normal) < 0.8) {
        discard;
    }

    vec3 result = (normal + vec3(1.0f)) / 2.0f;
    frag_color = vec4(result, 1.0f);
}