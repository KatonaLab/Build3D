cmake_minimum_required(VERSION 2.8.3)
project(a3dc-app)

include_directories("${PROJECT_SOURCE_DIR}/include")

add_subdirectory(lib/libics)
add_subdirectory(src)