cmake_minimum_required(VERSION 3.20)
project(Tetris LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# raylib headers (raylib.h, rcamera.h, raymath.h, ...) live in the source tree;
# the prebuilt static lib ships inside the w64devkit bundle
set(RAYLIB_SRC "C:/raylib/raylib/src" CACHE PATH "Path to the raylib source (headers live here)")
set(RAYLIB_ROOT "C:/raylib/w64devkit" CACHE PATH "Path to the raylib install (libraylib.a in lib/)")

add_executable(${PROJECT_NAME}
    src/main.c
)

target_include_directories(${PROJECT_NAME} PRIVATE "${RAYLIB_SRC}")
target_link_directories(${PROJECT_NAME} PRIVATE "${RAYLIB_ROOT}/lib")
target_link_libraries(${PROJECT_NAME} PRIVATE raylib opengl32 gdi32 winmm)
