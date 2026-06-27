cmake_minimum_required(VERSION 3.24)

project(
    Islewright
    VERSION 0.1.0
    DESCRIPTION "A 2D tile-map based building and automation simulation game"
    LANGUAGES CXX
)

find_package(EnTT CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)

add_subdirectory(common)
add_subdirectory(client)
add_subdirectory(server)
