#include "../include/engine3d.h"
#include <iostream>
#include <cmath>

using namespace Engine3D;

// Simple mesh creation functions
mesh createCube() {
    mesh cube;
    
    // Define the 8 vertices of a unit cube
    vec3d v[8] = {
        vec3d(-1, -1, -1), vec3d( 1, -1, -1), vec3d( 1,  1, -1), vec3d(-1,  1, -1), // Back face
        vec3d(-1, -1,  1), vec3d( 1, -1,  1), vec3d( 1,  1,  1), vec3d(-1,  1,  1)  // Front face
    };
    
    // Define 12 triangles (2 per face)
    int indices[36] = {
        0,1,2, 2,3,0,  // Back face
        4,6,5, 6,4,7,  // Front face  
        0,3,7, 7,4,0,  // Left face
        1,5,6, 6,2,1,  // Right face
        0,4,5, 5,1,0,  // Bottom face
        3,2,6, 6,7,3   // Top face
    };
    
    for (int i = 0; i < 36; i += 3) {
        triangle tri;
        tri.p[0] = v[indices[i]];
        tri.p[1] = v[indices[i + 1]];
        tri.p[2] = v[indices[i + 2]];
        tri.computeNormal();
        cube.addTriangle(tri);
    }
    
    return cube;
}

mesh createPyramid() {
    mesh pyramid;
    
    // Define vertices
    vec3d top(0, 1, 0);
    vec3d base[4] = {
        vec3d(-1, -1, -1), vec3d(1, -1, -1),
        vec3d(1, -1, 1),   vec3d(-1, -1, 1)
    };
    
    // Create triangular faces
    for (int i = 0; i < 4; i++) {
        triangle tri;
        tri.p[0] = top;
        tri.p[1] = base[i];
        tri.p[2] = base[(i + 1) % 4];
        tri.computeNormal();
        pyramid.addTriangle(tri);
    }
    
    // Add base (2 triangles)
    triangle base1, base2;
    base1.p[0] = base[0]; base1.p[1] = base[1]; base1.p[2] = base[2];
    base2.p[0] = base[0]; base2.p[1] = base[2]; base2.p[2] = base[3];
    base1.computeNormal(); base2.computeNormal();
    pyramid.addTriangle(base1);
    pyramid.addTriangle(base2);
    
    return pyramid;
}

int main() {
    std::cout << "=== 3D Engine - Window Renderer ===" << std::endl;
    
    // Create window renderer
    WindowRenderer renderer;
    
    // Initialize window
    if (!renderer.initialize(1024, 768, "3D Engine - Real Window!")) {
        std::cerr << "Failed to initialize window renderer!" << std::endl;
        return -1;
    }
    
    // Setup camera
    renderer.setCamera(vec3d(0, 3, -10), vec3d(0, 0, 0), vec3d(0, 1, 0));
    renderer.setCameraPerspective(60.0f * DEG_TO_RAD, 1024.0f / 768.0f, 0.1f, 100.0f);
    
    // Create objects
    mesh cube = createCube();
    mesh pyramid = createPyramid();
    
    std::cout << "Created cube with " << cube.getTriangleCount() << " triangles" << std::endl;
    std::cout << "Created pyramid with " << pyramid.getTriangleCount() << " triangles" << std::endl;
    std::cout << "Controls: ESC to exit, WASD for movement (coming soon)" << std::endl;
    
    // Animation variables
    float angle = 0.0f;
    float time = 0.0f;
    
    // Main loop
    while (!renderer.shouldClose()) {
        renderer.handleEvents();
        renderer.beginFrame();
        
        // Update animation
        angle += 0.02f;
        time += 0.016f; // ~60 FPS
        
        // Draw coordinate axes
        renderer.drawAxis(3.0f);
        
        // Draw rotating cube
        mat4x4 cubeTransform = mat4x4::translation(-3, 0, 0) * 
                              mat4x4::rotationY(angle) * 
                              mat4x4::rotationX(angle * 0.7f);
        renderer.drawMesh(cube, cubeTransform, vec3d(0.8f, 0.2f, 0.2f)); // Red cube
        
        // Draw rotating pyramid
        mat4x4 pyramidTransform = mat4x4::translation(3, 0, 0) * 
                                 mat4x4::rotationY(-angle * 0.8f) * 
                                 mat4x4::scale(0.8f, 0.8f, 0.8f);
        renderer.drawMesh(pyramid, pyramidTransform, vec3d(0.2f, 0.8f, 0.2f)); // Green pyramid
        
        // Draw animated triangle in center
        triangle centerTri;
        centerTri.p[0] = vec3d(0, 1.5f + std::sin(time * 2) * 0.5f, 0);
        centerTri.p[1] = vec3d(-0.8f, -0.5f, 0);
        centerTri.p[2] = vec3d(0.8f, -0.5f, 0);
        centerTri.computeNormal();
        
        mat4x4 triTransform = mat4x4::rotationZ(angle * 2.0f);
        renderer.drawTriangle(triangle(
            triTransform.multiplyVector(centerTri.p[0]),
            triTransform.multiplyVector(centerTri.p[1]),
            triTransform.multiplyVector(centerTri.p[2])
        ), vec3d(0.2f, 0.2f, 0.8f)); // Blue triangle
        
        // Draw some lines for fun
        for (int i = 0; i < 5; i++) {
            float offset = i * 0.5f - 1.0f;
            vec3d start(-5, offset, std::sin(time + i) * 2);
            vec3d end(5, offset, std::cos(time + i) * 2);
            renderer.drawLine(start, end, vec3d(0.5f, 0.5f, 1.0f));
        }
        
        renderer.endFrame();
    }
    
    renderer.shutdown();
    
    std::cout << "\nWindow closed." << std::endl;
    return 0;
}
