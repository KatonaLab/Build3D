cmake_minimum_required(VERSION 3.9.5 FATAL_ERROR)
project(a3dc-app)

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/lib/libics")

add_subdirectory(lib/libics)
add_subdirectory(src)
