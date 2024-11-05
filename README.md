![rtWeave_v0.1.0](teaser/teaser01.png)

# Weave Renderer 0.1.2
A personal "toy" ray-tracing renderer developed as an ongoing project by Yinfei Qian.


Reference:
- [rt-in-one-weekend](https://raytracing.github.io/) books 1-3
- [PBRT](https://www.pbrt.org/)(book)

---
| Linux             | 
|       :---:       |
|[![Build Status][1]][2]|

[1]: https://github.com/271806/rtWeave/actions/workflows/c-cpp.yml/badge.svg
[2]: https://github.com/271806/rtWeave/actions/runs/11682655845

---

## Notes
Use GCC instead of Clang to build this project. (recommend currently)


## Introduction
### Why "Weave" ?
**Weave Renderer** - Inspired by 'The Weave' from the D&D 5e universe, a mystical network of energy that permeates the world and grants the power of magic. Just as the Weave binds and shapes magical energy, this project aims to weave light and shadow together through ray-tracing techniques to create stunning visuals. Weave Renderer is both a tribute to this beloved fantasy world and a personal journey into the beauty of computer graphics.


Weave Renderer is a personal project aimed at building a ray-tracing renderer for educational and experimental purposes.


## Features
- **Monte Carlo Integration** ✔️
- **Importance Sampling** ✔️
- **Light Scattering** ✔️
- **PDF Calculation** ✔️
- **Triangle Meshes** ✔️
- **Directional Light** ✔️
- **HDR Map and Skybox** ✔️
- **PNG Output** (planned)
- **Adjustable Gamma Correction** (planned)
- **Multiple Mesh Import** (planned)
- **Accerlation via Optix, cuda** (planned)


## Quick Start

```bash
$ git clone --recursive https://github.com/271806/rtWeave.git
```


NOTE: Use Release Mode for faster experience
```bash
$ cmake -B build/Release -DCMAKE_BUILD_TYPE=Release
$ cmake --build build/Release
$ build/Release/raytracing > output/image.ppm
```


## Bibtex

```bibtex
@software{Weave,
    title = {Weave Renderer},
    author = {Yinfei Qian},
    note = {https://github.com/271806/rtWeave.git},
    version = {0.2.1},
    year = 2024
}
```