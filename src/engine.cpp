#include "../include/engine.h"
#include <cmath>
#include <fstream>
#include <sstream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Engine3D {
    void populateMatrix(matrix4x4& mat, float width, float height, float z_length) {
        float zNear = 0.1f;
        float zFar = z_length;
        float fov = 90.0f;
        float aspectRatio = height / width;
        float fovRad = 1.0f / tanf((fov * 0.5f) * (M_PI / 180.0f));

        mat.m[0][0] = aspectRatio * fovRad;
        mat.m[1][1] = fovRad;
        mat.m[2][2] = zFar / (zFar - zNear);
        mat.m[3][2] = (-zFar * zNear) / (zFar - zNear);
        mat.m[2][3] = 1.0f;
        mat.m[3][3] = 0.0f;
    }

    void createRotationMatrixX(matrix4x4& mat, float angle) {
        // Initialize as identity matrix
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                mat.m[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
        
        // Set rotation values
        mat.m[1][1] = cosf(angle);
        mat.m[1][2] = sinf(angle);
        mat.m[2][1] = -sinf(angle);
        mat.m[2][2] = cosf(angle);
    }

    void createRotationMatrixZ(matrix4x4& mat, float angle) {
        // Initialize as identity matrix
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                mat.m[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
        
        // Set rotation values
        mat.m[0][0] = cosf(angle);
        mat.m[0][1] = sinf(angle);
        mat.m[1][0] = -sinf(angle);
        mat.m[1][1] = cosf(angle);
    }

    triangle rotateTriangle(const triangle& tri, const matrix4x4& rotX, const matrix4x4& rotZ) {
        triangle rotated;
        
        // First rotate around Z axis, then X axis
        for (int i = 0; i < 3; i++) {
            vec3d rotatedZ = rotZ.multiplyVector(tri.points[i]);
            rotated.points[i] = rotX.multiplyVector(rotatedZ);
        }
        
        return rotated;
    }

    bool mesh::loadFromObjectFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        // Clear existing triangles
        triangles.clear();
        
        // Local cache of vertices
        vector<vec3d> vertices;

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            
            istringstream ss(line);
            string prefix;
            ss >> prefix;

            if (prefix == "v") {
                // Vertex line: v x y z
                vec3d vertex;
                ss >> vertex.x >> vertex.y >> vertex.z;
                vertices.push_back(vertex);
            }
            else if (prefix == "f") {
                // Face line: f v1 v2 v3 (1-indexed)
                int f[3];
                ss >> f[0] >> f[1] >> f[2];
                
                // Convert to 0-indexed and create triangle
                if (f[0] > 0 && f[1] > 0 && f[2] > 0 && 
                    f[0] <= vertices.size() && f[1] <= vertices.size() && f[2] <= vertices.size()) {
                    triangles.push_back(triangle(
                        vertices[f[0] - 1], 
                        vertices[f[1] - 1], 
                        vertices[f[2] - 1]
                    ));
                }
            }
        }

        file.close();
        return !triangles.empty();
    }

    float calculateLighting(const vec3d& normal, const vec3d& lightDirection) {
        // Dot product gives us the cosine of the angle between normal and light
        float dp = normal.dot(lightDirection.normalize());
        // Clamp to 0-1 range (no negative lighting)
        return max(0.0f, dp);
    }
}
