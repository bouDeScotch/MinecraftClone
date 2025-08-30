cmake_minimum_required(VERSION 3.16)
project(opengl_basic LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Prevent building lots of examples for dependencies
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

include(FetchContent)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
)
FetchContent_MakeAvailable(glfw)

# GLAD
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
)
FetchContent_MakeAvailable(glad)

# GLM (header-only)
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
)
FetchContent_MakeAvailable(glm)


add_executable(app
    src/main.cpp
    src/Player.cpp
    src/Chunk.cpp
    src/World.cpp
    src/Camera.cpp
    src/Renderer.cpp
    src/Shader.cpp
)
target_compile_definitions(app PRIVATE GLM_ENABLE_EXPERIMENTAL)
target_link_libraries(app PRIVATE glfw glad glm)

if (WIN32)
    target_link_libraries(app PRIVATE opengl32 gdi32)
elseif (UNIX AND NOT APPLE)
    find_package(OpenGL REQUIRED)
    target_link_libraries(app PRIVATE OpenGL::GL X11 Xi Xrandr Xxf86vm Xinerama Xcursor pthread dl)
endif()
