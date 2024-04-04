cmake_minimum_required(VERSION 3.27)
project(alpha_composition)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -fstrict-aliasing -fopenmp-simd")

file(MAKE_DIRECTORY Output)
add_executable(Compositor CompositorSerial.cpp)
find_package(OpenMP REQUIRED)
target_link_libraries(Compositor OpenMP::OpenMP_CXX)
