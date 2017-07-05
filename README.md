FEM analysis: Linear Elastic Object 
====

Overview

This porject is for learning how to implement Finite Element Method (FEM) for deformation simulation in C language. Let's fill in the code at TODO.

## Text
[See here](https://github.com/yoshimoto56/solidfem/blob/master/text/FEM_text.pdf)

## Demo
[![Solid FEM](https://img.youtube.com/vi/ESifso8Z9a8/0.jpg)](https://www.youtube.com/watch?v=ESifso8Z9a8 "Solid FEM")

Command Key - when main window is active

    q:quit

    r:rotate object

    n:select node(left:fixed,middle:input,right:free)

    s:calculate inverse stiffness matrix

    f:fem simulation mode

    i:clear deformation

    c:clear node settings

## Requirement

- glut,OpenGL
- FEM Data : ../data/sphere.fem

## Compile command

    $ gcc main.c GLTool.c MathTool.c Mesh.c SolidFEM.c -o test -lglut -lGL -lGLU

    $ ./test ../data/sphere.fem

## Licence

[MIT](https://github.com/yoshimoto56/solidfem/blob/master/LICENSE)

## Author

[yoshimoto56](https://github.com/yoshimoto56)
