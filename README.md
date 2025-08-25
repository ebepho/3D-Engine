# 3D Engine

A custom 3D graphics engine built from scratch in C++ using SDL2 for window management and pixel-level rendering.

## Features

- **Custom 3D Math Library**: Complete implementation of 3D vectors, 4x4 transformation matrices, and geometric operations
- **Software Rendering Pipeline**: Pixel-level rendering with depth buffering and wireframe triangle drawing
- **Mesh System**: Triangle-based mesh structure supporting complex 3D objects
- **Real-time Animation**: Smooth rotation and transformation animations with matrix operations
- **Camera System**: Configurable perspective projection and camera positioning
- **Primitive Shape Generation**: Built-in functions to create cubes, pyramids, and other basic shapes
- **Multi-object Rendering**: Simultaneous rendering of multiple 3D objects with individual transformations

## Technical Implementation

- **Language**: C++17
- **Graphics Library**: SDL2 (window management and pixel display only)
- **Architecture**: Modular design with separate geometry, rendering, and application layers
- **Rendering**: Custom software renderer with depth testing and wireframe visualization
- **Math Engine**: Hand-implemented vector operations, matrix transformations, and projection calculations

## Project Structure

```
├── include/
│   ├── engine3d.h          # Main engine header
│   └── geometry.h          # 3D math structures and operations
├── src/
│   ├── main.cpp           # Application entry point and demo scene
│   └── geometry.cpp       # 3D math implementation
├── graphics/
│   ├── window_renderer.h  # Rendering system header
│   └── window_renderer.cpp # SDL2-based rendering implementation
└── Makefile               # Build configuration
```

## Building and Running

### Prerequisites
- C++17 compatible compiler (GCC/Clang)
- SDL2 development libraries

### macOS Installation
```bash
brew install sdl2
```

### Ubuntu/Debian Installation
```bash
sudo apt-get install libsdl2-dev
```

### Build and Run
```bash
make
./engine3d
```

## Demo Scene

The included demo showcases:
- Rotating red cube with dual-axis rotation
- Animated green pyramid with scaling
- Floating blue triangle with sine wave movement
- Animated wireframe lines
- 3D coordinate axes for reference
- Real-time 60 FPS animation loop

## Technical Highlights

- **Zero External 3D Libraries**: All 3D mathematics and rendering logic implemented from scratch
- **Memory Efficient**: Uses vector containers for dynamic mesh storage and pixel buffers
- **Cross-platform Compatible**: Works on macOS, Linux, and Windows with SDL2
- **Educational Focus**: Clean, readable code demonstrating core 3D graphics principles
- **Extensible Design**: Modular architecture allows easy addition of new features

## Future Enhancements

- Triangle filling and solid surface rendering
- Texture mapping and UV coordinates
- Lighting models and shading
- Interactive camera controls
- Model loading from external files
- Anti-aliasing and improved line drawing

## Controls

- **ESC**: Exit application
- Camera controls: *Coming soon*

---

This engine demonstrates fundamental 3D graphics programming concepts including linear algebra, transformation matrices, projection geometry, and software rendering pipelines.