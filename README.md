# MinecraftClone

## Project Description
MinecraftClone is a voxel-based sandbox game inspired by Minecraft, written in modern C++ and using OpenGL for graphics. It features a procedurally generated block world, chunk management, player controls, and basic world streaming—aimed at providing an open-source architecture for Minecraft-like games.

## Features

- **Block & Chunk System:**  
  World is partitioned into chunks, each containing a 3D grid of blocks (air, dirt, grass, stone, etc.), with efficient mesh generation and manipulation.

- **Procedural Terrain Generation:**  
  Terrain and landscape are created using Perlin noise for natural features and smooth elevation changes.

- **Player Mechanics:**  
  Includes first-person camera, movement, gravity, jumping, collision detection, and block interaction (place/break).

- **Rendering Framework:**  
  Uses OpenGL and GLM for fast 3D rendering, including custom shaders, lighting, and instanced drawing.

- **World Management:**  
  Handles chunk loading/unloading, world saving/loading, and structure placement (trees, etc.).

## Getting Started

### Prerequisites
- C++17 or later compiler
- CMake (version 3.10+)
- OpenGL
- GLM (OpenGL Math Library)
- GLFW (for window/context/input)
- stb_image (for textures, optional)

### Build Instructions

1. **Clone the Repository**
   ```bash
   git clone https://github.com/bouDeScotch/MinecraftClone.git
   cd MinecraftClone
   ```

2. **Configure & Build**
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

3. **Run the Game**
   ```bash
   ./MinecraftClone
   ```

### Notes
- All source code is in the `src/` and `include/` directories.
- You may need to install dependencies via your OS package manager or download them to a `libs/` directory.
- For Windows, use Visual Studio's CMake integration or MinGW.

## Controls

- **W/A/S/D:** Move forward/left/back/right
- **Space:** Jump
- **Mouse:** Look around
- **Left Click:** Break block
- **Right Click:** Place block
- **Esc:** Quit game

## Project Structure

```
MinecraftClone/
├── include/           # Header files (Camera.h, Chunk.h, Player.h, etc.)
├── src/               # Source files
├── shaders/           # GLSL shaders
├── assets/            # Textures, models
├── CMakeLists.txt     # Build configuration
└── README.md          # This file
```

## Contribution

Pull requests and suggestions are welcome!  
Please provide clear descriptions and follow C++ best practices.

## Credits

- Inspired by [Minecraft](https://www.minecraft.net)
- Uses [GLM](https://github.com/g-truc/glm), [GLFW](https://github.com/glfw/glfw), [stb_image](https://github.com/nothings/stb)
- Thanks to all contributors

## License

This project is licensed under the MIT License.
