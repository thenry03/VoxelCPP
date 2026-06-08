# VoxelCPP

> Learning C++ and OpenGL by building a Minecraft-inspired voxel engine from scratch.

![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)
![Language: C++17](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)
![OpenGL 3.3](https://img.shields.io/badge/OpenGL-3.3_Core_Profile-red.svg)
![Version](https://img.shields.io/badge/Version-v0.2.0-orange.svg)
![Status](https://img.shields.io/badge/Status-Phase_2_Complete-brightgreen.svg)

> [Versión en español](README.es.md)

---

![Terrain overview](docs/screenshots/terrain_01.png)

*Screenshot to be updated after Phase 3.*

---

## About

VoxelCPP is a prototype that I want to turn into a fully usable voxel engine. For now, it is just a simple hobby project where I play around with procedural terrain generation.

I wanted to get to know the basics of C++ and expand what I learned in my university's Computer Graphics course, which was not enough to satisfy the curiosity that the subject aroused in me.

It was supposed to be simple, but I didn't realize how many components go into programming your very own game engine. I'm having a lot of fun building it though.

This is the first iteration of several planned, and it is still in development. Next iterations will include concepts such as Deferred Rendering and Ambient Occlusion lighting.

---

## Features

The engine includes the following features:

- **Graphics Pipeline:** Complete implementation utilizing the OpenGL 3.3 Core Profile API.
- **Camera System:** Free-look FPS camera with cursor capture support.
- **Terrain:** Procedural generation powered by coherent noise (FBm + OpenSimplex2).
- **Chunk Management:** SoA (Structure of Arrays) data layout with strict boundary-checking.
- **Optimization:** Face culling between adjacent chunks.
- **Texturing:** Texture Atlas with UV coordinate calculations per block type.
- **Scene:** 14 distinct block types with extensible ID-based registry.
- **Memory Architecture:** Strict RAII memory management, leveraging clean encapsulation and strict separation of responsibilities.
- **Event Architecture:** Event-driven window system featuring a custom EventDispatcher class and Observer pattern implementation.

---

## Tech Stack

The project is built using the following technologies:

- **Language:** C++17
- **Graphics API:** OpenGL 3.3 (Core Profile)
- **Windowing & Input:** GLFW
- **Mathematics:** GLM (OpenGL Mathematics)
- **OpenGL Loading:** GLAD
- **Texture Loading:** stb\_image
- **Noise Generation:** FastNoiseLite
- **Build System:** CMake 3.20+

---

## Building

To compile, paste the following commands into your Linux terminal.

**Clone project and submodules**

```bash
git clone --recurse-submodules https://github.com/thenry03/VoxelCPP.git
cd VoxelCPP
```

**Install system dependencies (Ubuntu/Debian-based systems)**

```bash
sudo apt install build-essential cmake libgl1-mesa-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

**Compile**

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
cd ..
```

**Run — always from the project root**

```bash
./build/VoxelCPP
```

The binary must be executed from the project root. Running it from inside `build/` will cause shader loading to fail due to relative paths.

---

## Architecture

The project is built upon a layered architecture that enforces a strict separation of responsibilities between the world logic and the rendering pipeline.

For a comprehensive overview of the engine's design decisions and structural layout, refer to the [Architecture Documentation](docs/architecture.pdf).

---

## Roadmap

The following table outlines the development phases, current progress, and core milestones of the engine:

| Phase | Status | Description |
|-------|--------|-------------|
| Phase 0 — Environment Setup | ✅ | GLFW + GLAD setup, build system configuration and window initialization. |
| Phase 1 — Base Pipeline | ✅ | Windowing, shader compilation, camera, input, and delta timer. |
| Phase 2 — Voxel Engine Core | ✅ | Chunk system, mesher with culling, procedural world generation, Texture Atlas. |
| Phase 3 — Core Gameplay Mechanics | 🚧 | Chunk height extension, dynamic chunk loading/unloading, infinite world generation, basic environment: mountains and trees. |
| Phase 4 — Lighting and Environment | ⬜ | Flood-fill lighting, fog and render distance, water, biomes: Forest, Beaches, Desert, Mountains, Granite Mountains. |
| Phase 5 — Optimization | ⬜ | Bit-packing, Palette Compression, multi-threading. |

---

## Credits

**Assets**

- [Kenney](https://kenney.nl) — High-quality Voxel Texture Pack.
- [Auburn](https://github.com/Auburn/FastNoiseLite) — Coherent noise generation library (FastNoiseLite).

**Core Utilities**

- [Sean Barrett](https://github.com/nothings/stb) — Single-header image loading library (stb\_image.h).

**Graphics & Windowing**

- [GLFW](https://www.glfw.org) — Multi-platform windowing and input library.
- [GLAD](https://glad.dav1d.de) — OpenGL function loader generator.
- [GLM](https://glm.g-truc.net) — Mathematics library for graphics software based on GLSL specifications.

---

## License

This project is licensed under the [MIT License](LICENSE).
