#pragma once

#include <memory>
#include <unordered_map>

#include "GID.h"
#include "Shape.h"
#include "VertexArray.h"

namespace pepcy::renderer {

class VertexArrayManager {
  public:
    VertexArray GetVertexArray(const Shape *shape);

    void Clear();

  private:
    std::unordered_map<gid::GID, VertexArray, gid::GIDHasher> vaos;
};

}