#pragma once

#include "Light.h"
#include "Shape.h"

namespace pepcy::renderer {

class Scene {
  public:
    Scene() = default;
    Scene(const std::vector<DirectionalLight> &dir_lights,
          const std::vector<PointLight> &point_lights,
          const std::vector<SpotLight> &spot_lights,
          const std::vector<Shape *> &meshes);
    ~Scene();

    void Clear();

    const std::vector<Shape *> &GetMeshes() const;
    std::vector<Shape *> &GetMeshes();
    const std::vector<DirectionalLight> &GetDirLights() const;
    std::vector<DirectionalLight> &GetDirLights();
    const std::vector<PointLight> &GetPointLights() const;
    std::vector<PointLight> &GetPointLights();
    const std::vector<SpotLight> &GetSpotLights() const;
    std::vector<SpotLight> &GetSpotLights();

    void AddLight(const DirectionalLight &light);
    void AddLight(const PointLight &light);
    void AddLight(const SpotLight &light);
    void AddMesh(Shape *sh);

  private:
    std::vector<DirectionalLight> dir_lights;
    std::vector<PointLight> point_lights;
    std::vector<SpotLight> spot_lights;
    std::vector<Shape *> meshes;
};

}