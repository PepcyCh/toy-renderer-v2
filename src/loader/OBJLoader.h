#pragma once

#include <map>

#include "Scene.h"

namespace pepcy::renderer {

class OBJLoader {
  public:
    Scene ReadFile(const std::string &filename);

  private:
    void Clear();
    void ReadMTLFile(const std::string &filename);
    void Triangulate(const std::vector<unsigned int> &vec);
    std::vector<gm::Vector3> CalcTangents();

    void AddMeshToScene();
    void AddMaterialToMap(const std::string &name, bool has_name);

    static constexpr int BUF_LEN = 4096;
    char buf[BUF_LEN];

    std::string directory;

    std::vector<gm::Vector3> v, v_buf;
    std::vector<gm::Vector3> vn, vn_buf;
    std::vector<gm::Vector2> vt, vt_buf;
    std::vector<unsigned int> f, f_buf;
    std::map<std::string, Material> materials;
    std::map<std::tuple<int, int, int>, unsigned int> id_map;

    Scene scene_buf;
    Material material_buf;
};

}