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
    "src/Exporter.h" "src/Exporter.cpp" 
    "src/engine/Frustum.h" "src/engine/Frustum.cpp"
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
    ${IMGUI_BACKEND_DIR}"/imgui_impl_opengl3.cpp" ${IMGUI_BACKEND_DIR}"/imgui_impl_opengl3.h" ${IMGUI_BACKEND_DIR}"/imgui_impl_opengl3_loader.h")
 
set(OPENCV_SOURCES "external/include/opencv2/cvconfig.h" "external/include/opencv2/calib3d/calib3d_c.h" "external/include/opencv2/core/core_c.h" "external/include/opencv2/core/cvdef.h" "external/include/opencv2/core/cv_cpu_dispatch.h" "external/include/opencv2/core/cv_cpu_helper.h" "external/include/opencv2/core/types_c.h" "external/include/opencv2/core/hal/interface.h" "external/include/opencv2/core/hal/msa_macros.h" "external/include/opencv2/features2d/hal/interface.h" "external/include/opencv2/flann/allocator.h" "external/include/opencv2/flann/all_indices.h" "external/include/opencv2/flann/any.h" "external/include/opencv2/flann/autotuned_index.h" "external/include/opencv2/flann/composite_index.h" "external/include/opencv2/flann/config.h" "external/include/opencv2/flann/defines.h" "external/include/opencv2/flann/dist.h" "external/include/opencv2/flann/dummy.h" "external/include/opencv2/flann/dynamic_bitset.h" "external/include/opencv2/flann/general.h" "external/include/opencv2/flann/ground_truth.h" "external/include/opencv2/flann/hdf5.h" "external/include/opencv2/flann/heap.h" "external/include/opencv2/flann/hierarchical_clustering_index.h" "external/include/opencv2/flann/index_testing.h" "external/include/opencv2/flann/kdtree_index.h" "external/include/opencv2/flann/kdtree_single_index.h" "external/include/opencv2/flann/kmeans_index.h" "external/include/opencv2/flann/linear_index.h" "external/include/opencv2/flann/logger.h" "external/include/opencv2/flann/lsh_index.h" "external/include/opencv2/flann/lsh_table.h" "external/include/opencv2/flann/matrix.h" "external/include/opencv2/flann/nn_index.h" "external/include/opencv2/flann/object_factory.h" "external/include/opencv2/flann/params.h" "external/include/opencv2/flann/random.h" "external/include/opencv2/flann/result_set.h" "external/include/opencv2/flann/sampling.h" "external/include/opencv2/flann/saving.h" "external/include/opencv2/flann/simplex_downhill.h" "external/include/opencv2/flann/timer.h" "external/include/opencv2/highgui/highgui_c.h" "external/include/opencv2/imgcodecs/imgcodecs_c.h" "external/include/opencv2/imgcodecs/ios.h" "external/include/opencv2/imgcodecs/macosx.h" "external/include/opencv2/imgcodecs/legacy/constants_c.h" "external/include/opencv2/imgproc/imgproc_c.h" "external/include/opencv2/imgproc/types_c.h" "external/include/opencv2/imgproc/hal/interface.h" "external/include/opencv2/photo/legacy/constants_c.h" "external/include/opencv2/video/legacy/constants_c.h" "external/include/opencv2/videoio/cap_ios.h" "external/include/opencv2/videoio/videoio_c.h" "external/include/opencv2/videoio/legacy/constants_c.h")


# set standard version
if (MSVC) # needed to enable use of __cplusplus macro
    add_compile_options(/Zc:__cplusplus)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

add_executable (CameraPlacement ${SOURCES} ${IMGUI_SOURCES} ${OPENCV_SOURCES} "external/include/stb_image.h" "external/include/rapidcsv.h")

target_compile_definitions(CameraPlacement
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CV_LIB opencv_world490d.lib)
else ()
    set(CV_LIB opencv_world490.lib)
endif ()

message(CMAKE_BUILD_TYPE)

target_link_libraries(CameraPlacement 
    glfw3.lib 
    glew32.lib 
    opengl32.lib 
    glu32.lib 
    assimp-vc143-mt.lib
    ${CV_LIB}
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CameraPlacement PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
