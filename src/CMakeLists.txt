set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets 3DCore 3DExtras 3DRender 3DInput 3DQuickExtras)

set(Python_ADDITIONAL_VERSIONS 3.6)
find_package(PythonLibs 3 REQUIRED)

# FIXME:
include_directories("/Library/Frameworks/Python.framework/Versions/3.6/include/python3.6m")
include_directories("/Library/Frameworks/Python.framework/Versions/3.6/lib/python3.6/site-packages/numpy/core/include")

file(GLOB sources *.cpp)
add_executable(app ${sources})
target_link_libraries(app
    libics 
    ${PYTHON_LIBRARIES}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::3DCore
    Qt5::3DExtras
    Qt5::3DRender
    Qt5::3DInput
    Qt5::3DQuickExtras
    "/Library/Frameworks/Python.framework/Versions/3.6/lib/python3.6/site-packages/numpy/core/lib/libnpymath.a"
    )

