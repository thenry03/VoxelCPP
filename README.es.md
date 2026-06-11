# VoxelCPP

> Aprendiendo C++ y OpenGL construyendo un motor de vóxeles inspirado en Minecraft desde cero.

![Licencia: MIT](https://img.shields.io/badge/Licencia-MIT-green.svg)
![Lenguaje: C++17](https://img.shields.io/badge/Lenguaje-C%2B%2B17-blue.svg)
![OpenGL 3.3](https://img.shields.io/badge/OpenGL-3.3_Core_Profile-red.svg)
![Versión](https://img.shields.io/badge/Versión-v0.3.0-orange.svg)
![Estado](https://img.shields.io/badge/Estado-Fase_3_Completada-brightgreen.svg)

> [English version](README.md)

---

<img width="2560" height="1600" alt="release_screenshot_3" src="https://github.com/user-attachments/assets/facb2c4c-1e41-4030-94a4-5e7c3958ed03" />

---

## Sobre el proyecto

VoxelCPP es un prototipo que quiero convertir en un motor de vóxeles completamente funcional. Por ahora, es simplemente un proyecto personal donde experimento con la generación procedural de terreno.

Quería aprender los fundamentos de C++ y ampliar lo aprendido en el curso de Computación Gráfica de mi universidad, que no fue suficiente para satisfacer la curiosidad que me despertó la asignatura.

Se suponía que iba a ser sencillo, pero no me di cuenta de cuántos componentes hay que programar para tener tu propio motor de videojuego. Lo estoy pasando muy bien construyéndolo, eso sí.

Esta es la primera iteración de varias planificadas, y sigue en desarrollo. Las próximas iteraciones incluirán conceptos como Deferred Rendering e iluminación por Ambient Occlusion.

---

## Características

El motor incluye las siguientes características:

- **Pipeline gráfico:** Implementación completa utilizando la API OpenGL 3.3 Core Profile.
- **Sistema de cámara:** Cámara FPS con soporte de captura de cursor.
- **Mundo infinito:** Carga y descarga dinámica de chunks basada en la posición del jugador y una distancia de renderizado configurable.
- **Generación de terreno:** Generación procedural con domain warping, mapa de continentalidad, llanuras, montañas, playas, ríos y nivel del mar.
- **Flora:** Generación determinista de árboles a través de los límites de chunk mediante un sistema de hash por celdas.
- **Pipeline multihilo:** La generación de chunks y la construcción de mallas se ejecutan en hilos de trabajo dedicados, dejando el hilo principal libre para el renderizado.
- **Gestión de chunks:** Layout de datos SoA (Structure of Arrays) con comprobación estricta de límites.
- **Optimización:** Face culling entre chunks adyacentes.
- **Sombreado por cara:** Iluminación por normal de cara para dar profundidad visual sin un sistema de iluminación completo.
- **Sol visual:** Quad billboard que siempre mira hacia la cámara, renderizado en una posición fija del mundo.
- **Texturizado:** Atlas de texturas con cálculo de coordenadas UV por tipo de bloque.
- **Escena:** 14 tipos de bloque distintos con registro extensible basado en IDs.
- **Arquitectura de memoria:** Gestión de memoria RAII estricta, con encapsulación limpia y separación estricta de responsabilidades.
- **Arquitectura de eventos:** Sistema de ventana orientado a eventos con clase EventDispatcher personalizada e implementación del patrón Observer.

---

## Tecnologías

El proyecto está construido con las siguientes tecnologías:

- **Lenguaje:** C++17
- **API gráfica:** OpenGL 3.3 (Core Profile)
- **Ventana e input:** GLFW
- **Matemáticas:** GLM (OpenGL Mathematics)
- **Carga de OpenGL:** GLAD
- **Carga de texturas:** stb\_image
- **Generación de ruido:** FastNoiseLite
- **Sistema de build:** CMake 3.20+

---

## Compilación

Para compilar, pega los siguientes comandos en tu terminal de Linux.

**Clonar el proyecto y los submódulos**

```bash
git clone --recurse-submodules https://github.com/thenry03/VoxelCPP.git
cd VoxelCPP
```

**Instalar dependencias del sistema (sistemas basados en Ubuntu/Debian)**

```bash
sudo apt install build-essential cmake libgl1-mesa-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

**Compilar**

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
cd ..
```

**Ejecutar — siempre desde la raíz del proyecto**

```bash
./build/VoxelCPP
```

El binario debe ejecutarse desde la raíz del proyecto. Ejecutarlo desde dentro de `build/` provocará un fallo en la carga de los shaders debido a las rutas relativas.

---

## Arquitectura

El proyecto está construido sobre una arquitectura en capas que impone una separación estricta de responsabilidades entre la lógica del mundo y el pipeline de renderizado.

Para una visión completa de las decisiones de diseño y la estructura del motor, consulta la [Documentación de Arquitectura](docs/architecture.pdf).

---

## Roadmap

La siguiente tabla resume las fases de desarrollo, el progreso actual y los hitos principales del motor:

| Fase | Estado | Descripción |
|------|--------|-------------|
| Fase 0 — Configuración del entorno | ✅ | Configuración de GLFW + GLAD, sistema de build e inicialización de ventana. |
| Fase 1 — Pipeline base | ✅ | Ventana, compilación de shaders, cámara, input y delta timer. |
| Fase 2 — Núcleo del motor de vóxeles | ✅ | Sistema de chunks, mesher con culling, generación procedural de mundo, atlas de texturas. |
| Fase 3 — Mecánicas de juego principales | ✅ | Mundo infinito, generación procedural avanzada con domain warping, ríos y playas, árboles deterministas, sombreado por cara, sol visual, pipeline multihilo. |
| Fase 4 — Iluminación y entorno | ⬜ | Iluminación flood-fill, niebla y distancia de renderizado, agua, biomas: Bosque, Playas, Desierto, Montañas, Montañas de granito. |
| Fase 5 — Optimización | ⬜ | Empaquetado de bits, compresión por paleta, renderizado diferido. |

---

## Créditos

**Assets**

- [Kenney](https://kenney.nl) — Voxel Texture Pack de alta calidad.
- [Auburn](https://github.com/Auburn/FastNoiseLite) — Librería de generación de ruido coherente (FastNoiseLite).

**Utilidades**

- [Sean Barrett](https://github.com/nothings/stb) — Librería de carga de imágenes en cabecera única (stb\_image.h).

**Gráficos y ventana**

- [GLFW](https://www.glfw.org) — Librería multiplataforma de ventana e input.
- [GLAD](https://glad.dav1d.de) — Generador de cargador de funciones OpenGL.
- [GLM](https://glm.g-truc.net) — Librería matemática para software gráfico basada en las especificaciones de GLSL.

---

## Estado de la documentación

El documento de arquitectura (`docs/architecture.pdf`) cubre el diseño de la Fase 2. Se actualizará para reflejar los sistemas de la Fase 3 — pipeline multihilo, mundo infinito, TerrainShaper y flora — en una revisión futura.

---

## Licencia

Este proyecto está licenciado bajo la [Licencia MIT](LICENSE).
