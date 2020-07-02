#version 330 core

in vec2 texcoord;

out vec4 frag_color;

// G buffers
uniform sampler2D g_pos;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;
uniform sampler2D g_ambient;
uniform sampler2D g_spec;
// SSAO
uniform int enableSSAO;
uniform sampler2D SSAO;

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

float DirLightShadow(vec4 proj_pos, vec3 normal, int i) {
    vec3 pos = proj_pos.xyz;
    pos = pos * 0.5f + 0.5f;
    float d_o = texture(dir_lights[i].depth, pos.xy).r;
    float d_r = pos.z;
    float bias = max(0.1 * (1.0 - dot(normal, -dir_lights[i].dir)), 0.005);
    float res = (d_o + bias > d_r || d_r > 1.0) ? 1.0 : 0.0;
    return res;
}
float PointLightShadow(vec3 dir, vec3 pos, vec3 normal, int i) {
    float far = 100;
    float d_o = texture(point_lights[i].depth, dir).r * far;
    float d_r = length(point_lights[i].pos - pos);
    float bias = max(0.1 * (1.0 - dot(normal, -dir)), 0.005);
    float res = (d_o + bias > d_r || d_r > far) ? 1.0 : 0.0;
    return res;
}
float SpotLightShadow(vec4 proj_pos, vec3 normal, int i) {
    vec3 pos = proj_pos.xyz / proj_pos.w;
    pos = pos * 0.5f + 0.5f;
    float d_o = texture(spot_lights[i].depth, pos.xy).r;
    float d_r = pos.z;
    float bias = max(0.1 * (1.0 - dot(normal, -spot_lights[i].dir)), 0.005);
    float res = (d_o + bias > d_r || d_r > 1.0) ? 1.0 : 0.0;
    return res;
}

void main() {
    // get values from G buffers
    vec3 pos = texture(g_pos, texcoord).rgb;
    vec3 normal = texture(g_normal, texcoord).rgb;
    vec3 albedo_v = texture(g_albedo, texcoord).rgb;
    vec3 ambient_v = texture(g_ambient, texcoord).rgb;
    vec4 spec_exp = texture(g_spec, texcoord);
    vec3 specular_v = spec_exp.rgb;
    float exp = spec_exp.a * 32.0f;
    float occlusion = 1.0f;
    if (enableSSAO != 0) {
        occlusion = texture(SSAO, texcoord).r;
    }

    // discard frag that with nothing on it
    if (length(normal) < 0.8) {
        discard;
    }

    // ambient
    vec3 ambient = ambient_v * occlusion;

    // diffuse & specular
    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);
    vec3 view_dir = normalize(cam_pos - pos);

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
            shadow = DirLightShadow(dir_lights[i].mat * vec4(pos, 1.0f),
                normal, i);
        }

        // sum up
        diffuse += diffuse_p * shadow;
        specular += specular_p * shadow;
    }

    // point lights
    for (int i = 0; i < n_point_lights; i++) {
        // diffuse
        vec3 light_dir = normalize(point_lights[i].pos - pos);
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
            shadow = PointLightShadow(-light_dir, pos, normal, i);
        }

        atten *= shadow;
        diffuse += diffuse_p * atten;
        specular += specular_p * atten;
    }

    // spot lights
    for (int i = 0; i < n_spot_lights; i++) {
        vec3 light_dir = normalize(spot_lights[i].pos - pos);
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
            shadow = SpotLightShadow(spot_lights[i].mat * vec4(pos, 1.0f),
                normal, i);
        }

        atten *= shadow;
        diffuse += diffuse_p * atten * ratio;
        specular += specular_p * atten * ratio;
    }

    vec3 result = (ambient + diffuse + specular) * albedo_v;
    result /= (result + 1.0f);
    result = pow(result, vec3(1.0f / 2.2f));
    frag_color = vec4(result, 1.0f);
}