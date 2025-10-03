#include <iostream>
#include <SDL.h>
#include <cmath>
#include "../include/engine.h"
#include "../graphics/renderer.h"

using namespace Engine3D;
using namespace std;

void populateCube(mesh& m);

int main(){
    mesh shape;
    vec3d cameraPos(0.0f, 0.0f, 0.0f);
    
    // Try to load tetrahedron from file, fallback to cube if failed
    // if (!shape.loadFromObjectFile("tetrahedron.obj")) {
    //     cout << "Failed to load tetrahedron.obj, using default cube" << endl;
    //     populateCube(shape);
    // } else {
    //     cout << "Successfully loaded tetrahedron.obj with " << shape.triangles.size() << " triangles" << endl;
    // }
    populateCube(shape);
    
    int width = 800;
    int height = 600;
    int z_length = 1000;

    // Create renderer
    Renderer renderer(width, height);

    if (!renderer.init()) {
        cout << "Failed to initialize renderer!" << endl;
        return -1;
    }
    
    matrix4x4 projection;
    populateMatrix(projection, width, height, z_length);

    // Rotation variables
    float theta = 0.0f;
    
    bool running = true;
    SDL_Event event;
    while (running) {

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Update rotation angle
        theta += 0.01f;

        // Create rotation matrices
        matrix4x4 rotX, rotZ;
        createRotationMatrixX(rotX, theta);
        createRotationMatrixZ(rotZ, theta);

        // Render
        renderer.clear();
        renderer.drawMesh(shape, projection, rotX, rotZ, cameraPos);
        renderer.present();
        
        // Cap framerate
        SDL_Delay(16);
    }
    
    renderer.cleanup();
    return 0;
}

void populateCube(mesh& m) {
    // Define the 8 vertices of a cube
    vec3d vertices[8] = {
        vec3d(0.0f, 0.0f, 0.0f), // 0: front bottom left
        vec3d(0.0f, 0.0f, 1.0f), // 1: back bottom right
        vec3d(0.0f, 1.0f, 0.0f), // 2: front top left
        vec3d(0.0f, 1.0f, 1.0f), // 3: back top left
        vec3d(1.0f, 0.0f, 0.0f), // 4: front bottom right
        vec3d(1.0f, 0.0f, 1.0f), // 5: back bottom right
        vec3d(1.0f, 1.0f, 0.0f), // 6: front top right
        vec3d(1.0f, 1.0f, 1.0f)  // 7: back top right
    };
    
    // Front face
    m.triangles.push_back(triangle(vertices[0], vertices[2], vertices[6]));
    m.triangles.push_back(triangle(vertices[0], vertices[6], vertices[4]));

    // Back face
    m.triangles.push_back(triangle(vertices[5], vertices[7], vertices[3]));
    m.triangles.push_back(triangle(vertices[5], vertices[3], vertices[1]));

    // Left face
    m.triangles.push_back(triangle(vertices[1], vertices[3], vertices[2]));
    m.triangles.push_back(triangle(vertices[1], vertices[2], vertices[0]));

    // Right face
    m.triangles.push_back(triangle(vertices[4], vertices[6], vertices[7]));
    m.triangles.push_back(triangle(vertices[4], vertices[7], vertices[5]));
    
    // Top face
    m.triangles.push_back(triangle(vertices[2], vertices[3], vertices[7]));
    m.triangles.push_back(triangle(vertices[2], vertices[7], vertices[6]));

    // Bottom face
    m.triangles.push_back(triangle(vertices[5], vertices[1], vertices[0]));
    m.triangles.push_back(triangle(vertices[5], vertices[0], vertices[4]));
}
