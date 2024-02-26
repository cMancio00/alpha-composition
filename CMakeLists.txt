cmake_minimum_required(VERSION 3.27)
project(alpha_composition)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(Compositor main.cpp)

#find_package(OpenMP REQUIRED)
#target_link_libraries(Compositor OpenMP::OpenMP_CXX)