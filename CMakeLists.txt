cmake_minimum_required(VERSION 3.27)
project(alpha_composition)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${INCLUDE_DIR})
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -fstrict-aliasing -fopenmp-simd")

file(MAKE_DIRECTORY Output)
set(SOURCES
        image.h
        image.cpp
        compositor.h
        compositor.cpp
)
add_executable(CompositorCPU CompositorCPU.cpp ${SOURCES})
find_package(OpenMP REQUIRED)
target_link_libraries(CompositorCPU OpenMP::OpenMP_CXX)
