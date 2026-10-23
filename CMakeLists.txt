cmake_minimum_required(VERSION 3.20)

# Renaming the project: change the name below only. Nothing else in this repo
# (tasks.json, launch.json, main.c) needs to be touched, since the binary is
# always emitted as app.exe and the window title is injected via APP_NAME.
project(raylib_template LANGUAGES C CXX)

# Ninja is a single-config generator: CMAKE_BUILD_TYPE must be set at configure
# time. Default to Debug if nothing was passed (the Debug/Release tasks always
# pass it explicitly).
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Root of the raylib Windows installer package (contains raylib/ and w64devkit/).
# This is the one setting that changes between machines: override it by setting
# the RAYLIB_PATH environment variable before launching VS Code / CMake. The
# same variable is read by tasks.json, launch.json and c_cpp_properties.json.
if(DEFINED ENV{RAYLIB_PATH})
    set(RAYLIB_PATH_DEFAULT "$ENV{RAYLIB_PATH}")
else()
    set(RAYLIB_PATH_DEFAULT "C:/raylib")
endif()
set(RAYLIB_PATH "${RAYLIB_PATH_DEFAULT}" CACHE PATH "Root of the raylib installer package")

# raylib headers (raylib.h, rcamera.h, raymath.h, ...) live in the source tree;
# the prebuilt static lib ships inside the w64devkit bundle
set(RAYLIB_SRC "${RAYLIB_PATH}/raylib-6.2/src")
set(RAYLIB_ROOT "${RAYLIB_PATH}/w64devkit")

# add all project files below
add_executable(${PROJECT_NAME}
    src/main.c
    # src/example.cpp
    # etc.
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "app")
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "APP_NAME=\"${PROJECT_NAME}\""
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(${PROJECT_NAME} PRIVATE "${RAYLIB_SRC}")
target_link_directories(${PROJECT_NAME} PRIVATE "${RAYLIB_ROOT}/lib")
target_link_libraries(${PROJECT_NAME} PRIVATE raylib opengl32 gdi32 winmm)

# Hide the console window in Release only; Debug keeps it so TraceLog/printf are visible.
if(WIN32 AND CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:-mwindows>)
endif()

# # Change folder names after ASSET_DIR based on what assets you need included with binary
# # Used blocks / music / sound as example directories
foreach(ASSET_DIR blocks music sound)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/${ASSET_DIR}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${ASSET_DIR}"
    )
endforeach()
