#version 330 core

in vec3 direction;

out vec4 fragColor;

uniform sampler2D equirectangular;

const vec2 invAtan = vec2(0.1591f, 0.3183f);
vec2 SampleSphericalMap(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5f;
    return uv;
}

void main() {
    vec2 uv = SampleSphericalMap(normalize(direction));
    vec3 color = texture(equirectangular, uv).rgb;

    fragColor = vec4(color, 1.0f);
}