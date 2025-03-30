cmake_minimum_required(VERSION 3.28)
project(Master-Engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_library(Master-Engine)
target_sources(Master-Engine PRIVATE src/gameObject.cpp src/physics.cpp src/renderer.cpp src/textureLoader.cpp)
target_link_libraries(Master-Engine PUBLIC raylib)
target_include_directories(Master-Engine PUBLIC .)
