## Dependencies

C++ 17 or newer
GLFW, GLEW, GLM, Assimp, stb_image, rapidcsv, argparse

## Linux Notes
Add lib files for GLFW and GLEW to external/lib. Link to libGL.so instead of opengl32.lib. You may also need to add following command line arguments: -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl

## Acknowledgments
This work was done during a stay at the Research in Orthopedic Computer Science Group (ROCS) at Balgrist Hospital Zurich. The assets taken from paper "Creating a Digital Twin of Spinal Surgery: A Proof of Concept".