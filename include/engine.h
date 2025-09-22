#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <iostream>
using namespace std;

namespace Engine3D {

    struct vec3d{
        float x, y, z;
        
        // Constructors
        vec3d() : x(0), y(0), z(0) {}
        vec3d(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

        vec3d operator -(const vec3d& v) const {
            return vec3d(x - v.x, y - v.y, z - v.z);
        }

        vec3d operator +(const vec3d& v) const {
            return vec3d(x + v.x, y + v.y, z + v.z);
        }

        vec3d operator *(const vec3d& v) const {
            return vec3d(x * v.x, y * v.y, z * v.z);
        }

        // Dot product
        float dot(const vec3d& v) const {
            return x * v.x + y * v.y + z * v.z;
        }

        // Cross product
        vec3d cross(const vec3d& v) const {
            return vec3d(
                y * v.z - z * v.y,
                z * v.x - x * v.z,
                x * v.y - y * v.x
            );
        }

        // Normalize vector
        vec3d normalize() const {
            float length = sqrt(x * x + y * y + z * z);
            if (length == 0.0f) return vec3d(0, 0, 0);
            return vec3d(x / length, y / length, z / length);
        }

        // Get length
        float length() const {
            return sqrt(x * x + y * y + z * z);
        }
    };

    struct triangle{
        vec3d points[3];
        vec3d normal;

        // Constructor
        triangle() {}
        triangle(vec3d p1, vec3d p2, vec3d p3) {
            points[0] = p1;
            points[1] = p2;
            points[2] = p3;
        }

        triangle operator +(const vec3d& v) const {
            return triangle(
                vec3d(points[0].x + v.x, points[0].y + v.y, points[0].z + v.z),
                vec3d(points[1].x + v.x, points[1].y + v.y, points[1].z + v.z),
                vec3d(points[2].x + v.x, points[2].y + v.y, points[2].z + v.z)
            );
        }

        triangle operator *(const vec3d& v) const {
            return triangle(
                vec3d(points[0].x * v.x, points[0].y * v.y, points[0].z * v.z),
                vec3d(points[1].x * v.x, points[1].y * v.y, points[1].z * v.z),
                vec3d(points[2].x * v.x, points[2].y * v.y, points[2].z * v.z)
            );
        }

        void calculateNormal() {
            vec3d u = points[1] - points[0];
            vec3d v = points[2] - points[0];
            normal = u.cross(v).normalize();
        }

        // Check if triangle is facing camera (for backface culling)
        bool isFacingCamera(const vec3d& cameraPos) const {
            vec3d toCamera = cameraPos - points[0];
            return normal.dot(toCamera) > 0.0f;
        }
    };

    struct mesh{
        vector<triangle> triangles;
        vec3d position;
        
        bool loadFromObjectFile(const string& filename);
    };

    struct matrix4x4{
        float m[4][4] = { 0 };

        vec3d multiplyVector(const vec3d& i) const {
            vec3d o;
            o.x = i.x * m[0][0] + i.y * m[1][0] + i.z * m[2][0] + m[3][0];
            o.y = i.x * m[0][1] + i.y * m[1][1] + i.z * m[2][1] + m[3][1];
            o.z = i.x * m[0][2] + i.y * m[1][2] + i.z * m[2][2] + m[3][2];
            float w = i.x * m[0][3] + i.y * m[1][3] + i.z * m[2][3] + m[3][3];
        
            if (w != 0.0f) {
                o.x /= w; o.y /= w; o.z /= w;
            }
            return o;
        }
    };

    // Function declarations
    void populateMatrix(matrix4x4& mat, float width, float height, float z_length);
    void createRotationMatrixX(matrix4x4& mat, float angle);
    void createRotationMatrixZ(matrix4x4& mat, float angle);
    triangle rotateTriangle(const triangle& tri, const matrix4x4& rotX, const matrix4x4& rotZ);
    float calculateLighting(const vec3d& normal, const vec3d& lightDirection);

}

#endif