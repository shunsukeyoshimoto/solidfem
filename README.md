Oct. 17th, 2014


This program was created by Shunsuke Yoshimoto, Osaka University

Contact: yoshimoto[at]bpe.es.osaka-u.ac.jp



0.Compile

LIB:math,glut,OpenGL

FEM Data PATH: ../data/sphere.fem


*LinuxまたはCygwin:

gcc main.c GLTool.c MathTool.c Mesh.c SolidFEM.c -o test -lglut -lGL -lGLU

./test ../data/sphere.fem



※ 注意

freeglut(./@.c):failed to open display

と出たら，

$ export DISPLAY=:0

$ startxwin

既にXプロセスが起動している場合は一度落とす



1.Command Key

q:quit

r:rotate object

n:select node(left:fixed,middle:input,right:free)

s:calculate inverse stiffness matrix

f:fem simulation mode

i:clear deformation

c:clear node settings

-when main window is active
