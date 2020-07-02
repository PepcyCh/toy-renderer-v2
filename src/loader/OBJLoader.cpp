#include "OBJLoader.h"

#include <cstring>
#include <fstream>

namespace pepcy::renderer {

Scene OBJLoader::ReadFile(const std::string &filename) {
    Clear();

    std::ifstream obj(filename);
    if (!obj) {
        return scene_buf;
    }

    directory = filename.substr(0, filename.find_last_of('/'));
    while (!obj.eof()) {
        obj.getline(buf, BUF_LEN);
        char *p = buf;
        while (*p == ' ') ++p;

        if (*p == '#') { // comments
            continue;
        } else if (*p == 'v') {
            if (*(p + 1) == ' ') { // vertex
                double x = 0, y = 0, z = 0;
                sscanf(p + 2, "%lf %lf %lf", &x, &y, &z);
                v_buf.emplace_back(x, y, z);
            } else if (*(p + 1) == 'n' && *(p + 2) == ' ') { // normal
                double x = 0, y = 0, z = 0;
                sscanf(p + 3, "%lf %lf %lf", &x, &y, &z);
                vn_buf.emplace_back(x, y, z);
            } else if (*(p + 1) == 't' && *(p + 2) == ' ') { // texture uv
                double x = 0, y = 0;
                sscanf(p + 3, "%lf %lf", &x, &y);
                vt_buf.emplace_back(x, y);
            }
        } else if (*p == 'f' && *(p + 1) == ' ') { // face
            f_buf.clear();
            p += 2;
            while (*p == ' ') ++p;

            while (*p) {
                int a = 0, b = 0, c = 0;
                int ret = sscanf(p, "%d/%d/%d", &a, &b, &c);
                if (ret == 0)
                    break;

                if (ret == 1)
                    sscanf(p, "%d//%d", &a, &c);

                while (*p && !isspace(*p)) ++p;
                while (*p == ' ') ++p;

                if (a < 0) a += v_buf.size() + 1;
                if (b < 0) b += vt_buf.size() + 1;
                if (b == 0) b = -1;
                if (c < 0) c += vn_buf.size() + 1;
                if (c == 0) c = -1;

                auto tuple = std::make_tuple(a, b, c);
                unsigned int id;
                if (id_map.find(tuple) == id_map.end()) {
                    id = id_map.size();
                    id_map[tuple] = id;
                    v.push_back(v_buf[a - 1]);
                    if (b != -1) vt.push_back(vt_buf[b - 1]);
                    if (c != -1) vn.push_back(vn_buf[c - 1]);
                } else {
                    id = id_map[tuple];
                }

                f_buf.push_back(id);
            }
            Triangulate(f_buf);
        } else if (*p == 'o' && *(p + 1) == ' ') { // object
            // add mesh to scene
            AddMeshToScene();
        } else if (*p == 'g' && *(p + 1) == ' ') { // group
            // add last mesh to scene
            AddMeshToScene();
        } else if (strncmp(p, "mtllib ", 7) == 0) { // external .mtl file
            sscanf(p + 7, "%s", buf);
            ReadMTLFile(directory + '/' + buf);
        } else if (strncmp(p, "usemtl ", 7) == 0) { // specify material
            sscanf(p + 7, "%s", buf);
            material_buf = materials[buf];
        }
    }

    // deal with the last mesh
    AddMeshToScene();

    return scene_buf;
}

void OBJLoader::AddMeshToScene() {
    if (!v.empty()) {
        std::vector<gm::Vector3> tan = CalcTangents();
        auto sh = new Shape(f, v, vn, vt, tan);
        material_buf.CheckPhong();
        sh->SetMaterial(material_buf);
        scene_buf.AddMesh(sh);
        if (material_buf.HasTexture("emissive")) {
            auto col = material_buf.GetTexture("emissive").GetColor();
            if (col.Luminance() > 0.001f) {
                gm::Vector3 center(0, 0, 0);
                for (const auto &p : v) {
                    center += p;
                }
                center /= v.size();
                scene_buf.AddLight(PointLight(col, center, 0.0f, 0.0f, 1.0f));
            }
        }
    }
    v.clear();
    vn.clear();
    vt.clear();
    f.clear();
    id_map.clear();
    material_buf = Material();
}

void OBJLoader::Clear() {
    v.clear();
    v_buf.clear();
    vn.clear();
    vn_buf.clear();
    vt.clear();
    vt_buf.clear();
    f.clear();
    materials.clear();
    id_map.clear();

    scene_buf = Scene();
    material_buf = Material();
}

void OBJLoader::ReadMTLFile(const std::string &filename) {
    std::ifstream mtl(filename);
    if (!mtl)
        return;

    std::string mtl_name;
    bool has_name = false;
    while (!mtl.eof()) {
        mtl.getline(buf, BUF_LEN);
        char *p = buf;
        while (*p == ' ') ++p;

        if (*p == '#') { // comments
            continue;
        } else if (strncmp(p, "newmtl ", 7) == 0) { // new material
            // add last material
            AddMaterialToMap(mtl_name, has_name);

            // get name if next material
            int ret = sscanf(p + 7, "%s", buf);
            if (ret == 1) {
                mtl_name = buf;
                has_name = true;
            }
        // basic material
        } else if (strncmp(p, "Ka ", 3) == 0) { // ambient - color
            double r = 0, g = 0, b = 0;
            sscanf(p + 3, "%lf %lf %lf", &r, &g, &b);
            material_buf.AddTexture("ambient", gm::Color(r, g, b).Clamp());
        } else if (strncmp(p, "Kd ", 3) == 0) { // diffuse - color
            double r = 0, g = 0, b = 0;
            sscanf(p + 3, "%lf %lf %lf", &r, &g, &b);
            material_buf.AddTexture("albedo", gm::Color(r, g, b).Clamp());
        } else if (strncmp(p, "Ks ", 3) == 0) { // specular - color
            double r = 0, g = 0, b = 0;
            sscanf(p + 3, "%lf %lf %lf", &r, &g, &b);
            material_buf.AddTexture("specular", gm::Color(r, g, b).Clamp());
        } else if (strncmp(p, "Ns ", 3) == 0) { // exponent
            double ns;
            sscanf(p + 3, "%lf", &ns);
            ns /= 32;
            material_buf.AddTexture("exponent", gm::Color(ns, ns, ns).Clamp());
        } else if (strncmp(p, "Ke ", 3) == 0) { // emissive - color
            double r = 0, g = 0, b = 0;
            sscanf(p + 3, "%lf %lf %lf", &r, &g, &b);
            material_buf.AddTexture("emissive", gm::Color(r, g, b).Clamp());
        } else if (strncmp(p, "map_Ka ", 7) == 0) { // ambient - texture map
            sscanf(p + 7, "%s", buf);
            material_buf.AddTexture("ambient", directory + '/' + buf);
        } else if (strncmp(p, "map_Kd ", 7) == 0) { // diffuse - texture map
            sscanf(p + 7, "%s", buf);
            material_buf.AddTexture("albedo", Texture(directory + '/' + buf, true));
        } else if (strncmp(p, "map_Ks ", 7) == 0) { // specular - texture map
            sscanf(p + 7, "%s", buf);
            material_buf.AddTexture("specular", directory + '/' + buf);
        } else if (strncmp(p, "map_bump ", 9) == 0) { // normal - texture map
            sscanf(p + 9, "%s", buf);
            material_buf.AddTexture("normal", directory + '/' + buf);

        // PRB extension
        } else if (strncmp(p, "Pr ", 3) == 0) { // roughness - color
            double r = 0;
            sscanf(p + 3, "%lf", &r);
            material_buf.AddTexture("roughness", Texture(gm::Color(r, r, r)));
        } else if (strncmp(p, "map_Pr ", 7) == 0) { // roughness - texture map
            sscanf(p + 7, "%s", buf);
            material_buf.AddTexture("roughness", directory + '/' + buf);
        } else if (strncmp(p, "Pm ", 3) == 0) { // metallic - color
            double m = 0;
            sscanf(p + 3, "%lf", &m);
            material_buf.AddTexture("metallic", gm::Color(m, m, m).Clamp());
        } else if (strncmp(p, "map_Pm ", 7) == 0) { // metallic - texture map
            sscanf(p + 7, "%s", buf);
            material_buf.AddTexture("metallic", directory + '/' + buf);
        } else if (strncmp(p, "norm ", 5) == 0) { // normal - texture map
            sscanf(p + 5, "%s", buf);
            material_buf.AddTexture("normal", directory + '/' + buf);
        }
    }

    AddMaterialToMap(mtl_name, has_name);
}

void OBJLoader::AddMaterialToMap(const std::string &name, bool has_name) {
    if (has_name) {
        materials[name] = material_buf;
    }
    material_buf = Material();
}

void OBJLoader::Triangulate(const std::vector<unsigned int> &vec) {
    // triangulate and push to vector f
    gm::Vector3 norm0 = vn[vec[0]];
    for (int i = 1; i + 1 < vec.size(); i++) {
        gm::Vector3 v01 = v[vec[i]] - v[vec[0]];
        gm::Vector3 v12 = v[vec[i + 1]] - v[vec[i]];

        f.push_back(vec[0]);
        if (Dot(norm0, Cross(v01, v12)) > 0) {
            f.push_back(vec[i]);
            f.push_back(vec[i + 1]);
        } else {
            f.push_back(vec[i + 1]);
            f.push_back(vec[i]);
        }
    }
}

std::vector<gm::Vector3> OBJLoader::CalcTangents() {
    if (vt.empty()) {
        return {};
    }

    int N = v.size();
    std::vector<gm::Vector3> tan(N);
    std::vector<int> deg(N, 0);

    for (int i = 0; i < f.size(); i += 3) {
        gm::Vector3 e1 = v[f[i + 1]] - v[f[i]];
        gm::Vector3 e2 = v[f[i + 2]] - v[f[i]];
        gm::Vector2 u1 = vt[f[i + 1]] - vt[f[i]];
        gm::Vector2 u2 = vt[f[i + 2]] - vt[f[i]];
        float f = 1.0 / gm::Cross(u1, u2);

        gm::Vector3 t = gm::Vector3(f * (u2[1] * e1[0] - u1[1] * e2[0]),
                                    f * (u2[1] * e1[1] - u1[1] * e2[1]),
                                    f * (u2[1] * e1[2] - u1[1] * e2[2]));
        t = gm::Normalize(t);

        tan[this->f[i]] += t;
        ++deg[this->f[i]];
        tan[this->f[i + 1]] += t;
        ++deg[this->f[i + 1]];
        tan[this->f[i + 2]] += t;
        ++deg[this->f[i + 2]];
    }

    for (int i = 0; i < N; i++) {
        float inv = 1.0 / deg[i];
        tan[i] = gm::Normalize(tan[i] * inv);
    }

    return tan;
}

}