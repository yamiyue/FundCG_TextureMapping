
# FundCG_TextureMapping
NTUST Fundamental of Computer Graphics Project 3

## Introduction
The propose of this project is to use the OpenMesh Lib to do texture mapping. The program should provide user functions of picking faces, parameterizing the mesh pactch, texture mapping. The texture uv parametering visualization is necessary. 

## Goal of the game
Control the character to destroy the boxes and collect the apples. 

## Implement

### Platform/Environment
- QT 5.14.1
- MSVC2017
- OpenGL 4.3
- OpenMesh 6.3
- eigen
- Windows10 64bit

### Picking faces

Use `gluUnProject` to select the face we want. Use these faces to create a new mesh.
![pick faces](https://user-images.githubusercontent.com/36958393/131991147-85af3667-f811-4e70-8a62-d660e2f797c2.jpg)

### Mesh parameterizing

Calculating the uv of the new mesh.
![para](https://user-images.githubusercontent.com/36958393/131991167-270961d1-78bd-4ee1-9763-80f77655d185.jpg)

### Shaders

- Texture mapping
  
  Texture sampler
- Lava effect

  three.js https://threejs.org/examples/?q=lava#webgl_shader_lava

## Demo
https://youtu.be/mJkwdZXGfYo
![demo](https://user-images.githubusercontent.com/36958393/131991544-92150454-4f6c-49c6-9eef-fb43487211ef.jpg)
