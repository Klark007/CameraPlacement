# CMakeList.txt : CMake project for CameraPlacement, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# set standard version
add_compile_options(/Zc:__cplusplus)


project ("CameraPlacement")


# Add source to this project's executable.
set(SOURCES src/CameraPlacement.cpp src/CameraPlacement.h "src/engine/Model.cpp" "src/engine/Model.h" 
    "src/engine/Program.cpp" "src/engine/Program.h" 
    "src/engine/Shader.cpp" "src/engine/Shader.h" 
    "src/engine/Mesh.cpp" "src/engine/Mesh.h" 
    "src/engine/Camera.cpp" "src/engine/Camera.h" 
    "src/engine/Exception.h" "src/engine/Exception.cpp"
    "src/engine/Helper.h" "src/engine/Helper.cpp"
    "src/Controller.cpp" "src/Controller.h"
    "src/Gui.cpp" "src/Gui.h" 
    "src/engine/Material.h" "src/engine/Material.cpp"
    "src/engine/Texture.h" "src/engine/Texture.cpp" 
    "src/engine/Renderpass.h" "src/engine/Renderpass.cpp" 
    "src/engine/AABB.h" "src/engine/AABB.cpp" 
    "src/engine/Multisample_Renderpass.h" "src/engine/Multisample_Renderpass.cpp"
    "src/engine/Multisample_Texture.h" "src/engine/Multisample_Texture.cpp" 
    "src/engine/interface/Texture_Interface.h" "src/engine/interface/Texture_Interface.cpp"  
    "src/Exporter.h" "src/Exporter.cpp" 
    "src/engine/Frustum.h" "src/engine/Frustum.cpp"
 "src/engine/Billboard.h" "src/engine/Billboard.cpp" "src/CameraPreview.h" "src/CameraPreview.cpp")


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

add_executable (CameraPlacement ${SOURCES})

find_package(argparse CONFIG REQUIRED)
target_link_libraries(CameraPlacement PRIVATE argparse::argparse)

find_package(Stb REQUIRED)
target_include_directories(CameraPlacement PRIVATE ${Stb_INCLUDE_DIR})

find_package(assimp CONFIG REQUIRED)
target_link_libraries(CameraPlacement PRIVATE assimp::assimp)

find_package(GLEW REQUIRED)
target_link_libraries(CameraPlacement PRIVATE GLEW::GLEW)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(CameraPlacement PRIVATE glfw)

find_package(glm CONFIG REQUIRED)
target_link_libraries(CameraPlacement PRIVATE glm::glm)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(CameraPlacement PRIVATE imgui::imgui)

set(OpenCV_DIR "${VCPKG_INSTALLED_DIR}/x64-windows/share/opencv2")
find_package(OpenCV REQUIRED)
target_link_libraries(CameraPlacement PRIVATE ${OpenCV_LIBS})

find_path(RAPIDCSV_INCLUDE_DIRS "rapidcsv.h")
target_include_directories(CameraPlacement PRIVATE ${RAPIDCSV_INCLUDE_DIRS})

message(CMAKE_BUILD_TYPE)
