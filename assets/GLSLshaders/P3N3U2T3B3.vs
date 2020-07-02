#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out VS_OUT {
    vec3 pos;
    vec3 normal;
    vec2 texcoord;
    vec3 tangent;
    vec3 bitangent;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 model_TI;

void main() {
    mat3 model_TI3 = mat3(model_TI);
    gl_Position = projection * view * model * vec4(pos, 1.0f);
    vs_out.pos = vec3(model * vec4(pos, 1.0f));
    vs_out.normal = normalize(model_TI3 * normal);
    vs_out.texcoord = texcoord;
    vs_out.tangent = normalize(model_TI3 * tangent);
    vs_out.bitangent = normalize(model_TI3 * bitangent);
}