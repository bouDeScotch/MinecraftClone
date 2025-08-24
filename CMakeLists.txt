cmake_minimum_required(VERSION 3.16)
project(opengl_basic LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Prevent building lots of examples for dependencies
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

include(FetchContent)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
)
FetchContent_MakeAvailable(glfw)

# GLAD (loader)
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
)
FetchContent_MakeAvailable(glad)

add_executable(app
    src/main.cpp
    src/Player.cpp
    src/Chunk.cpp
    src/World.cpp
    src/Camera.cpp
    src/Renderer.cpp
    src/Shader.cpp
)

# include directories (glad provides target 'glad')
target_link_libraries(app PRIVATE glfw glad ${CMAKE_DL_LIBS})
find_package(glm CONFIG REQUIRED)
target_link_libraries(app PRIVATE glm::glm)
if (WIN32)
    target_link_libraries(app PRIVATE opengl32 gdi32)
endif()

# On some systems you may need to link X11 or pthreads automatically (usually handled by glfw)
# if you want warnings as errors uncomment:
# target_compile_options(app PRIVATE -Wall -Wextra -Werror)
