#pragma once

#include <vector>

namespace Engine3D {

    // Basic 3D vector structure
    struct vec3d {
        float x, y, z;
        
        // Constructor
        vec3d() : x(0.0f), y(0.0f), z(0.0f) {}
        vec3d(float x, float y, float z) : x(x), y(y), z(z) {}
        
        // Basic vector operations
        vec3d operator+(const vec3d& other) const;
        vec3d operator-(const vec3d& other) const;
        vec3d operator*(float scalar) const;
        float dot(const vec3d& other) const;
        vec3d cross(const vec3d& other) const;
        float magnitude() const;
        vec3d normalize() const;
    };

    // Triangle structure representing a face with 3 vertices
    struct triangle {
        vec3d p[3];  // Three vertices
        vec3d normal;  // Face normal (computed)
        
        triangle() = default;
        triangle(const vec3d& p1, const vec3d& p2, const vec3d& p3);
        
        void computeNormal();
    };

    // Mesh structure containing multiple triangles
    struct mesh {
        std::vector<triangle> tris;
        
        void addTriangle(const triangle& tri);
        void clear();
        size_t getTriangleCount() const;
    };

    // 4x4 Matrix for transformations
    struct mat4x4 {
        float m[4][4];
        
        mat4x4();
        static mat4x4 identity();
        static mat4x4 translation(float x, float y, float z);
        static mat4x4 rotationX(float angle);
        static mat4x4 rotationY(float angle);
        static mat4x4 rotationZ(float angle);
        static mat4x4 scale(float sx, float sy, float sz);
        static mat4x4 perspective(float fov, float aspect, float near, float far);
        
        mat4x4 operator*(const mat4x4& other) const;
        vec3d multiplyVector(const vec3d& v) const;
    };

} // namespace Engine3D
