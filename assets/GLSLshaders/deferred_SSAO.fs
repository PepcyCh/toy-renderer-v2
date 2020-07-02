#version 330 core

in vec2 uv;

out float fragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D noises; // 4x4
const int MAX_SAMPLE = 64;
uniform vec3 samples[MAX_SAMPLE];
uniform mat4 projection;
uniform mat4 view;
uniform int screenWidth;
uniform int screenHeight;

void main() {
    vec2 noiseScale = vec2(screenWidth / 4.0f, screenHeight / 4.0f);

    vec3 pos = texture(gPosition, uv).rgb;
    vec3 normal = texture(gNormal, uv).rgb;
    vec3 randomVec = texture(noises, uv * noiseScale).rgb;

    if (length(normal) < 0.8) {
        fragColor = 1.0f;
    } else {
        vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
        vec3 bitangent = cross(normal, tangent);
        mat3 TBN = mat3(tangent, bitangent, normal);
        vec3 pos_t = vec3(view * vec4(pos, 1.0f));
        TBN = mat3(view) * TBN;

        float occlusion = 0.0f;
        for (int i = 0; i < MAX_SAMPLE; i++) {
            vec3 samplePos = pos_t + TBN * samples[i];

            vec4 offset = vec4(samplePos, 1.0f);
            offset = projection * offset;
            offset.xyz /= offset.w;
            offset.xyz = (offset.xyz + vec3(1.0f)) / 2.0f;

            float sampleDepth = -texture(gPosition, offset.xy).w;

            float coe = smoothstep(0.0f, 1.0f,
                    1.0f / (abs(pos_t.z - sampleDepth) + 0.001f));
            occlusion += (sampleDepth >= samplePos.z ? 1.0 : 0.0) * coe;
        }
        occlusion = 1.0f - occlusion / MAX_SAMPLE;
        fragColor = occlusion;
    }
}