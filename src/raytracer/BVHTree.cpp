#include "BVHTree.h"

#include <queue>
#include <stack>

namespace pepcy::renderer {

BVHNode::BVHNode(const gm::BBox &bbox, int start, int len) : bbox(bbox), 
        start(start), len(len), lc(), rc() {}

bool BVHNode::IsLeaf() const {
    return !lc && !rc;
}

void BVHTree::Build(const std::vector<Primitive *> &prims_) {
    prims = prims_;

    gm::BBox bbox;
    for (int i = 0; i < prims.size(); i++) {
        bbox.Expand(prims[i]->GetBBox());
    }
    root = std::make_shared<BVHNode>(bbox, 0, prims.size());

    std::queue<std::shared_ptr<BVHNode>> q;
    q.push(root);
    static const int max_leaf_size = 32;
    static const int B = 16;
    while (!q.empty()) {
        auto u = q.front();
        q.pop();
        if (u->len <= max_leaf_size) {
            continue;
        }

        int best_d = -1, best_i = -1;
        float SN = u->bbox.SurfaceArea();
        float best_c = std::numeric_limits<float>::max();

        for (int d = 0; d < 3; d++) {
            std::vector<gm::BBox> boxes(B);
            std::vector<std::vector<Primitive *>> prims_tmp(B);
            float min = u->bbox.p_min[d], max = u->bbox.p_max[d];
            float length = (max - min) / B;
            if (length == 0) continue;

            for (int i = 0; i < u->len; i++) {
                gm::BBox cb = prims[u->start + i]->GetBBox();
                float p = cb.Centroid()[d];
                int buc = std::clamp<int>((p - min) / length, 0, B - 1);
                prims_tmp[buc].push_back(prims[u->start + i]);
                boxes[buc].Expand(cb);
            }

            for (int i = 1; i < B; i++) {
                gm::BBox lb, rb;
                int ln = 0, rn = 0;
                for (int j = 0; j < i; j++) {
                    lb.Expand(boxes[j]);
                    ln += prims_tmp[j].size();
                }
                for (int j = i; j < B; j++) {
                    rb.Expand(boxes[j]);
                    rn += prims_tmp[j].size();
                }
                float SA = lb.SurfaceArea(), SB = rb.SurfaceArea();
                float C = SA / SN * ln + SB / SN * rn;
                if (C < best_c) {
                    best_d = d;
                    best_i = i;
                    best_c = C;
                }
            }
        }

        float min = u->bbox.p_min[best_d], max = u->bbox.p_max[best_d];
        float length = (max - min) / B;
        gm::BBox lb, rb;
        std::vector<Primitive *> lp, rp;
        for (int i = 0; i < u->len; i++) {
            gm::BBox cb = prims[u->start + i]->GetBBox();
            float p = cb.Centroid()[best_d];
            int buc = std::clamp<int>((p - min) / length, 0, B - 1);
            if (buc < best_i) {
                lb.Expand(cb);
                lp.push_back(prims[u->start + i]);
            } else {
                rb.Expand(cb);
                rp.push_back(prims[u->start + i]);
            }
        }

        if (lp.size() == 0 || lp.size() == u->len) {
            lb = gm::BBox();
            rb = gm::BBox();
            int hn = u->len / 2;
            for (int i = 0; i < hn; i++) {
                lb.Expand(prims[u->start + i]->GetBBox());
            }
            for (int i = hn; i < u->len; i++) {
                rb.Expand(prims[u->start + i]->GetBBox());
            }
            u->lc = std::make_shared<BVHNode>(lb, u->start, hn);
            u->rc = std::make_shared<BVHNode>(rb, u->start + hn, u->len - hn);
            q.push(u->lc);
            q.push(u->rc);
        } else {
            int p = 0;
            for (auto prim : lp) {
                prims[u->start + p] = prim;
                ++p;
            }
            int ln = p;
            for (auto prim : rp) {
                prims[u->start + p] = prim;
                ++p;
            }
            u->lc = std::make_shared<BVHNode>(lb, u->start, ln);
            u->rc = std::make_shared<BVHNode>(rb, u->start + ln, u->len - ln);
            q.push(u->lc);
            q.push(u->rc);
        }
    }
}

bool BVHTree::Intersect(const gm::Ray &r) const {
    bool flag = false;
    std::stack<std::shared_ptr<BVHNode>> s;
    s.push(root);
    while (!s.empty()) {
        auto u = s.top();
        s.pop();

        float t0, t1;
        if (u->bbox.Intersect(r, t0, t1)) {
            if (u->IsLeaf()) {
                for (int i = 0; i < u->len; i++) {
                    if (prims[u->start + i]->Intersect(r)) {
                        flag = true;
                        break;
                    }
                }
                if (flag) break;
            } else {
                if (u->lc) s.push(u->lc);
                if (u->rc) s.push(u->rc);
            }
        }
    }
    return flag;
}
bool BVHTree::Intersect(const gm::Ray &r, Intersection &inter) const {
    bool flag = false;
    std::stack<std::shared_ptr<BVHNode>> s;
    s.push(root);
    while (!s.empty()) {
        auto u = s.top();
        s.pop();

        float t0, t1;
        if (u->bbox.Intersect(r, t0, t1)) {
            if (u->IsLeaf()) {
                for (int i = 0; i < u->len; i++) {
                    if (prims[u->start + i]->Intersect(r, inter)) {
                        flag = true;
                    }
                }
            } else {
                if (u->lc) s.push(u->lc);
                if (u->rc) s.push(u->rc);
                float lt0, lt1;
                bool flag_l = false;
                if (u->lc) {
                    flag_l = u->lc->bbox.Intersect(r, lt0, lt1);
                }
                float rt0, rt1;
                bool flag_r = false;
                if (u->rc) {
                    flag_r = u->rc->bbox.Intersect(r, rt0, rt1);
                }

                if (flag_l && flag_r) {
                    if (lt0 < rt0) {
                        s.push(u->rc);
                        s.push(u->lc);
                    } else {
                        s.push(u->lc);
                        s.push(u->rc);
                    }
                } else if (flag_l) {
                    s.push(u->lc);
                } else if (flag_r) {
                    s.push(u->rc);
                }
            }
        }
    }
    return flag;
}

void BVHTree::Print() const {
    std::stack<std::pair<std::shared_ptr<BVHNode>, int>> s;
    s.emplace(root, 0);
    while (!s.empty()) {
        const auto &[u, dep] = s.top();
        s.pop();
        std::string indent(dep, '-');
        std::cout << indent << "(" << u->start << ", " << u->len << ")" << std::endl;
        std::cout << indent << "bbox: (" << u->bbox.p_min << ", " << u->bbox.p_max << ")" << std::endl;
        std::cout << indent << "lc ?= " << !!u->lc << ", rc ?= " << !!u->rc << std::endl;
        if (u->rc) {
            s.emplace(u->rc, dep + 1);
        }
        if (u->lc) {
            s.emplace(u->lc, dep + 1);
        }
    }
    for (int i = 0; i < prims.size(); i++) {
        std::cout << "i = " << i << ", bbox = (" << prims[i]->GetBBox().p_min <<
            ", " << prims[i]->GetBBox().p_max << std::endl;
    }
}

}