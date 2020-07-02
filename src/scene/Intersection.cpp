#include "Intersection.h"

namespace pepcy::renderer {

void Intersection::TransformedBy(const gm::Transform &trans) {
    norm = trans.TransformNormal(norm);
    tan = trans.TransformNormal(tan);
}

}