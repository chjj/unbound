# CMakeLists.txt - cmake build for unbound
# Copyright (c) 2020, Christopher Jeffrey (MIT License).
# https://github.com/chjj/unbound

cmake_minimum_required(VERSION 3.4)
project(node_unbound LANGUAGES C)

include(NodeJS)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 99)

add_node_module(node_unbound src/node_unbound.c)

if(NOT MSVC)
  target_compile_options(node_unbound PRIVATE -Wall -Wextra)
endif()

target_link_libraries(node_unbound PRIVATE unbound)
