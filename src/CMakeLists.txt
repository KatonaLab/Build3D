file(GLOB sources *.cpp)
add_executable(app ${sources})
target_link_libraries(app libics)