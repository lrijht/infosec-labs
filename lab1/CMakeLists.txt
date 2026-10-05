cmake_minimum_required(VERSION 3.16)
project(bdp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic)

add_library(biguint STATIC src/biguint.cpp)
target_include_directories(biguint PUBLIC src)
