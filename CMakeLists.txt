cmake_minimum_required(VERSION 3.10)
project(hehcraft)

# Set policy CMP0072 to use GLVND by default when available
if(POLICY CMP0072)
  cmake_policy(SET CMP0072 NEW)
endif()

set(CMAKE_SKIP_BUILD_RPATH  FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Os -s")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -Os -s")

# Paths to external libraries
set(GLFW_DIR "${CMAKE_SOURCE_DIR}/external/glfw")
set(GLM_DIR "${CMAKE_SOURCE_DIR}/external/glm")

# Include directories
include_directories(${GLM_DIR})
include_directories(${CMAKE_SOURCE_DIR}/external/tiny_obj_loader)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Find packages
# find_package(OpenGL REQUIRED)
find_package(Vulkan REQUIRED)

# Manually specify the paths to GLFW if not found by find_package
if (NOT TARGET glfw)
    # add_subdirectory(${GLFW_DIR})
    include_directories(${GLFW_DIR}/include)
    link_directories(${GLFW_DIR}/lib)
endif()

# Add executable
add_executable(hehcraft
    src/main.cpp
    src/window.cpp
    src/pipeline.cpp
    src/device.cpp
    src/swap_chain.cpp
    src/model.cpp
    src/renderer.cpp
    src/simple_render_system.cpp
    src/app.cpp
    src/camera.cpp
    src/keyboard_controller.cpp
    src/game_object.cpp
    src/buffer.cpp
)

# Link libraries
target_link_libraries(hehcraft
    glfw
    ${OPENGL_LIBRARIES}
    Vulkan::Vulkan
)

if (WIN32)
    target_link_libraries(hehcraft opengl32)
endif()

if (UNIX)
    target_link_libraries(hehcraft GL X11 Xxf86vm Xrandr Xi pthread dl)
endif()