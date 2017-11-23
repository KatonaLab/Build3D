set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ADD_DEFINITIONS(${QT_DEFINITIONS})
# # ADD_DEFINITIONS(-DQT_PLUGIN)
# # ADD_DEFINITIONS(-DQT_NO_DEBUG)
# # ADD_DEFINITIONS(-DQT_SHARED)
# add_definitions("-DQT_STATICPLUGIN")

find_package(Qt5 REQUIRED COMPONENTS 
    Core 
    Gui 
    Widgets 
    3DCore 
    3DExtras 
    3DRender 
    3DInput 
    3DQuickExtras 
    Quick 
    QuickWidgets
    # PATHS /usr/local/opt/qt/lib/cmake/Qt5
    PATHS /Users/fodorbalint/projects/tmp/mxe/usr/i686-w64-mingw32.static/qt5/lib/cmake/Qt5
    NO_DEFAULT_PATH
)

file(GLOB sources *.cpp)
file(GLOB headers *.h)

SET(BUILD_SHARED_LIBRARIES OFF)
SET(CMAKE_EXE_LINKER_FLAGS "-static")

add_definitions("-DQT_STATICPLUGIN")

link_directories("/Users/fodorbalint/projects/tmp/mxe/usr/i686-w64-mingw32.static/qt5/lib")

add_executable(app ${sources} ${headers} ../resources.qrc)

foreach(plugin ${Qt5Core_PLUGINS} 
    ${Qt5Gui_PLUGINS} 
    ${Qt5Widgets_PLUGINS} 
    ${Qt53DCore_PLUGINS} 
    ${Qt53DExtras_PLUGINS} 
    ${Qt53DRender_PLUGINS} 
    ${Qt53DInput_PLUGINS} 
    ${Qt53DQuickExtras_PLUGINS}
    ${Qt5Quick_PLUGINS}
    ${Qt5QuickWidgets_PLUGINS}
    )

    get_target_property(_loc ${plugin} LOCATION)
#    message("Plugin ${plugin} is at location ${_loc}")
#    # link_directories(${_loc})
    target_link_libraries(app ${_loc})
endforeach()

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

target_link_libraries(app
    libics_static
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::3DCore
    Qt5::3DExtras
    Qt5::3DRender
    Qt5::3DInput
    Qt5::3DQuickExtras
    Qt5::Quick
    Qt5::QuickWidgets
    Qt5::QWindowsIntegrationPlugin
    libQt5EventDispatcherSupport.a
    libQt5FontDatabaseSupport.a
    libQt5ThemeSupport.a
    libQt5Quick.a
    libQt5QuickControls2.a
    freetype
    bz2
    harfbuzz
    freetype_too
    harfbuzz_too
)
