#include "../include/geometry.h"
#include <cmath>

namespace Engine3D {

    // vec3d implementations
    vec3d vec3d::operator+(const vec3d& other) const {
        return vec3d(x + other.x, y + other.y, z + other.z);
    }

    vec3d vec3d::operator-(const vec3d& other) const {
        return vec3d(x - other.x, y - other.y, z - other.z);
    }

    vec3d vec3d::operator*(float scalar) const {
        return vec3d(x * scalar, y * scalar, z * scalar);
    }

    float vec3d::dot(const vec3d& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    vec3d vec3d::cross(const vec3d& other) const {
        return vec3d(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    float vec3d::magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    vec3d vec3d::normalize() const {
        float mag = magnitude();
        if (mag > 0.0f) {
            return vec3d(x / mag, y / mag, z / mag);
        }
        return vec3d();
    }

    // triangle implementations
    triangle::triangle(const vec3d& p1, const vec3d& p2, const vec3d& p3) {
        p[0] = p1;
        p[1] = p2;
        p[2] = p3;
        computeNormal();
    }

    void triangle::computeNormal() {
        vec3d line1 = p[1] - p[0];
        vec3d line2 = p[2] - p[0];
        normal = line1.cross(line2).normalize();
    }

    // mesh implementations
    void mesh::addTriangle(const triangle& tri) {
        tris.push_back(tri);
    }

    void mesh::clear() {
        tris.clear();
    }

    size_t mesh::getTriangleCount() const {
        return tris.size();
    }

    // mat4x4 implementations
    mat4x4::mat4x4() {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                m[i][j] = 0.0f;
            }
        }
    }

    mat4x4 mat4x4::identity() {
        mat4x4 result;
        for (int i = 0; i < 4; i++) {
            result.m[i][i] = 1.0f;
        }
        return result;
    }

    mat4x4 mat4x4::translation(float x, float y, float z) {
        mat4x4 result = identity();
        result.m[3][0] = x;
        result.m[3][1] = y;
        result.m[3][2] = z;
        return result;
    }

    mat4x4 mat4x4::rotationX(float angle) {
        mat4x4 result = identity();
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        result.m[1][1] = cosA;
        result.m[1][2] = sinA;
        result.m[2][1] = -sinA;
        result.m[2][2] = cosA;
        return result;
    }

    mat4x4 mat4x4::rotationY(float angle) {
        mat4x4 result = identity();
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        result.m[0][0] = cosA;
        result.m[0][2] = sinA;
        result.m[2][0] = -sinA;
        result.m[2][2] = cosA;
        return result;
    }

    mat4x4 mat4x4::rotationZ(float angle) {
        mat4x4 result = identity();
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        result.m[0][0] = cosA;
        result.m[0][1] = sinA;
        result.m[1][0] = -sinA;
        result.m[1][1] = cosA;
        return result;
    }

    mat4x4 mat4x4::scale(float sx, float sy, float sz) {
        mat4x4 result = identity();
        result.m[0][0] = sx;
        result.m[1][1] = sy;
        result.m[2][2] = sz;
        return result;
    }

    mat4x4 mat4x4::perspective(float fov, float aspect, float near, float far) {
        mat4x4 result;
        float fovRad = 1.0f / std::tan(fov * 0.5f);
        
        result.m[0][0] = aspect * fovRad;
        result.m[1][1] = fovRad;
        result.m[2][2] = far / (far - near);
        result.m[3][2] = (-far * near) / (far - near);
        result.m[2][3] = 1.0f;
        result.m[3][3] = 0.0f;
        
        return result;
    }

    mat4x4 mat4x4::operator*(const mat4x4& other) const {
        mat4x4 result;
        for (int c = 0; c < 4; c++) {
            for (int r = 0; r < 4; r++) {
                result.m[r][c] = m[r][0] * other.m[0][c] + 
                                m[r][1] * other.m[1][c] + 
                                m[r][2] * other.m[2][c] + 
                                m[r][3] * other.m[3][c];
            }
        }
        return result;
    }

    vec3d mat4x4::multiplyVector(const vec3d& v) const {
        vec3d result;
        result.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + m[3][0];
        result.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + m[3][1];
        result.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + m[3][2];
        float w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + m[3][3];
        
        if (w != 0.0f && w != 1.0f) {
            result.x /= w;
            result.y /= w;
            result.z /= w;
        }
        
        return result;
    }

} // namespace Engine3D
