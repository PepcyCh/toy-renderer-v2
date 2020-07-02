#include "RayTraceViewer.h"

#include <future>
#include <random>

#include "stb_image_write.h"
#include "../defines.h"
#include "BasicShape.h"
#include "Triangle.h"

static std::random_device rnd_dv;
static std::mt19937 rnd_gen(rnd_dv());
static std::uniform_real_distribution rnd_real(0.0f, 1.0f);

namespace pepcy::renderer {

RayTraceViewer raytrace_viewer;

RayTraceViewer::RayTraceViewer() {
    img = nullptr;

    for (int i = 0; i < N_SAMPLES; i++) {
        samples[i][0] = float(i) / N_SAMPLES;
        float t = 0.5f, res = 0.0f;
        int j = i;
        while (j) {
            res += t * (j & 1);
            t /= 2.0f;
            j >>= 1;
        }
        samples[i][1] = res;
    }
}

RayTraceViewer::~RayTraceViewer() {
    delete[] img;
}

void RayTraceViewer::SetConfig(const RayTraceViewerConfig &config) {
    bool resize = config.width != this->config.width ||
        config.height != this->config.height;
    this->config = config;
    if (resize) {
        Resize(config.width, config.height);
    }
}

void RayTraceViewer::Resize(int width, int height) {
    delete[] img;
    img = new unsigned char[width * height * 3];
}

void RayTraceViewer::Draw() {
    ++n_shot;
    std::string name = "ray_trace_" + std::to_string(n_shot);

    std::cout << "build BVH" << std::endl;
    BuildBVH();
    // bvh_tree.Print();

    std::cout << "begin tracing" << std::endl;
    std::vector<std::future<void>> handles;
    for (int i = 0; i < config.height; i += 32) {
        for (int j = 0; j < config.width; j += 32) {
            int h = std::min(32, config.height - i);
            int w = std::min(32, config.width - j);
            handles.push_back(std::async(std::launch::async,
                [this, i, j, h, w]() { DrawQuad(i, j, h, w); }));
        }
    }
    for (auto &handle : handles) {
        handle.get();
    }

    std::string filename = shot_path + name + ".png";
    stbi_write_png(filename.c_str(), config.width, config.height, 3, img, config.width * 3);
    std::cout << "end" << std::endl;
}

void RayTraceViewer::DrawQuad(int i0, int j0, int h, int w) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            gm::Color col;
            for (int k = 0; k < N_SAMPLES; k++) {
                float x = j0 + j + samples[k][0];
                float y = i0 + i + samples[k][1];
                gm::Ray r = config.cam->GenRay(x / config.width, y / config.height);
                gm::Color res = Raytrace(r);
                col += res;
            }
            col /= N_SAMPLES;
            SetColor(i0 + i, j0 + j, col);
        }
    }
}

static gm::Vector3 Sample(float &pdf) {
    float xi1 = rnd_real(rnd_gen);
    float xi2 = rnd_real(rnd_gen);

    float sin = std::sqrt(xi1);
    float cos = std::sqrt(1 - xi1);
    float phi = 2.0 * gm::PI * xi2;

    float xs = sin * std::cos(phi);
    float ys = sin * std::sin(phi);
    float zs = cos;

    pdf = cos / gm::PI;

    return gm::Vector3(xs, ys, zs);
}

gm::Color RayTraceViewer::Raytrace(const gm::Ray &r, int depth) {
    if (depth >= MAX_TRACE_DEPTH) {
        return gm::Color();
    }

    Intersection inter;
    if (!bvh_tree.Intersect(r, inter)) {
        return gm::Color();
    }

    gm::Vector3 hit_p = r.orig + r.dir * inter.t;
    gm::Vector3 hit_n = inter.norm;
    gm::Vector3 hit_t = inter.tan;
    gm::Vector3 hit_b = gm::Cross(hit_n, hit_t);
    gm::Matrix3 o2w(hit_t, hit_b, hit_n);
    gm::Matrix3 w2o = gm::Transpose(o2w);
    
    gm::Vector3 w_out = gm::Normalize(w2o * (r.orig - hit_p));
    gm::Color f;
    if (auto p = dynamic_cast<const Triangle *>(inter.prim)) {
        auto albedo = p->GetMaterial().GetTexture("albedo");
        if (albedo.IsColor()) {
            f = albedo.GetColor() * gm::PI_INV;
        } else { // TODO - sample texture
            f = gm::Color(1.0f, 1.0f, 1.0f) * gm::PI_INV;
        }
    }

    gm::Color L_out;
    for (const auto &light : config.scene->GetDirLights()) {
        gm::Color L_light = light.color;
        gm::Vector3 light_dir = -light.dir;
        gm::Vector3 w_in = w2o * light_dir;
        if (w_in[2] < 0) {
            continue;
        }
        float cos = w_in[2];
        gm::Ray shadow(hit_p + hit_n * 0.001f, light_dir);
        if (!bvh_tree.Intersect(shadow)) {
            L_out += f * L_light * cos;
        }
    }
    for (const auto &light : config.scene->GetPointLights()) {
        gm::Color L_light = light.color;
        gm::Vector3 light_dir = light.pos - hit_p;
        gm::Vector3 w_in = w2o * light_dir;
        if (w_in[2] < 0) {
            continue;
        }
        float cos = w_in[2];
        gm::Ray shadow(hit_p + hit_n * 0.005f, light_dir);
        float dist = light_dir.Norm();
        if (!bvh_tree.Intersect(shadow)) {
            L_out += f * L_light * cos * light.GetAtten(dist / 10.0f);
            // L_out += f * L_light * cos;
        }
    }
    for (const auto &light : config.scene->GetSpotLights()) {
        gm::Color L_light = light.color;
        gm::Vector3 light_dir = light.pos - hit_p;
        gm::Vector3 w_in = w2o * light_dir;
        if (w_in[2] < 0) {
            continue;
        }
        float cos = w_in[2];
        gm::Ray shadow(hit_p + hit_n * 0.001f, light_dir);
        float dist = light_dir.Norm();
        float theta = std::acos(gm::Dot(light_dir, light.dir));
        float atten = light.GetAtten(dist / 10.0f, theta);
        if (!bvh_tree.Intersect(shadow)) {
            L_out += f * L_light * cos * atten;
            // L_out += f * L_light * cos;
        }
    }

    
    float pdf;
    gm::Color fr = f;
    gm::Vector3 w_in = Sample(pdf);

    // Russian roulette
    float prob = 1.0;
    if (fr.Luminance() < 0.5) {
        prob = 0.5;
    }
    if (rnd_real(rnd_gen) > prob) {
        return L_out;
    }

    gm::Ray ri(hit_p, o2w * w_in);
    gm::Color Li = Raytrace(ri, depth + 1);
    L_out += fr * Li * (std::abs(w_in[2]) / (pdf * prob));

    return L_out;
}

void RayTraceViewer::BuildBVH() {
    std::vector<Primitive *> prims;
    for (auto &mesh : config.scene->GetMeshes()) {
        int M = mesh->GetIndexCount();
        const unsigned int *p_ind = mesh->GetIndices();
        for (int i = 0; i < M; i += 3) {
            prims.push_back(new Triangle(mesh, p_ind[i], p_ind[i + 1], p_ind[i + 2]));
        }
    }

    bvh_tree.Build(prims);
}

void RayTraceViewer::SetColor(int i, int j, const gm::Color &col) {
    int ind = ((config.height - i - 1) * config.width + j) * 3;
    img[ind] = std::pow(col.r / (col.r + 1.0f), 1.0f/ 2.2f) * 255;
    img[ind + 1] = std::pow(col.g / (col.g + 1.0f), 1.0f/ 2.2f) * 255;
    img[ind + 2] = std::pow(col.b / (col.b + 1.0f), 1.0f/ 2.2f) * 255;
}

}