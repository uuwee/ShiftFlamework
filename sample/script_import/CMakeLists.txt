cmake_minimum_required(VERSION 3.26)

project(test_script)

set(SMAKE_CXX_STANDARD 20)

file(GLOB includes include/*.hpp)
file(GLOB sources source/*.cpp)

add_library(test_script SHARED ${sources} ${includes})

target_compile_features(test_script PRIVATE cxx_std_20)

target_include_directories(test_script PUBLIC include)