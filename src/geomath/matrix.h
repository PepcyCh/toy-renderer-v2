#pragma once

#include "vector.h"

namespace pepcy::gm {

template<typename T, size_t C, size_t R>
class Matrix {
  private:
    Vector<T, R> data[C];

  public:
    // constructors
    Matrix() = default;

    Matrix(std::initializer_list<T> list) {
        int c = 0, r = 0;
        for (auto i : list) {
            data[c][r] = i;
            ++r;
            if (r == R) {
                r = 0;
                ++c;
            }
        }
    }

    template<typename... Args,
            std::enable_if_t<sizeof...(Args) == C * R, bool> = false>
    Matrix(Args... args) : Matrix({args...}) {}

    explicit Matrix(T val) {
        if constexpr (C == R) {
            for (int i = 0; i < R; i++) {
                data[i][i] = val;
            }
        }
    }

    Matrix(std::initializer_list <Vector<T, R>> list) {
        int i = 0;
        for (auto &vec : list) {
            data[i++] = vec;
        }
        for (; i < C; i++) {
            data[i] = Vector<T, R>(0);
        }
    }

    template<typename... Args, std::enable_if_t<sizeof...(Args) == C, int> = 0>
    Matrix(Args... columns) : Matrix({columns...}) {}

    template<size_t C2, size_t R2>
    explicit Matrix(const Matrix<T, C2, R2> &rhs) {
        if constexpr (C2 < C) {
            for (int i = 0; i < C2; i++) {
                data[i] = rhs[i];
            }
            for (int i = C2; i < C; i++) {
                data[i] = Vector<T, R>(0);
            }
        } else {
            for (int i = 0; i < C; i++) {
                data[i] = rhs[i];
            }
        }
    }

    // copy operators
    Matrix &operator=(const Matrix &rhs) {
        std::copy(rhs.data, rhs.data + C, data);
        return *this;
    }

    template<size_t C2, size_t R2>
    Matrix &operator=(const Matrix<T, C2, R2> &rhs) {
        if constexpr (C2 < C) {
            for (int i = 0; i < C2; i++) {
                data[i] = rhs[i];
            }
            for (int i = C2; i < C; i++) {
                data[i] = Vector<T, R>(0);
            }
        } else {
            for (int i = 0; i < C; i++) {
                data[i] = rhs[i];
            }
        }
        return *this;
    }

    // cast
    template <typename T2>
    operator Matrix<T2, C, R>() {
        Matrix<T2, C, R> res;
        for (int i = 0; i < C; i++) {
            res[i] = static_cast<Vector<T2, R>>(data[i]);
        }
        return res;
    }

    // index
    Vector<T, R> &operator[](size_t i) {
        return data[i];
    }

    const Vector<T, R> &operator[](size_t i) const {
        return data[i];
    }

    // calculation
    Matrix operator+(const Matrix &rhs) const {
        Matrix res;
        for (int i = 0; i < C; i++) {
            res[i] = data[i] + rhs[i];
        }
        return res;
    }
    Matrix &operator+=(const Matrix &rhs) {
        return *this = *this + rhs;
    }
    Matrix operator-(const Matrix &rhs) const {
        Matrix res;
        for (int i = 0; i < C; i++) {
            res[i] = data[i] - rhs[i];
        }
        return res;
    }
    Matrix &operator-=(const Matrix &rhs) {
        return *this = *this - rhs;
    }
    Matrix operator*(const T rhs) const {
        Matrix res;
        for (int i = 0; i < C; i++) {
            res[i] = data[i] * rhs;
        }
        return res;
    }
    Matrix &operator*=(const T rhs) {
        return *this = *this * rhs;
    }
    Matrix operator/(const T rhs) const {
        Matrix res;
        T inv = 1 / rhs;
        for (int i = 0; i < C; i++) {
            res[i] = data[i] * inv;
        }
        return res;
    }
    Matrix &operator/=(const T rhs) {
        return *this = *this / rhs;
    }
    template <size_t P>
    Matrix operator*(const Matrix<T, P, C> &rhs) const {
        Matrix<T, P, R> res;
        for (int i = 0; i < P; i++) {
            for (int j = 0; j < C; j++) {
                for (int k = 0; k < R; k++) {
                    res[i][k] += data[j][k] * rhs[i][j];
                }
            }
        }
        return res;
    }
    template <size_t P>
    Matrix &operator*=(const Matrix<T, P, C> &rhs) {
        return *this = *this * rhs;
    }
    Vector<T, R> operator*(const Vector<T, C> &vec) const {
        Vector<T, R> res(0);
        for (int i = 0; i < C; i++) {
            res += data[i] * vec[i];
        }
        return res;
    }

    // compare
    bool operator==(const Matrix &rhs) const {
        for (int i = 0; i < C; i++) {
            if (data[i] != rhs[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Matrix &rhs) const {
        return !(*this == rhs);
    }

    // get pointer
    const T *Data() const {
        return data[0].Data();
    }

    template<typename T2, size_t C2, size_t R2>
    friend class Matrix;
};

template <typename T, size_t C, size_t R>
Matrix<T, R, C> Transpose(const Matrix<T, C, R> &m) {
    Matrix<T, R, C> res;
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            res[i][j] = m[j][i];
        }
    }
    return res;
}

template <typename T, size_t N,
         std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
Matrix<T, N, N> Inverse(const Matrix<T, N, N> &m) {
    Matrix<T, N, N> res(1);
    Matrix<T, N, N> tmp = m;
    for (int i = 0; i < N; i++) {
        int pivot = i;
        T max = std::abs(tmp[i][i]);
        for (int j = i + 1; j < N; j++) {
            T val = std::abs(tmp[i][j]);
            if (val > max) {
                max = val;
                pivot = j;
            }
        }

        if (i != pivot) {
            for (int j = 0; j < N; j++) {
                std::swap(tmp[j][i], tmp[j][pivot]);
                std::swap(res[j][i], res[j][pivot]);
            }
        }

        if (max < std::numeric_limits<T>::epsilon()) return m;
        T pivinv = 1 / max;
        for (int j = 0; j < N; j++) if (i != j) {
            T f = tmp[i][j] * pivinv;
            for (int k = 0; k < N; k++) {
                tmp[k][j] -= f * tmp[k][i];
                res[k][j] -= f * res[k][i];
            }
        }

        for (int j = 0; j < N; j++) {
            tmp[j][i] *= pivinv;
            res[j][i] *= pivinv;
        }
    }

    return res;
}

template <typename T, size_t C, size_t R>
inline std::ostream &operator<<(std::ostream &out, const Matrix<T, C, R> &mat) {
    out << "[ ";
    for (int i = 0; i < R; i++) {
        out << "[ ";
        for (int j = 0; j < C - 1; j++) {
            out << mat[j][i] << ", ";
        }
        out << mat[C - 1][i] << " ]";
        if (i == R - 1) {
            out << " ]";
        } else {
            out << ", ";
        }
    }
    return out;
}

using Matrix2 = Matrix<float, 2, 2>;
using Matrix3 = Matrix<float, 3, 3>;
using Matrix4 = Matrix<float, 4, 4>;
using Matrix2x2 = Matrix<float, 2, 2>;
using Matrix2x3 = Matrix<float, 2, 3>;
using Matrix2x4 = Matrix<float, 2, 4>;
using Matrix3x2 = Matrix<float, 3, 2>;
using Matrix3x3 = Matrix<float, 3, 3>;
using Matrix3x4 = Matrix<float, 3, 4>;
using Matrix4x2 = Matrix<float, 4, 2>;
using Matrix4x3 = Matrix<float, 4, 3>;
using Matrix4x4 = Matrix<float, 4, 4>;

using Matrix2d = Matrix<double, 2, 2>;
using Matrix3d = Matrix<double, 3, 3>;
using Matrix4d = Matrix<double, 4, 4>;
using Matrix2x2d = Matrix<double, 2, 2>;
using Matrix2x3d = Matrix<double, 2, 3>;
using Matrix2x4d = Matrix<double, 2, 4>;
using Matrix3x2d = Matrix<double, 3, 2>;
using Matrix3x3d = Matrix<double, 3, 3>;
using Matrix3x4d = Matrix<double, 3, 4>;
using Matrix4x2d = Matrix<double, 4, 2>;
using Matrix4x3d = Matrix<double, 4, 3>;
using Matrix4x4d = Matrix<double, 4, 4>;

using Matrix2i = Matrix<int, 2, 2>;
using Matrix3i = Matrix<int, 3, 3>;
using Matrix4i = Matrix<int, 4, 4>;
using Matrix2x2i = Matrix<int, 2, 2>;
using Matrix2x3i = Matrix<int, 2, 3>;
using Matrix2x4i = Matrix<int, 2, 4>;
using Matrix3x2i = Matrix<int, 3, 2>;
using Matrix3x3i = Matrix<int, 3, 3>;
using Matrix3x4i = Matrix<int, 3, 4>;
using Matrix4x2i = Matrix<int, 4, 2>;
using Matrix4x3i = Matrix<int, 4, 3>;
using Matrix4x4i = Matrix<int, 4, 4>;

}