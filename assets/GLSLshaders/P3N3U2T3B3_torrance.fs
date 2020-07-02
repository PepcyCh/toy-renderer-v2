#version 330 core

in VS_OUT {
    vec3 pos;
    vec3 normal;
    vec2 texcoord;
    vec3 tangent;
    vec3 bitangent;
} vs_out;

out vec4 fragColor;

// lights
const int MAX_LIGHT = 10;
uniform int nLight;
uniform vec3 viewPos;
uniform vec3 lightPos[MAX_LIGHT];
uniform vec3 lightColor[MAX_LIGHT];
// ambient
uniform int ambient_b;
uniform vec3 ambient_c;
uniform sampler2D ambient_i;
// ao
uniform int ao_b;
uniform sampler2D ao_i;
// albedo
uniform int albedo_b;
uniform vec3 albedo_c;
uniform sampler2D albedo_i;
// normal
uniform int normal_b;
uniform sampler2D normal_i;
// metallic
uniform int metallic_b;
uniform vec3 metallic_c;
uniform sampler2D metallic_i;
// roughness
uniform int roughness_b;
uniform vec3 roughness_c;
uniform sampler2D roughness_i;
// environment cubemap
uniform int hasIrradMap;
uniform samplerCube irradMap;
// reflection cubemap
uniform int hasReflMap;
uniform samplerCube reflMap;
// BRDF LUT
uniform sampler2D BRDFLUT;

const float PI = 3.141592653589793238463f;

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0f - F0) * pow(1.0 - cosTheta, 5.0f);
}
vec3 fresnelSchlickR(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0f - roughness), F0) - F0) *
            pow(1.0f - cosTheta, 5.0f);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;

    float nom = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

void main() {
    // calc normal
    vec3 normal = normalize(vs_out.normal);
    if (normal_b != 0) {
        vec3 normal_t = texture(normal_i, vs_out.texcoord).rgb;
        normal_t = normal_t * 2.0f - 1.0f;
        normal = normalize(normal_t);
        mat3 TBN = mat3(vs_out.tangent, vs_out.bitangent, vs_out.normal);
        normal = normalize(TBN * normal);
    }
    vec3 N = normal;

    // get values according to XXX_b
    vec3 albedo = albedo_c;
    if (albedo_b != 0) albedo = texture(albedo_i, vs_out.texcoord).rgb;
    float metallic = metallic_c.r;
    if (metallic_b != 0) metallic = texture(metallic_i, vs_out.texcoord).r;
    float roughness = roughness_c.r;
    if (roughness_b != 0) roughness = texture(roughness_i, vs_out.texcoord).r;
    float ao = 1.0f;
    if (ao_b != 0) ao = texture(ao_i, vs_out.texcoord).r;

    // light
    vec3 Lo = vec3(0.0f);
    vec3 V = normalize(viewPos - vs_out.pos);
    vec3 R = reflect(-V, N);

    vec3 F0 = vec3(0.04f);
    F0 = mix(F0, albedo, metallic);

    // direct lights
    for (int i = 0; i < nLight && i < MAX_LIGHT; i++) {
        vec3 L = normalize(lightPos[i] - vs_out.pos);
        vec3 H = normalize(V + L);
        float dist = length(L);
        float atten = 1.0f / (dist * dist);
        vec3 radiance = lightColor[i] * atten;

        vec3 F = fresnelSchlick(max(dot(H, V), 0.0f), F0);
        float D = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);

        vec3 nom = D * G * F;
        float denom = 4.0f * max(dot(N, V), 0.0f) *
                max(dot(N, L), 0.0f) + 0.001f;
        vec3 specular = nom / denom;

        vec3 kS = F;
        vec3 kD = vec3(1.0f) - kS;
        kD *= 1.0f - metallic;

        float NdotL = max(dot(N, L), 0.0f);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    // ambient
    vec3 ambient = vec3(0.0f);
    if (hasIrradMap != 0) {
        vec3 F = fresnelSchlickR(max(dot(N, V), 0.0f), F0, roughness);
        vec3 kS = F;
        vec3 kD = 1.0f - kS;
        kD *= 1.0f - metallic;
        vec3 irrad = texture(irradMap, N).rgb;
        vec3 diffuse = irrad * albedo;

        vec3 refl = textureLod(reflMap, R, roughness * 4).rgb;
        vec2 BRDF = texture(BRDFLUT, vec2(max(dot(N, V), 0.0f), roughness)).rg;
        vec3 specular = refl * (F * BRDF.x + BRDF.y);

        ambient = (kD * diffuse + specular) * ao;
    } else {
        ambient = ambient_c;
        if (ambient_b != 0) ambient = texture(ambient_i, vs_out.texcoord).rgb;
    }

    vec3 result = ambient + Lo;
    result /= (result + 1.0f);
    result = pow(result, vec3(1.0f / 2.2f));
    fragColor = vec4(result, 1.0f);
}