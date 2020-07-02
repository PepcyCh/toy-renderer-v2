#pragma once

#include <cmath>

#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <type_traits>

namespace pepcy::gm {

template <typename T, size_t N>
class Vector {
  private:
    T data[N];

  public:
    // constructors
    explicit Vector(T val = 0) {
        for (int i = 0; i < N; i++) {
            data[i] = val;
        }
    }

    Vector(std::initializer_list<T> list) {
        int i = 0;
        for (auto val : list) {
            data[i++] = val;
        }
        for (; i < N; i++) {
            data[i] = 0;
        }
    }

    template <typename... Args, std::enable_if_t<sizeof...(Args) == N, int> = 0>
    Vector(Args... args) : Vector({args...}) {}

    template <size_t M>
    explicit Vector(const Vector<T, M> &rhs) {
        if constexpr (M < N) {
            std::copy(rhs.data, rhs.data + M, data);
            std::fill(data + M, data + N, 0);
        } else {
            std::copy(rhs.data, rhs.data + N, data);
        }
    }

    template <size_t M, typename... Args,
             std::enable_if_t<sizeof...(Args) + M <= N, int> = 0>
    Vector(const Vector<T, M> &rhs, Args... args) {
        std::copy(rhs.data, rhs.data + M, data);
        int len = sizeof...(args);
        T last[] = {args...};
        std::copy(last, last + len, data + M);
        std::fill(data + M + len, data + N, 0);
    }

    // copy operators
    Vector &operator=(const Vector &rhs) {
        std::copy(rhs.data, rhs.data + N, data);
        return *this;
    }

    template <size_t M>
    Vector &operator=(const Vector<T, M> &rhs) {
        if constexpr (M < N) {
            std::fill(data, data + N, 0);
            std::copy(rhs.data, rhs.data + M, data);
        } else {
            std::copy(rhs.data, rhs.data + N, data);
        }
        return *this;
    }

    // cast
    template <typename T2>
    operator Vector<T2, N>() {
        Vector<T2, N> res;
        for (int i = 0; i < N; i++) {
            res[i] = static_cast<T2>(data[i]);
        }
        return res;
    }

    // index
    T &operator[](size_t i) {
        return data[i];
    }
    const T operator[](size_t i) const {
        return data[i];
    }

    // calculation
    Vector operator+(const Vector &rhs) const {
        Vector res(0);
        for (int i = 0; i < N; i++) {
            res[i] = data[i] + rhs[i];
        }
        return res;
    }
    Vector &operator+=(const Vector &rhs) {
        return *this = *this + rhs;
    }
    Vector operator-(const Vector &rhs) const {
        Vector res(0);
        for (int i = 0; i < N; i++) {
            res[i] = data[i] - rhs[i];
        }
        return res;
    }
    Vector &operator-=(const Vector &rhs) {
        return *this = *this - rhs;
    }
    Vector operator-() const {
        return Vector(0) - *this;
    }

    Vector operator*(const Vector &rhs) const {
        Vector res(0);
        for (int i = 0; i < N; i++) {
            res[i] = data[i] * rhs[i];
        }
        return res;
    }
    Vector operator*(const T rhs) const {
        Vector res(0);
        for (int i = 0; i < N; i++) {
            res[i] = data[i] * rhs;
        }
        return res;
    }
    Vector &operator*=(const Vector &rhs) {
        return *this = *this * rhs;
    }
    Vector &operator*=(const T rhs) {
        return *this = *this * rhs;
    }
    Vector operator/(const Vector &rhs) const {
        Vector res(0);
        for (int i = 0; i < N; i++) {
            res[i] = data[i] / rhs[i];
        }
        return res;
    }
    Vector operator/(const T rhs) const {
        Vector res(0);
        T inv = 1 / rhs;
        for (int i = 0; i < N; i++) {
            res[i] = data[i] * inv;
        }
        return res;
    }
    Vector &operator/=(const Vector &rhs) {
        return *this = *this / rhs;
    }
    Vector &operator/=(const T rhs) {
        return *this = *this / rhs;
    }

    // compare
    bool operator==(const Vector &rhs) const {
        for (int i = 0; i < N; i++) {
            if (data[i] != rhs[i]) {
                return false;
            }
        }
        return true;
    }
    bool operator!=(const Vector &rhs) const {
        return !(*this == rhs);
    }

    // get pointer
    const T *Data() const {
        return &data[0];
    }

    // norm
    T Norm2() const {
        if constexpr (std::is_floating_point_v<T>) {
            T res = 0;
            for (int i = 0; i < N; i++) {
                res += data[i] * data[i];
            }
            return res;
        } else {
            return -1;
        }
    }
    T Norm() const {
        if constexpr (std::is_floating_point_v<T>) {
            return std::sqrt(Norm2());
        } else {
            return -1;
        }
    }

    template <typename S, size_t M>
    friend class Vector;
};

template <typename T, size_t N,
         std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
inline Vector<T, N> Normalize(const Vector<T, N> &vec) {
    T norm = vec.Norm();
    if (norm < std::numeric_limits<T>::epsilon()) {
        return vec;
    }
    T inv = 1 / norm;
    return vec * inv;
}

template <typename T, size_t N>
inline T Dot(const Vector<T, N> &a, const Vector<T, N> &b) {
    T res = 0;
    for (int i = 0; i < N; i++) {
        res += a[i] * b[i];
    }
    return res;
}

template <typename T>
inline Vector<T, 3> Cross(const Vector<T, 3> &a, const Vector<T, 3> &b) {
    return Vector<T, 3>({
            a[1] * b[2] - a[2] * b[1],
            a[2] * b[0] - a[0] * b[2],
            a[0] * b[1] - a[1] * b[0]
        });
}

template <typename T>
inline T Cross(const Vector<T, 2> &a, const Vector<T, 2> &b) {
    return a[0] * b[1] - a[1] * b[0];
}

template <typename T>
inline T Mix(const Vector<T, 3> &a, const Vector<T, 3> &b, const Vector<T, 3> &c) {
    return a[0] * b[1] * c[2] + a[2] * b[0] * c[1] + a[1] * b[2] * c[0] -
           a[2] * b[1] * c[0] - a[1] * b[0] * c[2] - a[0] * b[2] * c[1];
}

template <typename T, size_t N>
inline Vector<T, N> Min(const Vector<T, N> &a, const Vector<T, N> &b) {
    Vector<T, N> res;
    for (int i = 0; i < N; i++) {
        res[i] = std::min(a[i], b[i]);
    }
    return res;
}
template <typename T, size_t N>
inline Vector<T, N> Max(const Vector<T, N> &a, const Vector<T, N> &b) {
    Vector<T, N> res;
    for (int i = 0; i < N; i++) {
        res[i] = std::max(a[i], b[i]);
    }
    return res;
}

template <typename T, size_t N>
inline std::ostream &operator<<(std::ostream &out, const Vector<T, N> &vec) {
    out << "[ ";
    for (int i = 0; i < N - 1; i++) {
        out << vec[i] << ", ";
    }
    out << vec[N - 1] << " ]";
    return out;
}

using Vector2 = Vector<float, 2>;
using Vector3 = Vector<float, 3>;
using Vector4 = Vector<float, 4>;
using Vector2d = Vector<double, 2>;
using Vector3d = Vector<double, 3>;
using Vector4d = Vector<double, 4>;
using Vector2i = Vector<int, 2>;
using Vector3i = Vector<int, 3>;
using Vector4i = Vector<int, 4>;

}
