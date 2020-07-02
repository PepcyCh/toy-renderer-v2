#pragma once

#include <algorithm>

namespace pepcy::gm {

class Color {
  public:
    float r, g, b, a;

    Color(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) :
        r(r), g(g), b(b), a(a) {}

    gm::Color Clamp() {
        r = std::clamp(r, 0.0f, 1.0f);
        g = std::clamp(g, 0.0f, 1.0f);
        b = std::clamp(b, 0.0f, 1.0f);
        a = std::clamp(a, 0.0f, 1.0f);
        return *this;
    }

    Color operator*(float scale) const {
        return Color(r * scale, g * scale, b * scale, a);
    }
    Color &operator*=(float scale) {
        return *this = *this * scale;
    }
    Color operator*(const gm::Color &rhs) const {
        return Color(r * rhs.r, g * rhs.g, b * rhs.b, a);
    }
    Color &operator*=(const gm::Color &rhs) {
        return *this = *this * rhs;
    }
    Color operator/(float scale) const {
        float inv = 1.0f / scale;
        return Color(r * inv, g * inv, b * inv, a);
    }
    Color &operator/=(float scale) {
        return *this = *this / scale;
    }
    Color operator+(const Color &rhs) const {
        return Color(r + rhs.r, g + rhs.g, b + rhs.b, a);
    }
    Color &operator+=(const Color &rhs) {
        return *this = *this + rhs;
    }

    float Luminance() const {
        return 0.299 * r + 0.587 * g + 0.114 * b;
    }

    Color Over(const Color &rhs) {
        Color mc = *this * a;
        Color mr = rhs * a;
        Color mix = mc + mr * (1 - a);
        if (mix.a > 0) {
            mix.r /= mix.a;
            mix.g /= mix.a;
            mix.b /= mix.a;
        }
        return mix;
    }
};

inline Color operator*(float scale, const Color &col) {
    return col * scale;
}

inline std::ostream &operator<<(std::ostream &out, const Color &col) {
    return out << "(" << col.r << ", " << col.g << ", " << col.b <<
        ", " << col.a << ")";
}

}