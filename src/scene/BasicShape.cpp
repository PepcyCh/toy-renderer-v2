#include "BasicShape.h"

namespace pepcy::renderer {

Cube::Cube() {
    static bool init_id = true;
    if (init_id) {
        g_gid = id = gid::NewGID();
        init_id = false;
    } else {
        id = g_gid;
    }
    float hs = 0.5f;
    bbox = gm::BBox(gm::Vector3(-hs, -hs, -hs), gm::Vector3(hs, hs, hs));
    bbox_valid = true;

    positions = {
        gm::Vector3(-hs, -hs, -hs), gm::Vector3(-hs,  hs, -hs),
        gm::Vector3( hs,  hs, -hs), gm::Vector3( hs, -hs, -hs),
        gm::Vector3(-hs, -hs,  hs), gm::Vector3(-hs,  hs,  hs),
        gm::Vector3( hs,  hs,  hs), gm::Vector3( hs, -hs,  hs),
        gm::Vector3(-hs, -hs, -hs), gm::Vector3( hs, -hs, -hs),
        gm::Vector3( hs, -hs,  hs), gm::Vector3(-hs, -hs,  hs),
        gm::Vector3(-hs,  hs, -hs), gm::Vector3( hs,  hs, -hs),
        gm::Vector3( hs,  hs,  hs), gm::Vector3(-hs,  hs,  hs),
        gm::Vector3(-hs, -hs, -hs), gm::Vector3(-hs, -hs,  hs),
        gm::Vector3(-hs,  hs,  hs), gm::Vector3(-hs,  hs, -hs),
        gm::Vector3( hs, -hs, -hs), gm::Vector3( hs, -hs,  hs),
        gm::Vector3( hs,  hs,  hs), gm::Vector3( hs,  hs, -hs)
    };
    normals = {
        gm::Vector3( 0,  0, -1), gm::Vector3( 0,  0, -1),
        gm::Vector3( 0,  0, -1), gm::Vector3( 0,  0, -1),
        gm::Vector3( 0,  0,  1), gm::Vector3( 0,  0,  1),
        gm::Vector3( 0,  0,  1), gm::Vector3( 0,  0,  1),
        gm::Vector3( 0, -1,  0), gm::Vector3( 0, -1,  0),
        gm::Vector3( 0, -1,  0), gm::Vector3( 0, -1,  0),
        gm::Vector3( 0,  1,  0), gm::Vector3( 0,  1,  0),
        gm::Vector3( 0,  1,  0), gm::Vector3( 0,  1,  0),
        gm::Vector3(-1,  0,  0), gm::Vector3(-1,  0,  0),
        gm::Vector3(-1,  0,  0), gm::Vector3(-1,  0,  0),
        gm::Vector3( 1,  0,  0), gm::Vector3( 1,  0,  0),
        gm::Vector3( 1,  0,  0), gm::Vector3( 1,  0,  0)
    };
    texcoords = {
        gm::Vector2(0, 0), gm::Vector2(1, 0),
        gm::Vector2(1, 1), gm::Vector2(0, 1),
        gm::Vector2(0, 0), gm::Vector2(1, 0),
        gm::Vector2(1, 1), gm::Vector2(0, 1),
        gm::Vector2(0, 0), gm::Vector2(1, 0),
        gm::Vector2(1, 1), gm::Vector2(0, 1),
        gm::Vector2(0, 0), gm::Vector2(1, 0),
        gm::Vector2(1, 1), gm::Vector2(0, 1),
        gm::Vector2(0, 0), gm::Vector2(1, 0),
        gm::Vector2(1, 1), gm::Vector2(0, 1),
        gm::Vector2(0, 0), gm::Vector2(1, 0),
        gm::Vector2(1, 1), gm::Vector2(0, 1)
    };
    tangents = {
        gm::Vector3(0, 1, 0), gm::Vector3(0, 1, 0),
        gm::Vector3(0, 1, 0), gm::Vector3(0, 1, 0),
        gm::Vector3(1, 0, 0), gm::Vector3(1, 0, 0),
        gm::Vector3(1, 0, 0), gm::Vector3(1, 0, 0),
        gm::Vector3(1, 0, 0), gm::Vector3(1, 0, 0),
        gm::Vector3(1, 0, 0), gm::Vector3(1, 0, 0),
        gm::Vector3(0, 0, 1), gm::Vector3(0, 0, 1),
        gm::Vector3(0, 0, 1), gm::Vector3(0, 0, 1),
        gm::Vector3(0, 0, 1), gm::Vector3(0, 0, 1),
        gm::Vector3(0, 0, 1), gm::Vector3(0, 0, 1),
        gm::Vector3(0, 1, 0), gm::Vector3(0, 1, 0),
        gm::Vector3(0, 1, 0), gm::Vector3(0, 1, 0)
    };
    indices = {
         0,  1,  2,  0,  2,  3,
         4,  6,  5,  4,  7,  6,
         8,  9, 10,  8, 10, 11,
        12, 14, 13, 12, 15, 14,
        16, 17, 18, 16, 18, 19,
        20, 22, 21, 20, 23, 22
    };
    int N = positions.size();
    bitangents.resize(N);
    for (int i = 0; i < N; i++) {
        bitangents[i] = gm::Cross(normals[i], tangents[i]);
    }
}

Sphere::Sphere() {
    static bool init_id = true;
    if (init_id) {
        g_gid = id = gid::NewGID();
        init_id = false;
    } else {
        id = g_gid;
    }
    bbox = gm::BBox(gm::Vector3(-0.5f, -0.5f, -0.5f), gm::Vector3(0.5f, 0.5f, 0.5f));
    bbox_valid = true;

    const int Y_COUNT = 24;
    const int X_COUNT = 2 * Y_COUNT;
    for (int j = 0; j <= Y_COUNT; j++) {
        float y = float(j) / Y_COUNT;
        for (int i = 0; i < X_COUNT; i++) {
            float x = float(i) / X_COUNT;

            float rad_x = x * 2.0f * gm::PI;
            float rad_y = y * gm::PI;

            float x_pos = std::cos(rad_x) * std::sin(rad_y);
            float y_pos = std::cos(rad_y);
            float z_pos = std::sin(rad_x) * std::sin(rad_y);

            positions.emplace_back(x_pos, y_pos, z_pos);
            texcoords.emplace_back(x, y);
            normals.emplace_back(x_pos, y_pos, z_pos);
            tangents.emplace_back(-std::sin(rad_x), 0.0f, std::cos(rad_x));
        }
        int N = positions.size() - X_COUNT;
        positions.emplace_back(positions[N]);
        texcoords.emplace_back(1, y);
        normals.emplace_back(normals[N]);
        tangents.emplace_back(tangents[N]);
    }
    for (int j = 0; j < Y_COUNT; j++) {
        for (int i = 0; i < X_COUNT; i++) {
            indices.push_back( j      * (X_COUNT + 1) + i);
            indices.push_back((j + 1) * (X_COUNT + 1) + i + 1);
            indices.push_back((j + 1) * (X_COUNT + 1) + i);
            indices.push_back( j      * (X_COUNT + 1) + i);
            indices.push_back( j      * (X_COUNT + 1) + i + 1);
            indices.push_back((j + 1) * (X_COUNT + 1) + i + 1);
        }
    }
    int N = positions.size();
    bitangents.resize(N);
    for (int i = 0; i < N; i++) {
        bitangents[i] = gm::Cross(normals[i], tangents[i]);
    }
}

/*
static gm::Vector3 GetSphereTangent(gm::Vector3 &norm) {
    float xt = norm[2] * norm[2] / (norm[0] * norm[0] + norm[2] * norm[2]);
    float zt = std::sqrt(1 - xt);
    xt = std::sqrt(xt);
    gm::Vector3 tan(xt, 0.0f, zt);
    if (norm[0] >= 0 && norm[2] > 0) {
        tan[0] = -xt;
        tan[2] = zt;
    } else if (norm[0] < 0 && norm[2] >= 0) {
        tan[0] = -xt;
        tan[2] = -zt;
    } else if (norm[0] <= 0 && norm[2] < 0) {
        tan[0] = xt;
        tan[2] = -zt;
    }
    return tan;
}
static bool SphereTest(const Sphere &s, const gm::Ray &r, float &t0, float &t1) {
    float a = r.dir.Norm2();
    float b = gm::Dot(r.orig, r.dir);
    float c = r.orig.Norm2() - 1.0f;
    float disc = b * b - a * c;
    if (disc > 0) {
        float sqrtd = sqrt(disc);
        t0 = (-b - sqrtd) / a;
        t1 = (-b + sqrtd) / a;
        return true;
    }
    return false;
}
bool Sphere::Intersect(const gm::Ray &r_) const {
    auto trans = model;
    gm::Ray r = trans.InvTransformRay(r_);
    float t0, t1;
    if (SphereTest(*this, r, t0, t1)) {
        return !(t0 > r.t_max || t1 < r.t_min);
    }
    return false;
}
bool Sphere::Intersect(const gm::Ray &r_, Intersection &inter) const {
    auto trans = model;
    gm::Ray r = trans.InvTransformRay(r_);
    float t0, t1;
    if (SphereTest(*this, r, t0, t1)) {
        if (t0 > r.t_max || t1 < r.t_min) {
            return false;
        }
        inter.prim = this;
        if (t0 > r.t_min) {
            r.t_max = t0;
            inter.norm = gm::Normalize(r.orig + r.dir * t0);
        } else {
            r.t_max = t1;
            inter.norm = gm::Normalize(r.orig + r.dir * t1);
        }
        inter.tan = GetSphereTangent(inter.norm);
        r = trans.TransformRay(r);
        inter.TransformedBy(trans);
        inter.t = r_.t_max = r.t_max;
        return true;
    }
    return false;
}
*/

Plane::Plane() {
    static bool init_id = true;
    if (init_id) {
        g_gid = id = gid::NewGID();
        init_id = false;
    } else {
        id = g_gid;
    }
    bbox = gm::BBox(gm::Vector3(-0.5f, -0.001f, -0.5f),
        gm::Vector3(0.5f, 0.001f, 0.5f));
    bbox_valid = true;

    positions = {
        gm::Vector3(-0.5f, 0.0f, -0.5f), gm::Vector3(-0.5f, 0.0f,  0.5f),
        gm::Vector3( 0.5f, 0.0f,  0.5f), gm::Vector3( 0.5f, 0.0f, -0.5f),
        gm::Vector3(-0.5f, 0.0f, -0.5f), gm::Vector3( 0.5f, 0.0f, -0.5f),
        gm::Vector3( 0.5f, 0.0f,  0.5f), gm::Vector3(-0.5f, 0.0f,  0.5f)
    };
    normals = {
        gm::Vector3(0,  1, 0), gm::Vector3(0,  1, 0),
        gm::Vector3(0,  1, 0), gm::Vector3(0,  1, 0),
        gm::Vector3(0, -1, 0), gm::Vector3(0, -1, 0),
        gm::Vector3(0, -1, 0), gm::Vector3(0, -1, 0)
    };
    texcoords = {
        gm::Vector2(0, 0), gm::Vector2(1, 0),
        gm::Vector2(1, 1), gm::Vector2(0, 1),
        gm::Vector2(0, 0), gm::Vector2(1, 0),
        gm::Vector2(1, 1), gm::Vector2(0, 1)
    };
    tangents = {
        gm::Vector3(0, 0, 1), gm::Vector3(0, 0, 1),
        gm::Vector3(0, 0, 1), gm::Vector3(0, 0, 1),
        gm::Vector3(1, 0, 0), gm::Vector3(1, 0, 0),
        gm::Vector3(1, 0, 0), gm::Vector3(1, 0, 0)
    };
    bitangents = {
        gm::Vector3(1, 0, 0), gm::Vector3(1, 0, 0),
        gm::Vector3(1, 0, 0), gm::Vector3(1, 0, 0),
        gm::Vector3(0, 0, 1), gm::Vector3(0, 0, 1),
        gm::Vector3(0, 0, 1), gm::Vector3(0, 0, 1)
    };
    indices = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7
    };
}

ScreenQuad::ScreenQuad() {
    static bool init_id = true;
    if (init_id) {
        g_gid = id = gid::NewGID();
        init_id = false;
    } else {
        id = g_gid;
    }
    model = gm::Transform();
    bbox = gm::BBox(gm::Vector3(-0.5f, -0.5f, -0.001f),
                    gm::Vector3(0.5f, 0.5f, 0.001f));
    bbox_valid = true;

    positions = {
        gm::Vector3(-1, -1, 0), gm::Vector3( 1, -1, 0),
        gm::Vector3( 1,  1, 0), gm::Vector3(-1,  1, 0)
    };
    normals = {
        gm::Vector3(0, 0, 1), gm::Vector3(0, 0, 1),
        gm::Vector3(0, 0, 1), gm::Vector3(0, 0, 1)
    };
    texcoords = {
        gm::Vector2(0, 0), gm::Vector2(1, 0),
        gm::Vector2(1, 1), gm::Vector2(0, 1)
    };
    tangents = {
        gm::Vector3(1, 0, 0), gm::Vector3(1, 0, 0),
        gm::Vector3(1, 0, 0), gm::Vector3(1, 0, 0)
    };
    bitangents = {
        gm::Vector3(0, 1, 0), gm::Vector3(0, 1, 0),
        gm::Vector3(0, 1, 0), gm::Vector3(0, 1, 0)
    };
    indices = {
        0, 1, 2, 0, 2, 3
    };
}

Cylinder::Cylinder() {
    static bool init_id = true;
    if (init_id) {
        g_gid = id = gid::NewGID();
        init_id = false;
    } else {
        id = g_gid;
    }
    bbox = gm::BBox(gm::Vector3(-1.0f, -0.5f, -1.0f),
                    gm::Vector3( 1.0f,  0.5f,  1.0f));
    bbox_valid = true;
    
    const int COUNT = 24;
    for (int i = 0; i < COUNT; i++) {
        float ratio = float(i) / COUNT;
        float rad = ratio * 2.0f * gm::PI;
        float cosr = std::cos(rad);
        float sinr = std::sin(rad);

        positions.emplace_back(0.0f, 0.5f, 0.0f);
        positions.emplace_back(cosr, 0.5f, sinr);
        positions.emplace_back(cosr, 0.5f, sinr);
        positions.emplace_back(cosr, -0.5f, sinr);
        positions.emplace_back(cosr, -0.5f, sinr);
        positions.emplace_back(0.0f, -0.5f, 0.0f);

        normals.emplace_back(0, 1, 0);
        normals.emplace_back(0, 1, 0);
        normals.emplace_back(cosr, 0, sinr);
        normals.emplace_back(cosr, 0, sinr);
        normals.emplace_back(0, -1, 0);
        normals.emplace_back(0, -1, 0);

        texcoords.emplace_back(ratio, 0);
        texcoords.emplace_back(ratio, 0.25f);
        texcoords.emplace_back(ratio, 0.25f);
        texcoords.emplace_back(ratio, 0.75f);
        texcoords.emplace_back(ratio, 0.75f);
        texcoords.emplace_back(ratio, 1);

        tangents.emplace_back(-sinr, 0, cosr);
        tangents.emplace_back(-sinr, 0, cosr);
        tangents.emplace_back(-sinr, 0, cosr);
        tangents.emplace_back(-sinr, 0, cosr);
        tangents.emplace_back(-sinr, 0, cosr);
        tangents.emplace_back(-sinr, 0, cosr);

        indices.emplace_back(6 * i);
        indices.emplace_back(6 * i + 7);
        indices.emplace_back(6 * i + 1);

        indices.emplace_back(6 * i + 2);
        indices.emplace_back(6 * i + 8);
        indices.emplace_back(6 * i + 9);

        indices.emplace_back(6 * i + 2);
        indices.emplace_back(6 * i + 9);
        indices.emplace_back(6 * i + 3);

        indices.emplace_back(6 * i + 5);
        indices.emplace_back(6 * i + 4);
        indices.emplace_back(6 * i + 10);
    }
    for (int i = 0; i < 6; i++) {
        positions.emplace_back(positions[i]);
        normals.emplace_back(texcoords[i]);
        tangents.emplace_back(tangents[i]);
    }
    texcoords.emplace_back(1, 0);
    texcoords.emplace_back(1, 0.25f);
    texcoords.emplace_back(1, 0.25f);
    texcoords.emplace_back(1, 0.75f);
    texcoords.emplace_back(1, 0.75f);
    texcoords.emplace_back(1, 1);

    int N = positions.size();
    bitangents.resize(N);
    for (int i = 0; i < N; i++) {
        bitangents[i] = gm::Cross(normals[i], tangents[i]);
    }
}

Capsule::Capsule(float radius, float height) : radius(radius), height(height) {
    id = gid::NewGID();
    float hh = height / 2.0f, r = radius;
    bbox = gm::BBox(gm::Vector3(-r, -hh - r, -r), gm::Vector3(r, hh + r, r));
    bbox_valid = true;

    const int Y_COUNT = 24;
    const int X_COUNT = 2 * Y_COUNT;
    for (int j = 0; j <= Y_COUNT / 2; j++) {
        float y = float(j) / Y_COUNT;
        for (int i = 0; i < X_COUNT; i++) {
            float x = float(i) / X_COUNT;

            float rad_x = x * 2.0f * gm::PI;
            float rad_y = y * gm::PI;

            float x_pos = std::cos(rad_x) * std::sin(rad_y);
            float y_pos = std::cos(rad_y);
            float z_pos = std::sin(rad_x) * std::sin(rad_y);

            positions.emplace_back(x_pos * r, y_pos * r + hh, z_pos * r);
            texcoords.emplace_back(x, y * 0.5f);
            normals.emplace_back(x_pos, y_pos, z_pos);
            tangents.emplace_back(-std::sin(rad_x), 0.0f, std::cos(rad_x));
        }
        int N = positions.size() - X_COUNT;
        positions.emplace_back(positions[N]);
        texcoords.emplace_back(1, y);
        normals.emplace_back(normals[N]);
        tangents.emplace_back(tangents[N]);
    }
    for (int j = Y_COUNT / 2; j <= Y_COUNT; j++) {
        float y = float(j) / Y_COUNT;
        for (int i = 0; i < X_COUNT; i++) {
            float x = float(i) / X_COUNT;

            float rad_x = x * 2.0f * gm::PI;
            float rad_y = y * gm::PI;

            float x_pos = std::cos(rad_x) * std::sin(rad_y);
            float y_pos = std::cos(rad_y);
            float z_pos = std::sin(rad_x) * std::sin(rad_y);

            positions.emplace_back(x_pos * r, y_pos * r - hh, z_pos * r);
            texcoords.emplace_back(x, 0.5f + y * 0.5f);
            normals.emplace_back(x_pos, y_pos, z_pos);
            tangents.emplace_back(-std::sin(rad_x), 0.0f, std::cos(rad_x));
        }
        int N = positions.size() - X_COUNT;
        positions.emplace_back(positions[N]);
        texcoords.emplace_back(1, y);
        normals.emplace_back(normals[N]);
        tangents.emplace_back(tangents[N]);
    }
    for (int j = 0; j <= Y_COUNT; j++) {
        for (int i = 0; i < X_COUNT; i++) {
            indices.push_back( j      * (X_COUNT + 1) + i);
            indices.push_back((j + 1) * (X_COUNT + 1) + i + 1);
            indices.push_back((j + 1) * (X_COUNT + 1) + i);
            indices.push_back( j      * (X_COUNT + 1) + i);
            indices.push_back( j      * (X_COUNT + 1) + i + 1);
            indices.push_back((j + 1) * (X_COUNT + 1) + i + 1);
        }
    }

    int N = positions.size();
    bitangents.resize(N);
    for (int i = 0; i < N; i++) {
        bitangents[i] = gm::Cross(normals[i], tangents[i]);
    }
}

}