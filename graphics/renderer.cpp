#include "renderer.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <SDL2_gfxPrimitives.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Engine3D {

    Renderer::Renderer(int width, int height) {
        screenWidth = width;
        screenHeight = height;
        window = nullptr;
        renderer = nullptr;
    }

    Renderer::~Renderer() {
        cleanup();
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
        
    }

    void Renderer::present() {
        // Present the rendered frame
        SDL_RenderPresent(renderer);
    }

    void Renderer::drawMesh(const mesh& m, const matrix4x4& projection) {
        
        for (const auto& tri : m.triangles) {
            triangle triTranslated;
            triTranslated.points[0] = tri.points[0];
            triTranslated.points[1] = tri.points[1];
            triTranslated.points[2] = tri.points[2];

            // move cube away from camera
            triTranslated.points[0].z += 3.0f;
            triTranslated.points[1].z += 3.0f;
            triTranslated.points[2].z += 3.0f;
            
            triangle proj;
            proj.points[0] = projection.multiplyVector(triTranslated.points[0]);
            proj.points[1] = projection.multiplyVector(triTranslated.points[1]);
            proj.points[2] = projection.multiplyVector(triTranslated.points[2]);

            vec3d pos(1.0f, 1.0f, 0.0f);
            proj = proj + pos;

            vec3d scale((float)screenWidth / 2.0f, (float)screenHeight / 2.0f, 1.0f);
            proj = proj * scale;

            drawTriangle(proj, RGB(255, 255, 255));
        }
    }

    void Renderer::drawMesh(const mesh& m, const matrix4x4& projection, const matrix4x4& rotX, const matrix4x4& rotZ, vec3d cameraPos) {
        std::vector<TriangleDepth> trianglesToRaster;
        
        for (const auto& tri : m.triangles) {
            // Apply rotation first
            triangle triRotated = rotateTriangle(tri, rotX, rotZ);

            triangle triTranslated;
            triTranslated.points[0] = triRotated.points[0];
            triTranslated.points[1] = triRotated.points[1];
            triTranslated.points[2] = triRotated.points[2];

            // move cube away from camera
            triTranslated.points[0].z += 3.0f;
            triTranslated.points[1].z += 3.0f;
            triTranslated.points[2].z += 3.0f;

            triTranslated.calculateNormal();

            // Skip back-facing triangles
            if (!triTranslated.isFacingCamera(cameraPos)) {
                continue;
            }

            // Calculate lighting
            vec3d lightDirection(0.0f, 0.0f, -1.0f); // Light coming from front
            float lightIntensity = calculateLighting(triTranslated.normal, lightDirection);

            // Project triangle from 3D to 2D
            triangle proj;
            proj.points[0] = projection.multiplyVector(triTranslated.points[0]);
            proj.points[1] = projection.multiplyVector(triTranslated.points[1]);
            proj.points[2] = projection.multiplyVector(triTranslated.points[2]);

            vec3d pos(1.0f, 1.0f, 0.0f);
            proj = proj + pos;

            vec3d scale((float)screenWidth / 2.0f, (float)screenHeight / 2.0f, 1.0f);
            proj = proj * scale;

            // Calculate average depth for sorting
            float avgDepth = (proj.points[0].z + proj.points[1].z + proj.points[2].z) / 3.0f;
            
            // Use lighting to determine color
            RGB shadedColor = calculateShadedColor(lightIntensity);
            
            // Store triangle for sorting
            // trianglesToRaster.push_back(TriangleDepth(proj, avgDepth, shadedColor));

            // drawTriangle(proj, RGB(255, 255, 255));
            fillTriangle(proj, shadedColor);
        }

        // Sort triangles from back to front (higher Z first)
        // sort(trianglesToRaster.begin(), trianglesToRaster.end(), 
        //      [](const TriangleDepth& a, const TriangleDepth& b) {
        //          return a.depth > b.depth;
        //      });
        
        // Now render sorted triangles
        // for (const auto& triDepth : trianglesToRaster) {
        //     fillTriangle(triDepth.tri, triDepth.color);
        // }
    }

    void Renderer::drawTriangle(const triangle& tri, RGB color) {
        // Set the drawing color
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
        
        // Draw the three edges of the triangle as lines
        SDL_RenderDrawLine(renderer, 
                          (int)tri.points[0].x, (int)tri.points[0].y,
                          (int)tri.points[1].x, (int)tri.points[1].y);
        
        SDL_RenderDrawLine(renderer, 
                          (int)tri.points[1].x, (int)tri.points[1].y,
                          (int)tri.points[2].x, (int)tri.points[2].y);
        
        SDL_RenderDrawLine(renderer, 
                          (int)tri.points[2].x, (int)tri.points[2].y,
                          (int)tri.points[0].x, (int)tri.points[0].y);
    }

    void Renderer::fillTriangle(const triangle& tri, RGB color) {
        Sint16 vx[3] = { (Sint16)tri.points[0].x, (Sint16)tri.points[1].x, (Sint16)tri.points[2].x };
        Sint16 vy[3] = { (Sint16)tri.points[0].y, (Sint16)tri.points[1].y, (Sint16)tri.points[2].y };
        
        // Set the drawing color
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
        
        // Fill the triangle
        filledPolygonRGBA(renderer, vx, vy, 3, color.r, color.g, color.b, 255);
    }


    RGB Renderer::calculateShadedColor(float lightIntensity) {
        // Convert light intensity to grayscale color
        Uint8 intensity = (Uint8)(lightIntensity * 255.0f);
        return RGB(intensity, intensity, intensity);
    }
};