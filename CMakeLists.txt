cmake_minimum_required (VERSION 3.8)
project ("AlgorithmVisualizer")

include(FetchContent)

set(RAYLIB_VERSION 5.0)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    FIND_PACKAGE_ARGS ${RAYLIB_VERSION} EXACT
)
set (BUILD_EXAMPLES OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(raylib)

add_executable (${PROJECT_NAME} "src/main.cpp")
target_sources(${PROJECT_NAME} PUBLIC 
    src/raygui_include.cpp
    src/sort.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC  "src" "include")
target_link_libraries(${PROJECT_NAME} raylib)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
file(COPY assets/fonts DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")