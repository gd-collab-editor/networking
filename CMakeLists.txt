cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -DASIO_STANDALONE -pthread")

project(collab-editor-networking VERSION 0.1.0 LANGUAGES CXX)

aux_source_directory(depends/asio ASIO_SRC)

add_subdirectory(depends/GDLevel)
add_subdirectory(server)