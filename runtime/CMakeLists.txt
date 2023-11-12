cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 20)
project(runtime)

file(GLOB sources *.cpp)
file (GLOB includes include/*.hpp)
add_executable(runtime ${sources} ${includes})
target_compile_options(runtime PRIVATE "-D DLL_IMPORT")
target_include_directories(runtime PUBLIC include)