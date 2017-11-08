set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# LDFLAGS:  -L/usr/local/opt/qt/lib
# set(Qt5_DIR /usr/local/opt/qt)

# set(CMAKE_EXE_LINKER_FLAGS "-Wl,-export-dynamic")

find_package(Qt5 REQUIRED COMPONENTS 
    Core Gui Widgets 3DCore 3DExtras 3DRender 3DInput 3DQuickExtras
    PATHS /usr/local/opt/qt/lib/cmake/Qt5
    NO_DEFAULT_PATH)

# Find necessary packages
find_package(PythonLibs 3.6.0 REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS python numpy REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

# set(Python_ADDITIONAL_VERSIONS 3.6)
# find_package(PythonLibs 3 REQUIRED)

# find_package(Boost 1.65.1 COMPONENTS program_options REQUIRED)
# include_directories( ${Boost_INCLUDE_DIR} )

# FIXME:
# include_directories("/Library/Frameworks/Python.framework/Versions/3.6/include/python3.6m")
# include_directories("/Library/Frameworks/Python.framework/Versions/3.6/lib/python3.6/site-packages/numpy/core/include")

file(GLOB sources *.cpp)
file(GLOB headers *.h)

add_executable(app ${sources} ${headers} ../resources.qrc)
target_link_libraries(app
    libics 
    # ${PYTHON_LIBRARIES}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::3DCore
    Qt5::3DExtras
    Qt5::3DRender
    Qt5::3DInput
    Qt5::3DQuickExtras
    ${Boost_LIBRARIES}
    # "/Library/Frameworks/Python.framework/Versions/3.6/lib/python3.6/site-packages/numpy/core/lib/libnpymath.a"
    )

# TODO: copy on windows
# add_custom_command(TARGET app PRE_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#     ${CMAKE_SOURCE_DIR}/src/qml $<TARGET_FILE_DIR:app>/qml)

# add_custom_command(TARGET app PRE_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#     ${CMAKE_SOURCE_DIR}/scripts $<TARGET_FILE_DIR:app>/scripts)
#

# add_custom_command(TARGET app PRE_BUILD
#     COMMAND ${CMAKE_COMMAND} -E create_symlink
#     ${CMAKE_SOURCE_DIR}/src/qml $<TARGET_FILE_DIR:app>/qml)

# add_custom_command(TARGET app PRE_BUILD
#     COMMAND ${CMAKE_COMMAND} -E create_symlink
#     ${CMAKE_SOURCE_DIR}/scripts $<TARGET_FILE_DIR:app>/scripts)