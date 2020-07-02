#pragma once

#include <memory>
#include <vector>

#include "Primitive.h"

namespace pepcy::renderer {

class BVHNode {
  public:
    BVHNode(const gm::BBox &bbox, int start, int len);

    bool IsLeaf() const;

  private:
    std::shared_ptr<BVHNode> lc, rc;
    gm::BBox bbox;
    int start, len;

    friend class BVHTree;
};

class BVHTree {
  public:
    void Build(const std::vector<Primitive *> &prims);

    bool Intersect(const gm::Ray &r) const;
    bool Intersect(const gm::Ray &r, Intersection &inter) const;

    void Print() const;

  private:
    std::vector<Primitive *> prims;
    std::shared_ptr<BVHNode> root;
};

}