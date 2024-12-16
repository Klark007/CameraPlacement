# CameraPlacement
An OpenGL app for placing calibrated cameras into 3d models.

## Features
- Model loading using Assimp
- MSAA anti-aliasing
- Frustum Culling
- Using camera calibrations for accurate previews

![Render](https://github.com/Klark007/CameraPlacement/blob/dev/media/Render.PNG)
*View from camera flying around scene*

![Render](https://github.com/Klark007/CameraPlacement/blob/dev/media/Preview.PNG)
*Preview from a Kinect placed in the room*

![Render](https://github.com/Klark007/CameraPlacement/blob/dev/media/Frustum.PNG)
*Displaying placed camera's frustum*

## Controls
- WASD for movement, Mouse for Looking around.
- Enter goes into preview mode for in GUI selected camera. While in preview mode Escape leaves Preview mode without adding to the list of placed cameras while pressing Enter adds the camera.
- Pressing space anytime switches to the GUI mode where settings might be changed. Pressing space again leaves the GUI mode.
- E for export

## Camera Placement Video Instructions
[![CameraPlacementInstructions](https://img.youtube.com/vi/Hx9r8s3ZA74/0.jpg)](https://www.youtube.com/watch?v=Hx9r8s3ZA74)

## Commandline arguments
- -cwd: Path to which all paths below are relative to
- -c or --cameras: Path to folder where camera icon, intrinics and optionally transforms (for stereo cameras) are stored
- -m or --models: Path to folder where models to be used are stored
- -o or --output: Path to folder where camera intrinics and extrinics are exported to as csv files

## Adding more camera types
One folder per camera in the path given by -c (by default cameras/). Folder name corresponds to the name of the camera.
- icon.png: Icon of the camera used for gui. Ideally 1024x1024 pixels
- intrinics.csv: Calibrated intrinsics of camera with format: fx, fy, cx, cy. The app assumes that the resolution of the camera is (2 x c_x, 2 x c_y)

## Installation
Build either using Visual Studio directly

## Dependencies

C++ 17 or newer
GLFW, GLEW, GLM, Assimp, stb_image, rapidcsv, argparse, opencv

## Linux Notes
Add lib files for GLFW, GLEW, Assimp, OpenCV to external/lib. Link to libGL.so instead of opengl32.lib. You may also need to add following command line arguments: -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl

## Acknowledgments
The app was created during a stay at the Research in Orthopedic Computer Science Group (ROCS) at Balgrist Hospital Zurich. The assets are taken from paper "Creating a Digital Twin of Spinal Surgery: A Proof of Concept".
This work has been supported by the OR-X - a swiss national research infrastructure for translational surgery - and associated funding by the University of Zurich and University Hospital Balgrist.
