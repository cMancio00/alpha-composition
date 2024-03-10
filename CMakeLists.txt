cmake_minimum_required(VERSION 3.27)
project(alpha_composition)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -fstrict-aliasing -fopenmp-simd")

file(MAKE_DIRECTORY Output)
add_executable(CompositorSerial CompositorSerial.cpp)
add_executable(CompositorParallel CompositorParallel.cpp)
find_package(OpenMP REQUIRED)
target_link_libraries(CompositorSerial OpenMP::OpenMP_CXX)
target_link_libraries(CompositorParallel OpenMP::OpenMP_CXX)