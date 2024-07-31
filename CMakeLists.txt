# CMakeList.txt : CMake project for CameraPlacement, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

include_directories("external/include" "external/imgui-master" "external/imgui-master/backends")
link_directories("external/lib")

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
    "src/Exporter.h"
)

set(IMGUI_DIR "external/imgui-master/")
set(IMGUI_BACKEND_DIR "external/imgui-master/backends")

set(IMGUI_SOURCES ${IMGUI_DIR}"/imgui.h" ${IMGUI_DIR}"/imgui.cpp"
    ${IMGUI_DIR}"/imstb_rectpack.h"
    ${IMGUI_DIR}"/imstb_textedit.h"
    ${IMGUI_DIR}"/imstb_truetype.h"
    ${IMGUI_DIR}"/imgui_demo.cpp"
    ${IMGUI_DIR}"/imgui_draw.cpp"
    ${IMGUI_DIR}"/imgui_internal.h"
    ${IMGUI_DIR}"/imgui_tables.cpp"
    ${IMGUI_DIR}"/imgui_widgets.cpp"

    ${IMGUI_BACKEND_DIR}"/imgui_impl_glfw.cpp" ${IMGUI_BACKEND_DIR}"/imgui_impl_glfw.h"
    ${IMGUI_BACKEND_DIR}"/imgui_impl_opengl3.cpp" ${IMGUI_BACKEND_DIR}"/imgui_impl_opengl3.h" ${IMGUI_BACKEND_DIR}"/imgui_impl_opengl3_loader.h"
)

# set standard version
if (MSVC) # needed to enable use of __cplusplus macro
    add_compile_options(/Zc:__cplusplus)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

add_executable (CameraPlacement ${SOURCES} ${IMGUI_SOURCES} "external/include/stb_image.h" "external/include/rapidcsv.h")

target_compile_definitions(CameraPlacement
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
)

target_link_libraries(CameraPlacement glfw3.lib glew32.lib opengl32.lib glu32.lib assimp-vc143-mt.lib)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CameraPlacement PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
