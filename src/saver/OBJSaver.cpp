#include "OBJSaver.h"

#include <fstream>

static std::string FaceStr(unsigned int i) {
    std::string num_str = std::to_string(i);
    return num_str + "/" + num_str + "/" + num_str;
}

namespace pepcy::renderer {

void OBJSaver::SaveScene(const std::string &directory,
        const std::string &filename, const Scene &scene) {
    this->directory = directory;
    int pos = filename.rfind('.');
    if (pos == -1 || filename.substr(pos, filename.size()) != ".obj") {
        name_base = filename;
    } else {
        name_base = filename.substr(0, pos);
    }

    SaveMTL(scene);

    std::ofstream fout(directory + name_base + ".obj");
    fout << "mtllib " << name_base << ".mtl" << std::endl;

    int n_mesh = 0;
    unsigned int offset = 1;
    for (const auto &mesh : scene.GetMeshes()) {
        ++n_mesh;
        fout << "o mesh" << n_mesh << std::endl;

        auto trans = mesh->GetModel();
        int N = mesh->GetVertexCount();
        const float *p_pos = mesh->GetPositions();
        for (int i = 0; i < N; i++) {
            float x = *p_pos++;
            float y = *p_pos++;
            float z = *p_pos++;
            gm::Vector3 pos(x, y, z);
            pos = trans.TransformPoint(pos);
            fout << "v " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
        }
        const float *p_norm = mesh->GetNormals();
        for (int i = 0; i < N; i++) {
            float x = *p_norm++;
            float y = *p_norm++;
            float z = *p_norm++;
            gm::Vector3 norm(x, y, z);
            norm = trans.TransformNormal(norm);
            fout << "vn " << norm[0] << " " << norm[1] << " " << norm[2] << std::endl;
        }
        const float *p_texcoord = mesh->GetTexcoords();
        for (int i = 0; i < N; i++) {
            float u = *p_texcoord++;
            float v = *p_texcoord++;
            fout << "vt " << u << " " << v << std::endl;
        }

        fout << "usemtl mesh" << n_mesh << "\ns 1" << std::endl;

        int M = mesh->GetIndexCount();
        const unsigned int *p_ind = mesh->GetIndices();
        for (int i = 0; i < M / 3; i++) {
            unsigned int a = offset + *p_ind++;
            unsigned int b = offset + *p_ind++;
            unsigned int c = offset + *p_ind++;
            fout << "f " << FaceStr(a) << " " << FaceStr(b) << " " <<
                FaceStr(c) << std::endl;
        }

        offset += N;
    }

    fout.close();
}

void OBJSaver::SaveMTL(const Scene &scene) {
    std::ofstream fout(directory + name_base + ".mtl");

    static const std::unordered_map<std::string, std::string> tex_name = {
        {"ambient", "Ka"}, {"albedo", "Kd"}, {"specular", "Ks"},
        {"normal", "bump"}, {"roughness", "Pr"}, {"metallic", "Pm"}
    };
    int n_mesh = 0;
    for (const auto &mesh : scene.GetMeshes()) {
        ++n_mesh;
        fout << "newmtl mesh" << n_mesh << std::endl;
        for (const auto &[name, tex] : mesh->GetMaterial().GetTextures()) {
            if (name == "exponent") {
                fout << "Ns " << tex.GetColor().r * 32 << std::endl;
                continue;
            } else if (tex_name.count(name) == 0) {
                continue;
            }
            auto out_name = tex_name.at(name);
            if (tex.IsColor()) {
                auto col = tex.GetColor();
                fout << out_name << " " << col.r << " " << col.g << " " <<
                    col.b << std::endl;
            } else {
                auto path = tex.GetPath();
                path = path.substr(directory.size(), path.size());
                fout << "map_" << out_name << " " << path << std::endl;
            }
        }

        fout << "d 1.0\nillum 2" << std::endl;
    }

    fout.close();
}

}