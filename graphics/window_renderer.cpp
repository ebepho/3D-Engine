#include "window_renderer.h"
#include <iostream>
#include <cmath>
#include <algorithm>

namespace Engine3D {

    WindowRenderer::WindowRenderer() 
        : window(nullptr), renderer(nullptr), frameTexture(nullptr)
        , screenWidth(800), screenHeight(600)
        , cameraPos(0, 0, -5), cameraTarget(0, 0, 0), cameraUp(0, 1, 0)
        , fov(60.0f * 3.14159f / 180.0f), nearPlane(0.1f), farPlane(100.0f) {
    }

    WindowRenderer::~WindowRenderer() {
        shutdown();
    }

    bool WindowRenderer::initialize(int width, int height, const char* title) {
        screenWidth = width;
        screenHeight = height;
        
        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
            return false;
        }
        
        // Create window
        window = SDL_CreateWindow(
            title,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            screenWidth,
            screenHeight,
            SDL_WINDOW_SHOWN
        );
        
        if (!window) {
            std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return false;
        }
        
        // Create renderer
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }
        
        // Create texture for pixel buffer
        frameTexture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            screenWidth,
            screenHeight
        );
        
        if (!frameTexture) {
            std::cerr << "Texture creation failed: " << SDL_GetError() << std::endl;
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }
        
        // Initialize buffers
        pixelBuffer.resize(screenWidth * screenHeight, 0xFF1A1A2E);
        depthBuffer.resize(screenWidth * screenHeight, 1000.0f);
        
        updateMatrices();
        
        std::cout << "Window renderer initialized: " << screenWidth << "x" << screenHeight << std::endl;
        return true;
    }

    void WindowRenderer::shutdown() {
        if (frameTexture) {
            SDL_DestroyTexture(frameTexture);
            frameTexture = nullptr;
        }
        
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

    void WindowRenderer::beginFrame() {
        clearBuffers();
    }

    void WindowRenderer::endFrame() {
        // Update texture with pixel buffer
        void* pixels;
        int pitch;
        
        SDL_LockTexture(frameTexture, nullptr, &pixels, &pitch);
        memcpy(pixels, pixelBuffer.data(), screenWidth * screenHeight * sizeof(uint32_t));
        SDL_UnlockTexture(frameTexture);
        
        // Render texture to screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, frameTexture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    bool WindowRenderer::shouldClose() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return true;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                return true;
            }
        }
        return false;
    }

    void WindowRenderer::handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_w:
                            // Move forward (you can implement camera movement here)
                            break;
                        case SDLK_s:
                            // Move backward
                            break;
                        case SDLK_a:
                            // Move left
                            break;
                        case SDLK_d:
                            // Move right
                            break;
                    }
                    break;
            }
        }
    }

    void WindowRenderer::drawTriangle(const triangle& tri, const vec3d& color) {
        // Draw triangle as wireframe
        uint32_t col = colorFromVec3d(color);
        drawLine(tri.p[0], tri.p[1], color);
        drawLine(tri.p[1], tri.p[2], color);
        drawLine(tri.p[2], tri.p[0], color);
    }

    void WindowRenderer::drawMesh(const mesh& m, const mat4x4& transform, const vec3d& color) {
        for (const auto& tri : m.tris) {
            // Transform triangle vertices
            triangle transformedTri;
            transformedTri.p[0] = transform.multiplyVector(tri.p[0]);
            transformedTri.p[1] = transform.multiplyVector(tri.p[1]);
            transformedTri.p[2] = transform.multiplyVector(tri.p[2]);
            transformedTri.computeNormal();
            
            drawTriangle(transformedTri, color);
        }
    }

    void WindowRenderer::drawLine(const vec3d& start, const vec3d& end, const vec3d& color) {
        vec3d screenStart = projectToScreen(start);
        vec3d screenEnd = projectToScreen(end);
        
        uint32_t col = colorFromVec3d(color);
        drawLineOnBuffer(screenStart, screenEnd, col);
    }

    void WindowRenderer::drawAxis(float scale) {
        // X-axis (red)
        drawLine(vec3d(0, 0, 0), vec3d(scale, 0, 0), vec3d(1.0f, 0.2f, 0.2f));
        // Y-axis (green)  
        drawLine(vec3d(0, 0, 0), vec3d(0, scale, 0), vec3d(0.2f, 1.0f, 0.2f));
        // Z-axis (blue)
        drawLine(vec3d(0, 0, 0), vec3d(0, 0, scale), vec3d(0.2f, 0.2f, 1.0f));
    }

    void WindowRenderer::setCamera(const vec3d& position, const vec3d& target, const vec3d& up) {
        cameraPos = position;
        cameraTarget = target;
        cameraUp = up;
        updateMatrices();
    }

    void WindowRenderer::setCameraPerspective(float fovRadians, float aspect, float near, float far) {
        fov = fovRadians;
        nearPlane = near;
        farPlane = far;
        updateMatrices();
    }

    // Private helper methods
    void WindowRenderer::clearBuffers(uint32_t clearColor) {
        std::fill(pixelBuffer.begin(), pixelBuffer.end(), clearColor);
        std::fill(depthBuffer.begin(), depthBuffer.end(), 1000.0f);
    }

    void WindowRenderer::setPixel(int x, int y, uint32_t color, float depth) {
        if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight) {
            int index = y * screenWidth + x;
            if (depth < depthBuffer[index]) {
                pixelBuffer[index] = color;
                depthBuffer[index] = depth;
            }
        }
    }

    uint32_t WindowRenderer::colorFromRGB(int r, int g, int b) {
        return 0xFF000000 | (r << 16) | (g << 8) | b;
    }

    uint32_t WindowRenderer::colorFromVec3d(const vec3d& color) {
        int r = static_cast<int>(std::clamp(color.x, 0.0f, 1.0f) * 255);
        int g = static_cast<int>(std::clamp(color.y, 0.0f, 1.0f) * 255);
        int b = static_cast<int>(std::clamp(color.z, 0.0f, 1.0f) * 255);
        return colorFromRGB(r, g, b);
    }

    vec3d WindowRenderer::projectToScreen(const vec3d& point) {
        // Apply view and projection transformations
        vec3d viewPoint = viewMatrix.multiplyVector(point);
        vec3d projPoint = projectionMatrix.multiplyVector(viewPoint);
        
        // Perspective divide (if w != 1)
        if (projPoint.z != 0.0f) {
            // Note: In our simplified matrix, z stores the w component for perspective divide
        }
        
        // Convert to screen coordinates
        float x = (projPoint.x + 1.0f) * 0.5f * screenWidth;
        float y = (1.0f - projPoint.y) * 0.5f * screenHeight;
        
        return vec3d(x, y, viewPoint.z); // Return view space Z for depth testing
    }

    void WindowRenderer::updateMatrices() {
        // Create view matrix (look-at)
        vec3d zaxis = (cameraPos - cameraTarget).normalize(); // Camera looks down negative Z
        vec3d xaxis = cameraUp.cross(zaxis).normalize();
        vec3d yaxis = zaxis.cross(xaxis);

        viewMatrix = mat4x4::identity();
        viewMatrix.m[0][0] = xaxis.x; viewMatrix.m[0][1] = yaxis.x; viewMatrix.m[0][2] = zaxis.x;
        viewMatrix.m[1][0] = xaxis.y; viewMatrix.m[1][1] = yaxis.y; viewMatrix.m[1][2] = zaxis.y;
        viewMatrix.m[2][0] = xaxis.z; viewMatrix.m[2][1] = yaxis.z; viewMatrix.m[2][2] = zaxis.z;
        viewMatrix.m[3][0] = -xaxis.dot(cameraPos);
        viewMatrix.m[3][1] = -yaxis.dot(cameraPos);
        viewMatrix.m[3][2] = -zaxis.dot(cameraPos);
        
        // Create projection matrix
        float aspect = static_cast<float>(screenWidth) / screenHeight;
        projectionMatrix = mat4x4::perspective(fov, aspect, nearPlane, farPlane);
    }

    void WindowRenderer::drawLineOnBuffer(const vec3d& start, const vec3d& end, uint32_t color) {
        // Bresenham's line algorithm
        int x0 = static_cast<int>(start.x);
        int y0 = static_cast<int>(start.y);
        int x1 = static_cast<int>(end.x);
        int y1 = static_cast<int>(end.y);
        
        int dx = abs(x1 - x0);
        int dy = abs(y1 - y0);
        int sx = (x0 < x1) ? 1 : -1;
        int sy = (y0 < y1) ? 1 : -1;
        int err = dx - dy;
        
        float depth = (start.z + end.z) * 0.5f;
        
        while (true) {
            setPixel(x0, y0, color, depth);
            
            if (x0 == x1 && y0 == y1) break;
            
            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y0 += sy;
            }
        }
    }

} // namespace Engine3D
