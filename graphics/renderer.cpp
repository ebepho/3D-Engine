#include "renderer.h"
#include <iostream>
#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Engine3D {

    Renderer::Renderer(int width, int height) {
        screenWidth = width;
        screenHeight = height;
        window = nullptr;
        renderer = nullptr;
        depthBuffer = new float[width * height];
    }

    Renderer::~Renderer() {
        cleanup();
        delete[] depthBuffer;
    }

    bool Renderer::init() {
        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        // Create window
        window = SDL_CreateWindow("3D Engine", 
                                SDL_WINDOWPOS_UNDEFINED, 
                                SDL_WINDOWPOS_UNDEFINED,
                                screenWidth, 
                                screenHeight, 
                                SDL_WINDOW_SHOWN);
        if (window == nullptr) {
            std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        // Create renderer
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == nullptr) {
            std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        return true;
    }

    void Renderer::cleanup() {
        if (renderer) {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }
        if (window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
        SDL_Quit();
    }

    void Renderer::clear() {
        // Clear the screen with black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        // Clear depth buffer (initialize to far plane)
        for (int i = 0; i < screenWidth * screenHeight; i++) {
            depthBuffer[i] = 1.0f; // Far plane value
        }
    }

    void Renderer::present() {
        // Present the rendered frame
        SDL_RenderPresent(renderer);
    }

    void Renderer::drawMesh(const mesh& m) {
        // Draw all triangles in the mesh with different colors per face
        int triangleIndex = 0;
        for (const auto& triangle : m.triangles) {
            // Skip triangles facing away from camera (backface culling)
            if (!isTriangleVisible(triangle)) {
                RGB faceColor = getFaceColor(triangleIndex);
                drawTriangleFilled(triangle, faceColor);
            }
            triangleIndex++;
        }
    }

    vec3d Renderer::projectToScreen(const vec3d& point) {
        // Perspective projection parameters
        float fov = 90.0f * M_PI / 180.0f; // 90 degrees in radians
        float aspectRatio = (float)screenWidth / screenHeight;
        float nearPlane = 0.1f;
        float farPlane = 1000.0f;
        
        // Move point away from camera (translate Z)
        float adjustedZ = point.z + 2.0f; // Move back so we can see the object
        
        // Prevent division by zero
        if (adjustedZ <= nearPlane) {
            adjustedZ = nearPlane + 0.01f;
        }
        
        // Perspective projection
        float f = 1.0f / tan(fov / 2.0f);
        
        // Project X and Y
        float projectedX = (point.x * f) / adjustedZ;
        float projectedY = (point.y * f * aspectRatio) / adjustedZ;
        
        // Convert to screen coordinates
        float screenX = (projectedX + 1.0f) * screenWidth * 0.5f;
        float screenY = (1.0f - projectedY) * screenHeight * 0.5f; // Flip Y
        
        // Normalize Z to [0, 1] range for depth buffer
        float normalizedZ = (adjustedZ - nearPlane) / (farPlane - nearPlane);
        normalizedZ = std::max(0.0f, std::min(1.0f, normalizedZ));
        
        return vec3d(screenX, screenY, normalizedZ);
    }

    void Renderer::drawTriangleFilled(const triangle& tri, RGB color) {
        // Project all vertices to screen space
        vec3d projected1 = projectToScreen(tri.points[0]);
        vec3d projected2 = projectToScreen(tri.points[1]);
        vec3d projected3 = projectToScreen(tri.points[2]);
        
        // Convert to integer coordinates
        int x1 = (int)projected1.x;
        int y1 = (int)projected1.y;
        float z1 = projected1.z;
        
        int x2 = (int)projected2.x;
        int y2 = (int)projected2.y;
        float z2 = projected2.z;
        
        int x3 = (int)projected3.x;
        int y3 = (int)projected3.y;
        float z3 = projected3.z;
        
        fillTriangle(x1, y1, z1, x2, y2, z2, x3, y3, z3, color);
    }

    void Renderer::fillTriangle(int x1, int y1, float z1, int x2, int y2, float z2, int x3, int y3, float z3, RGB color) {
        // Sort vertices by Y coordinate, keeping track of Z values
        if (y1 > y2) { 
            std::swap(x1, x2); 
            std::swap(y1, y2); 
            std::swap(z1, z2);
        }
        if (y1 > y3) { 
            std::swap(x1, x3); 
            std::swap(y1, y3); 
            std::swap(z1, z3);
        }
        if (y2 > y3) { 
            std::swap(x2, x3); 
            std::swap(y2, y3); 
            std::swap(z2, z3);
        }

        // Fill the triangle using scanlines with depth interpolation
        for (int y = y1; y <= y3; y++) {
            float xa, xb, za, zb;
            
            if (y <= y2) {
                // Upper part of triangle
                if (y2 - y1 != 0) {
                    float t = (float)(y - y1) / (y2 - y1);
                    xa = x1 + t * (x2 - x1);
                    za = z1 + t * (z2 - z1);
                } else {
                    xa = x1;
                    za = z1;
                }
                if (y3 - y1 != 0) {
                    float t = (float)(y - y1) / (y3 - y1);
                    xb = x1 + t * (x3 - x1);
                    zb = z1 + t * (z3 - z1);
                } else {
                    xb = x1;
                    zb = z1;
                }
            } else {
                // Lower part of triangle
                if (y3 - y2 != 0) {
                    float t = (float)(y - y2) / (y3 - y2);
                    xa = x2 + t * (x3 - x2);
                    za = z2 + t * (z3 - z2);
                } else {
                    xa = x2;
                    za = z2;
                }
                if (y3 - y1 != 0) {
                    float t = (float)(y - y1) / (y3 - y1);
                    xb = x1 + t * (x3 - x1);
                    zb = z1 + t * (z3 - z1);
                } else {
                    xb = x1;
                    zb = z1;
                }
            }
            
            // Ensure xa is left of xb
            if (xa > xb) {
                std::swap(xa, xb);
                std::swap(za, zb);
            }
            
            // Draw horizontal line with depth interpolation
            int startX = (int)xa;
            int endX = (int)xb;
            
            for (int x = startX; x <= endX; x++) {
                float z;
                if (endX - startX != 0) {
                    float t = (float)(x - startX) / (endX - startX);
                    z = za + t * (zb - za);
                } else {
                    z = za;
                }
                
                setPixel(x, y, z, color);
            }
        }
    }

    void Renderer::setPixel(int x, int y, float z, RGB color) {
        // Check bounds
        if (x < 0 || x >= screenWidth || y < 0 || y >= screenHeight) {
            return;
        }
        
        // Depth test
        int index = y * screenWidth + x;
        if (z < depthBuffer[index]) {
            depthBuffer[index] = z;
            
            // Set the pixel color
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }

    bool Renderer::isTriangleVisible(const triangle& tri) {
        // Calculate normal in world space
        vec3d normal = calculateNormal(tri);
        
        // Get the center of the triangle
        vec3d center;
        center.x = (tri.points[0].x + tri.points[1].x + tri.points[2].x) / 3.0f;
        center.y = (tri.points[0].y + tri.points[1].y + tri.points[2].y) / 3.0f;
        center.z = (tri.points[0].z + tri.points[1].z + tri.points[2].z) / 3.0f;
        
        // Camera is at origin looking down -Z axis
        // Vector from triangle center to camera
        vec3d toCamera;
        toCamera.x = 0.0f - center.x;
        toCamera.y = 0.0f - center.y;
        toCamera.z = 0.0f - center.z;
        
        // Dot product with normal - if positive, triangle faces camera
        float dot = normal.x * toCamera.x + normal.y * toCamera.y + normal.z * toCamera.z;
        
        return dot < 0.0f; // Cull if facing away
    }

    RGB Renderer::getFaceColor(int triangleIndex) {
        // Assign colors based on cube faces (2 triangles per face)
        int faceIndex = triangleIndex / 2;
        
        switch (faceIndex) {
            case 0: // Front face - Red
                return RGB(200, 100, 100);
            case 1: // Back face - Blue
                return RGB(100, 100, 200);
            case 2: // Left face - Green
                return RGB(100, 200, 100);
            case 3: // Right face - Yellow
                return RGB(200, 200, 100);
            case 4: // Top face - Magenta
                return RGB(200, 100, 200);
            case 5: // Bottom face - Cyan
                return RGB(100, 200, 200);
            default:
                return RGB(150, 150, 150); // Gray fallback
        }
    }

    vec3d Renderer::calculateNormal(const triangle& tri) {
        // Calculate two edge vectors
        vec3d v1(tri.points[1].x - tri.points[0].x,
                 tri.points[1].y - tri.points[0].y,
                 tri.points[1].z - tri.points[0].z);
        
        vec3d v2(tri.points[2].x - tri.points[0].x,
                 tri.points[2].y - tri.points[0].y,
                 tri.points[2].z - tri.points[0].z);
        
        // Cross product to get normal
        vec3d normal;
        normal.x = v1.y * v2.z - v1.z * v2.y;
        normal.y = v1.z * v2.x - v1.x * v2.z;
        normal.z = v1.x * v2.y - v1.y * v2.x;
        
        // Normalize
        float length = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
        if (length > 0) {
            normal.x /= length;
            normal.y /= length;
            normal.z /= length;
        }
        
        return normal;
    }

};