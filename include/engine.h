#ifndef ENGINE_H
#define ENGINE_H

using namespace std;
#include <vector>

namespace Engine3D {

    struct vec3d{
        float x, y, z;
        
        // Constructors
        vec3d() : x(0), y(0), z(0) {}
        vec3d(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    };

    struct triangle{
        vec3d points[3];
        
        // Constructor
        triangle() {}
        triangle(vec3d p1, vec3d p2, vec3d p3) {
            points[0] = p1;
            points[1] = p2;
            points[2] = p3;
        }
    };

    struct mesh{
        vector<triangle> triangles;
        vec3d position;
    };
}

#endif