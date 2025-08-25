#pragma once

#include "../include/geometry.h"
#include <SDL2/SDL.h>
#include <vector>

namespace Engine3D {

    class WindowRenderer {
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* frameTexture;
        
        int screenWidth, screenHeight;
        std::vector<uint32_t> pixelBuffer;
        std::vector<float> depthBuffer;
        
        // Camera properties
        vec3d cameraPos, cameraTarget, cameraUp;
        mat4x4 viewMatrix, projectionMatrix;
        float fov, nearPlane, farPlane;
        
        // Helper methods
        void clearBuffers(uint32_t clearColor = 0xFF1A1A2E);
        void setPixel(int x, int y, uint32_t color, float depth = 0.0f);
        uint32_t colorFromRGB(int r, int g, int b);
        uint32_t colorFromVec3d(const vec3d& color);
        vec3d projectToScreen(const vec3d& point);
        void updateMatrices();
        
        // Line drawing
        void drawLineOnBuffer(const vec3d& start, const vec3d& end, uint32_t color);
        
    public:
        WindowRenderer();
        ~WindowRenderer();
        
        // Initialization and cleanup
        bool initialize(int width, int height, const char* title);
        void shutdown();
        
        // Frame management
        void beginFrame();
        void endFrame();
        bool shouldClose();
        void handleEvents();
        
        // Rendering functions
        void drawTriangle(const triangle& tri, const vec3d& color = vec3d(1, 1, 1));
        void drawMesh(const mesh& m, const mat4x4& transform = mat4x4::identity(), 
                     const vec3d& color = vec3d(1, 1, 1));
        void drawLine(const vec3d& start, const vec3d& end, const vec3d& color = vec3d(1, 1, 1));
        void drawAxis(float scale = 2.0f);
        
        // Camera control
        void setCamera(const vec3d& position, const vec3d& target, const vec3d& up = vec3d(0, 1, 0));
        void setCameraPerspective(float fovRadians, float aspect, float near, float far);
        
        // Getters
        int getWidth() const { return screenWidth; }
        int getHeight() const { return screenHeight; }
        vec3d getCameraPosition() const { return cameraPos; }
        vec3d getCameraTarget() const { return cameraTarget; }
    };

} // namespace Engine3D
