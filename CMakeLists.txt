cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0072 NEW)  # No more nagging 🥱

project(RubiksCubeSim C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

file(GLOB SOURCES "src/*.c")
add_executable(rubik ${SOURCES})

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_path(CGLM_INCLUDE_DIR cglm/cglm.h)

include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
    ${CGLM_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(rubik
    ${SDL2_LIBRARIES}
    ${OPENGL_LIBRARIES}
    GLEW::GLEW
    m
)
