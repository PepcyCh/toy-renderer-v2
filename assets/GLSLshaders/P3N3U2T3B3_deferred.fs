#version 330 core

in VS_OUT {
    vec3 pos;
    vec3 normal;
    vec2 texcoord;
    vec3 tangent;
    vec3 bitangent;
} vs_out;

layout (location = 0) out vec4 g_pos;
layout (location = 1) out vec3 g_normal;
layout (location = 2) out vec3 g_albedo;
layout (location = 3) out vec3 g_ambient;
layout (location = 4) out vec4 g_spec;

// ambient
uniform int ambient_b;
uniform vec3 ambient_c;
uniform sampler2D ambient_i;
// albedo (diffuse)
uniform int albedo_b;
uniform vec3 albedo_c;
uniform sampler2D albedo_i;
// specular
uniform int specular_b;
uniform vec3 specular_c;
uniform sampler2D specular_i;
// normal
uniform int normal_b;
uniform sampler2D normal_i;
// exponent
uniform vec3 exponent_c;

void main() {
    // position (and linear depth)
    float NEAR = 0.1f, FAR = 100.f;
    float depth = gl_FragCoord.z * 2.0f - 1.0f;
    depth = (2.0 * NEAR * FAR) / (FAR + NEAR - depth * (FAR - NEAR));
    g_pos = vec4(vs_out.pos, depth);

    // normal
    vec3 normal = vs_out.normal;
    if (normal_b != 0) {
        vec3 normal_t = texture(normal_i, vs_out.texcoord).rgb;
        normal_t = normal_t * 2.0f - 1.0f;
        normal = normalize(normal_t);
        mat3 TBN = mat3(vs_out.tangent, vs_out.bitangent, vs_out.normal);
        normal = normalize(TBN * normal);
    }
    g_normal = normal;

    // albedo
    vec3 albedo = albedo_c;
    if (albedo_b != 0) albedo = texture(albedo_i, vs_out.texcoord).rgb;
    g_albedo = albedo;

    // ambient
    vec3 ambient = ambient_c;
    if (ambient_b != 0) ambient = texture(ambient_i, vs_out.texcoord).rgb;
    g_ambient = ambient;

    // specular - exponent
    vec3 specular = specular_c;
    if (specular_b != 0) specular = texture(specular_i, vs_out.texcoord).rgb;
    float exp = exponent_c.x;
    g_spec = vec4(specular, exp);
}