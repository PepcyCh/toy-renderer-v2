#include "Mesh.h"

namespace pepcy::renderer {

const std::shared_ptr<Shape> &Mesh::GetShape() const {
    return shape;
}

const Material &Mesh::GetMaterial() const {
    return mat;
}
Material &Mesh::GetMaterial() {
    return mat;
}
void Mesh::SetMaterial(const Material &mat) {
    this->mat = mat;
}

gm::Transform Mesh::GetModel() const {
    return model;
}

void Mesh::SetModel(const gm::Transform &trans) {
    model = trans;
}

gm::Vector3 Mesh::GetCentroid() const {
    return model.TransformPoint(shape->GetCentroid());
}

void Mesh::ApplyTransform() {
    shape->ApplyTransform(model);
    // model = gm::Transform();
}

}