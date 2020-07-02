#version 330 core

in vec3 direction;

out vec4 fragColor;

uniform samplerCube cubemap;

const float PI = 3.141592653589793238463;

void main() {
    vec3 normal = normalize(direction);
    vec3 irradiance = vec3(0.0);

    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, normal);
    up = cross(normal, right);

    float delta = 0.05;
    int nSample = 0;
    for (float phi = 0.0; phi < 2.0 * PI; phi += delta) {
        for (float theta = 0.0; theta < 0.5 * PI; theta += delta) {
            vec3 sample_t = vec3(sin(theta) * cos(phi),
                                      sin(theta) * sin(phi),
                                      cos(theta));
            vec3 sampleVec = sample_t.x * right +
                             sample_t.y * up +
                             sample_t.z * normal;

            irradiance += texture(cubemap, sampleVec).rgb *
                    cos(theta) * sin(theta);
            ++nSample;
        }
    }
    irradiance = PI * irradiance / float(nSample);

    fragColor = vec4(irradiance, 1.0f);
}