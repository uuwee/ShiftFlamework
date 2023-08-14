cmake_minimum_required(VERSION 3.13)
project(game)
set(CMAKE_CXX_STANDARD 20)

file(GLOB sources source/*.cpp)

add_executable(game ${sources})

if(EMSCRIPTEN)
    set_target_properties(game PROPERTIES SUFFIX ".html")
    target_link_options(game PRIVATE "-sUSE_WEBGPU=1" "-sUSE_GLFW=3")
else()
    set(DAWN_FETCH_DEPENDENCIES ON)
    add_subdirectory("lib/dawn" EXCLUDE_FROM_ALL)
    target_link_libraries(game PRIVATE webgpu_dawn webgpu_cpp webgpu_glfw)
endif()

