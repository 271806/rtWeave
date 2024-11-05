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


## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [TODOs](#todos)
- [Warnings](#warnings)
- [License](#license)
- [About](#about)

---

## Warnings
Use GCC instead of Clang to complie this project.

## Introduction
Weave Renderer is a personal project aimed at building a ray-tracing renderer for educational and experimental purposes. This project allows the exploration and implementation of various rendering techniques, such as Monte Carlo integration, importance sampling, and light scattering. It is ideal for users who wish to learn the fundamentals of ray tracing.

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
$ git clone https://github.com/271806/rtWeave.git
```


NOTE: Use Release Mode for faster experience
```bash
$ cmake -B build/Release -DCMAKE_BUILD_TYPE=Release
$ cmake --build build/Release
$ build/Release/raytracing > output/image.ppm
```


## About

This project was created by Yinfei Qian. (website)
From August 2024.
2229
need to be organized before early Nov.
xx