#include <iostream>
#include <SDL.h>
#include <cmath>
#include "../include/engine.h"
#include "../graphics/renderer.h"

using namespace Engine3D;
using namespace std;

void populateCube(mesh& m);

int main(){
    mesh cube;
    populateCube(cube);
    
    // Create renderer
    Renderer renderer(800, 600);
    
    if (!renderer.init()) {
        cout << "Failed to initialize renderer!" << endl;
        return -1;
    }
    
    // Game loop
    bool running = true;
    SDL_Event e;
    
    float rotationY = 0.0f;
    float rotationX = 0.0f;
    
    while (running) {
        // Handle events
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
            }
        }
        
        // Simple rotation animation on both axes
        rotationY += 0.02f;
        rotationX += 0.01f;
        
        // Apply rotation to the cube (Y-axis and X-axis rotation)
        mesh rotatedCube = cube; 
        for (auto& tri : rotatedCube.triangles) {
            for (int i = 0; i < 3; i++) {
                float x = tri.points[i].x;
                float y = tri.points[i].y;
                float z = tri.points[i].z;
                
                // Y-axis rotation
                float newX = x * cos(rotationY) - z * sin(rotationY);
                float newZ = x * sin(rotationY) + z * cos(rotationY);
                
                // X-axis rotation
                float newY = y * cos(rotationX) - newZ * sin(rotationX);
                newZ = y * sin(rotationX) + newZ * cos(rotationX);
                
                tri.points[i].x = newX;
                tri.points[i].y = newY;
                tri.points[i].z = newZ;
            }
        }
        
        // Render
        renderer.clear();
        renderer.drawMesh(rotatedCube);
        renderer.present();
        
        // Cap framerate
        SDL_Delay(16);
    }
    
    return 0;
}

void populateCube(mesh& m) {
    // Define the 8 vertices of a cube
    vec3d vertices[8] = {
        vec3d(-0.5f, -0.5f, -0.5f), // 0: front bottom left
        vec3d( 0.5f, -0.5f, -0.5f), // 1: front bottom right
        vec3d( 0.5f,  0.5f, -0.5f), // 2: front top right
        vec3d(-0.5f,  0.5f, -0.5f), // 3: front top left
        vec3d(-0.5f, -0.5f,  0.5f), // 4: back bottom left
        vec3d( 0.5f, -0.5f,  0.5f), // 5: back bottom right
        vec3d( 0.5f,  0.5f,  0.5f), // 6: back top right
        vec3d(-0.5f,  0.5f,  0.5f)  // 7: back top left
    };
    
    // Define the 12 triangles that make up the cube (2 triangles per face)
    // Front face
    m.triangles.push_back(triangle(vertices[0], vertices[1], vertices[2]));
    m.triangles.push_back(triangle(vertices[0], vertices[2], vertices[3]));
    
    // Back face
    m.triangles.push_back(triangle(vertices[4], vertices[6], vertices[5]));
    m.triangles.push_back(triangle(vertices[4], vertices[7], vertices[6]));
    
    // Left face
    m.triangles.push_back(triangle(vertices[4], vertices[0], vertices[3]));
    m.triangles.push_back(triangle(vertices[4], vertices[3], vertices[7]));
    
    // Right face
    m.triangles.push_back(triangle(vertices[1], vertices[5], vertices[6]));
    m.triangles.push_back(triangle(vertices[1], vertices[6], vertices[2]));
    
    // Top face
    m.triangles.push_back(triangle(vertices[3], vertices[2], vertices[6]));
    m.triangles.push_back(triangle(vertices[3], vertices[6], vertices[7]));
    
    // Bottom face
    m.triangles.push_back(triangle(vertices[4], vertices[5], vertices[1]));
    m.triangles.push_back(triangle(vertices[4], vertices[1], vertices[0]));
}