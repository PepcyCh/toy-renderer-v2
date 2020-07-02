#include "Scene.h"

namespace pepcy::renderer {

Scene::Scene(const std::vector<DirectionalLight> &dir_lights,
             const std::vector<PointLight> &point_lights,
             const std::vector<SpotLight> &spot_lights,
             const std::vector<Shape *> &meshes) :
    dir_lights(dir_lights), point_lights(point_lights),
    spot_lights(spot_lights), meshes(meshes) {}

Scene::~Scene() {}

void Scene::Clear() {
    for (auto &mesh : meshes) {
        delete mesh;
        mesh = nullptr;
    }
    meshes.clear();
    dir_lights.clear();
    point_lights.clear();
    spot_lights.clear();
}

const std::vector<Shape *> &Scene::GetMeshes() const {
    return meshes;
}
std::vector<Shape *> &Scene::GetMeshes() {
    return meshes;
}
const std::vector<DirectionalLight> &Scene::GetDirLights() const {
    return dir_lights;
}
std::vector<DirectionalLight> &Scene::GetDirLights() {
    return dir_lights;
}
const std::vector<PointLight> &Scene::GetPointLights() const {
    return point_lights;
}
std::vector<PointLight> &Scene::GetPointLights() {
    return point_lights;
}
const std::vector<SpotLight> &Scene::GetSpotLights() const {
    return spot_lights;
}
std::vector<SpotLight> &Scene::GetSpotLights() {
    return spot_lights;
}

void Scene::AddLight(const DirectionalLight &light) {
    dir_lights.push_back(light);
}
void Scene::AddLight(const PointLight &light) {
    point_lights.push_back(light);
}
void Scene::AddLight(const SpotLight &light) {
    spot_lights.push_back(light);
}

void Scene::AddMesh(Shape *sh) {
    meshes.push_back(sh);
}

}