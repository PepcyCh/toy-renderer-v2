#pragma once

#include "Scene.h"
#include "BVHTree.h"

namespace pepcy::renderer {

struct RayTraceViewerConfig {
    const Scene *scene;
    const Camera *cam;
    int width;
    int height;
};

class RayTraceViewer {
  public:
    RayTraceViewer();
    ~RayTraceViewer();

    void BuildBVH();
    void Draw();
    void SetColor(int i, int j, const gm::Color &col);

    void SetConfig(const RayTraceViewerConfig &config);
    void Resize(int width, int height);

  private:
    gm::Color Raytrace(const gm::Ray &r, int depth = 0);
    void DrawQuad(int x0, int y0, int w, int h);

    int n_shot = 0;

    const static int MAX_TRACE_DEPTH = 4;

    RayTraceViewerConfig config;
    unsigned char *img;
    BVHTree bvh_tree;

    const static int N_SAMPLES = 16;
    float samples[N_SAMPLES][2];
};

extern RayTraceViewer raytrace_viewer;

}