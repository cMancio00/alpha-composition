cmake_minimum_required(VERSION 3.27)
project(alpha_composition)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_CXX_STANDARD 20)

set(COMMON_SOURCES
        image.cpp
)

set(SOURCES_CPU
        CompositorCPU.cpp
        OpenMP_compositor.cpp
)

find_package(OpenMP REQUIRED)
add_executable(CompositorCPU ${COMMON_SOURCES} ${SOURCES_CPU})
target_link_libraries(CompositorCPU PRIVATE OpenMP::OpenMP_CXX)

