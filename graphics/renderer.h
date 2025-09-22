#include <SDL.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include "../include/engine.h"
using namespace std;

namespace Engine3D {

    struct RGB {
        Uint8 r, g, b;
        RGB(Uint8 red, Uint8 green, Uint8 blue) : r(red), g(green), b(blue) {}
        
        // Static color constants
        static const RGB WHITE;
        static const RGB RED;
        static const RGB GREEN;
        static const RGB BLUE;
        static const RGB BLACK;
    };

    class Renderer {
        private:
            SDL_Window* window;
            SDL_Renderer* renderer;
            int screenWidth, screenHeight;
            
            // Helper functions
    
        public:
            Renderer(int width, int height);
            ~Renderer();

            bool init();
            void clear();
            void present();
            void cleanup();

            void drawMesh(const mesh& m, const matrix4x4& projection);
            void drawMesh(const mesh& m, const matrix4x4& projection, const matrix4x4& rotX, const matrix4x4& rotZ);
            void drawTriangle(const triangle& tri, RGB color);
            void fillTriangle(const triangle& tri, RGB color);
            RGB calculateShadedColor(float lightIntensity);
            
            // Struct to hold triangle with its depth for sorting
            struct TriangleDepth {
                triangle tri;
                float depth;
                RGB color;
                
                TriangleDepth(const triangle& t, float d, const RGB& c) 
                    : tri(t), depth(d), color(c) {}
            };
    };
};