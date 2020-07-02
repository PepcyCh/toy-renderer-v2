#version 330 core

in VS_OUT {
    vec3 pos;
    vec3 normal;
    vec2 texcoord;
    vec3 tangent;
    vec3 bitangent;
} vs_out;

out vec4 frag_color;

// lights
const int MAX_LIGHT = 16;
uniform int n_dir_lights;
struct DirLight {
    vec3 color;
    vec3 dir;
    int shadow;
    mat4 mat;
    sampler2D depth;
};
uniform DirLight dir_lights[MAX_LIGHT];
uniform int n_point_lights;
struct PointLight {
    vec3 color;
    vec3 pos;
    float kq;
    float kl;
    float kc;
    int shadow;
    samplerCube depth;
};
uniform PointLight point_lights[MAX_LIGHT];
uniform int n_spot_lights;
struct SpotLight {
    vec3 color;
    vec3 pos;
    vec3 dir;
    float kq;
    float kl;
    float kc;
    float cutoff;
    float outer_cutoff;
    int shadow;
    mat4 mat;
    sampler2D depth;
};
uniform SpotLight spot_lights[MAX_LIGHT];
// camera
uniform vec3 cam_pos;
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

float DirLightShadow(vec4 proj_pos, int i) {
    vec3 pos = proj_pos.xyz;
    pos = pos * 0.5f + 0.5f;
    float d_o = texture(dir_lights[i].depth, pos.xy).r;
    float d_r = pos.z;
    float bias = max(0.1 * (1.0 - dot(vs_out.normal, -dir_lights[i].dir)), 0.005);
    float res = (d_o + bias > d_r || d_r > 1.0) ? 1.0 : 0.0;
    return res;
}
float PointLightShadow(vec3 dir, int i) {
    float far = 100;
    float d_o = texture(point_lights[i].depth, dir).r * far;
    float d_r = length(point_lights[i].pos - vs_out.pos);
    float bias = max(0.1 * (1.0 - dot(vs_out.normal, -dir)), 0.005);
    float res = (d_o + bias > d_r || d_r > far) ? 1.0 : 0.0;
    return res;
}
float SpotLightShadow(vec4 proj_pos, int i) {
    vec3 pos = proj_pos.xyz / proj_pos.w;
    pos = pos * 0.5f + 0.5f;
    float d_o = texture(spot_lights[i].depth, pos.xy).r;
    float d_r = pos.z;
    float bias = max(0.1 * (1.0 - dot(vs_out.normal, -spot_lights[i].dir)), 0.005);
    float res = (d_o + bias > d_r || d_r > 1.0) ? 1.0 : 0.0;
    return res;
}

void main() {
    // calc normal
    vec3 normal = vs_out.normal;
    if (normal_b != 0) {
        vec3 normal_t = texture(normal_i, vs_out.texcoord).rgb;
        normal_t = normal_t * 2.0f - 1.0f;
        normal = normalize(normal_t);
        mat3 TBN = mat3(vs_out.tangent, vs_out.bitangent, vs_out.normal);
        normal = normalize(TBN * normal);
    }

    // get values according to XXX_b
    vec3 albedo_v = albedo_c;
    if (albedo_b != 0) albedo_v = texture(albedo_i, vs_out.texcoord).rgb;
    vec3 specular_v = specular_c;
    if (specular_b != 0) specular_v = texture(specular_i, vs_out.texcoord).rgb;

    // ambient
    vec3 ambient = ambient_c;
    if (ambient_b != 0) ambient = texture(ambient_i, vs_out.texcoord).rgb;

    // diffuse & specular
    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);
    vec3 view_dir = normalize(cam_pos - vs_out.pos);
    float exp = exponent_c.x * 32.0f;

    // dir lights
    for (int i = 0; i < n_dir_lights; i++) {
        // diffuse
        vec3 light_dir = -normalize(dir_lights[i].dir);
        float diff = max(dot(normal, light_dir), 0.0f);
        vec3 diffuse_p = diff * dir_lights[i].color;

        // specular
        vec3 half_way = normalize(view_dir + light_dir);
        float spec = pow(max(dot(normal, half_way), 0.0f), exp);
        vec3 specular_p = spec * specular_v * dir_lights[i].color;

        // shadow
        float shadow = 1.0f;
        if (dir_lights[i].shadow != 0) {
            shadow =
                DirLightShadow(dir_lights[i].mat * vec4(vs_out.pos, 1.0f), i);
        }

        // sum up
        diffuse += diffuse_p * shadow;
        specular += specular_p * shadow;
    }

    // point lights
    for (int i = 0; i < n_point_lights; i++) {
        // diffuse
        vec3 light_dir = normalize(point_lights[i].pos - vs_out.pos);
        float diff = max(dot(normal, light_dir), 0.0f);
        vec3 diffuse_p = diff * point_lights[i].color;

        // specular
        vec3 half_way = normalize(view_dir + light_dir);
        float spec = pow(max(dot(normal, half_way), 0.0f), exp);
        vec3 specular_p = spec * specular_v * point_lights[i].color;

        // attenuation
        float dist = length(light_dir);
        float atten = 1.0f / (point_lights[i].kq * dist * dist +
            point_lights[i].kl * dist + point_lights[i].kc);

        // shadow
        float shadow = 1.0f;
        if (point_lights[i].shadow != 0) {
            shadow = PointLightShadow(-light_dir, i);
        }

        atten *= shadow;
        diffuse += diffuse_p * atten;
        specular += specular_p * atten;
    }

    // spot lights
    for (int i = 0; i < n_spot_lights; i++) {
        vec3 light_dir = normalize(spot_lights[i].pos - vs_out.pos);
        float view = acos(dot(-light_dir, spot_lights[i].dir));
        float ratio;
        if (view > spot_lights[i].outer_cutoff) {
            ratio = 0.0f;
        } else if (view < spot_lights[i].cutoff) {
            ratio = 1.0f;
        } else {
            ratio = (spot_lights[i].outer_cutoff - view) /
                (spot_lights[i].outer_cutoff - spot_lights[i].cutoff);
        }

        // diffuse
        float diff = max(dot(normal, light_dir), 0.0f);
        vec3 diffuse_p = diff * spot_lights[i].color;

        // specular
        vec3 half_way = normalize(view_dir + light_dir);
        float spec = pow(max(dot(normal, half_way), 0.0f), exp);
        vec3 specular_p = spec * specular_v * spot_lights[i].color;

        // attenuation
        float dist = length(light_dir);
        float atten = 1.0f / (spot_lights[i].kq * dist * dist +
            spot_lights[i].kl * dist + spot_lights[i].kc);

        // shadow
        float shadow = 1.0f;
        if (spot_lights[i].shadow != 0) {
            shadow =
                SpotLightShadow(spot_lights[i].mat * vec4(vs_out.pos, 1.0f), i);
        }

        atten *= ratio * shadow;
        diffuse += diffuse_p * atten;
        specular += specular_p * atten;
    }

    vec3 result = (ambient + diffuse + specular) * albedo_v;
    result /= (result + 1.0f);
    result = pow(result, vec3(1.0f / 2.2f));
    frag_color = vec4(result, 1.0f);
    // frag_color = vec4(tmp, 1.0f);
}