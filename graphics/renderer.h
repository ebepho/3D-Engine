#include <SDL.h>
#include <vector>
#include <cmath>
#include "../include/engine.h"
using namespace std;

namespace Engine3D {

    struct RGB {
        Uint8 r, g, b;
        RGB(Uint8 red, Uint8 green, Uint8 blue) : r(red), g(green), b(blue) {}
    };

    class Renderer {
        private:
            SDL_Window* window;
            SDL_Renderer* renderer;
            int screenWidth, screenHeight;
            float* depthBuffer;  // Depth buffer for Z-testing
            
            // Helper functions
            void drawTriangleFilled(const triangle& tri, RGB color);
            void fillTriangle(int x1, int y1, float z1, int x2, int y2, float z2, int x3, int y3, float z3, RGB color);
            void setPixel(int x, int y, float z, RGB color);
            vec3d projectToScreen(const vec3d& point);
            RGB getFaceColor(int triangleIndex);
            bool isTriangleVisible(const triangle& tri);
            vec3d calculateNormal(const triangle& tri);
    
        public:
            Renderer(int width, int height);
            ~Renderer();

            bool init();
            void clear();
            void present();
            void cleanup();

            void drawMesh(const mesh& m);
    };
};