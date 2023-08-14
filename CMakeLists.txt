cmake_minimum_required(VERSION 3.13)
project(game)
set(CMAKE_CXX_STANDARD 20)

set(DAWN_FETCH_DEPENDENCIES ON)
add_subdirectory("lib/dawn" EXCLUDE_FROM_ALL)

file(GLOB sources source/*.cpp)

add_executable(game ${sources})

target_link_libraries(game PRIVATE webgpu_dawn webgpu_cpp webgpu_glfw)