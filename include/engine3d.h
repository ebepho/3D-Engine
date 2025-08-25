#pragma once

// Core 3D Engine Headers
#include "geometry.h"

// Graphics renderer
#include "../graphics/window_renderer.h"

// Version information
#define ENGINE3D_VERSION_MAJOR 1
#define ENGINE3D_VERSION_MINOR 0
#define ENGINE3D_VERSION_PATCH 0

namespace Engine3D {
    
    // Utility constants
    const float PI = 3.14159265359f;
    const float DEG_TO_RAD = PI / 180.0f;
    const float RAD_TO_DEG = 180.0f / PI;
    
} // namespace Engine3D
