cmake_minimum_required(VERSION 3.27)
project(alpha_composition)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${INCLUDE_DIR})
set(CMAKE_CXX_STANDARD 20)

file(MAKE_DIRECTORY Output)
set(SOURCES
        image.h
        image.cpp
        compositor.h
        compositor.cpp
)

# Find OpenMP (for CPU and GPU)
find_package(OpenMP REQUIRED)

add_executable(CompositorGPU CompositorGPU.cpp ${SOURCES})
target_link_libraries(CompositorGPU PRIVATE OpenMP::OpenMP_CXX)
target_compile_options(CompositorGPU PRIVATE -fopenmp)

# CompositorCPU
add_executable(CompositorCPU CompositorCPU.cpp ${SOURCES})
target_link_libraries(CompositorCPU PRIVATE OpenMP::OpenMP_CXX)
target_compile_options(CompositorCPU PRIVATE ${OpenMP_CXX_FLAGS})  # OpenMP compiler flags
