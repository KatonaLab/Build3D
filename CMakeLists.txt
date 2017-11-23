cmake_minimum_required(VERSION 3.8.2 FATAL_ERROR)
project(a3dc-app)

set(CMAKE_CXX_STANDARD 14)

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/lib/libics")

add_subdirectory(lib/libics)
add_subdirectory(src)
