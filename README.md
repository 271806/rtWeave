![rtWeave_v0.1.0](teaser/teaser01.png)

# Weave Renderer 0.1.2
A Ray-Tracing Renderer. On-going personal project.
By Yinfei Qian.

Release version: v0.1.1

## TODO
- parallelization (mutilthreads cpu, cuda w/ or w/o Optix, etc.)
- HDR map with importance sampling
- BRDF/BSDF model
- Monte Carlo Intergration (Done)
- Importance Sampling (Done)
- Light Scattering (Done)
- PDF (Done)
- triangle meshes (Done)
- directional light (Done)
- HDR map, skybox (Done)
- PNG output
- adjustable gamma correction
- multiple mesh import

   
  





  


## Warnings
Use GCC instead of Clang to complie this project.

## Quick Start
Install Dependencies:

(placeholder)


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