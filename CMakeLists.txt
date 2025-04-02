###############################################################################################
#
#  @file      CMakeLists.txt
#  @author    Kai Isozumi (kai.isozumi@kohyoung.com)
#  @date      February 25, 2025
#  @brief     This file is Cmake file for building VTKWidget cpp GUI application.
#
#  @copyright Copyright (c) 2025 Koh Young Inc., All rights reserved.
#
###############################################################################################
cmake_minimum_required(VERSION 3.5)

#set(CMAKE_GENERATOR "Visual Studio 17 2022")
project(VTKWidget LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Qt5
set(QTDIR "C:/Users/kai/ThirdParty/Qt/5.15.2/msvc2019_64")
set(QT_LIBRARY_PATH "${QTDIR}/lib")
set(QT_INCLUDE_PATH ${QTDIR}/include)
set(Qt5_DIR ${QT_LIBRARY_PATH}/cmake/Qt5)
set(QT_QPA_PLATFORM_PLUGIN_PATH "${QTDIR}/plugins")

if (CMAKE_BUILD_TYPE MATCHES Release)
    set(QWINDOWS_FILE_PATH "platforms/qwindows.dll")
    set(QWINDOWS_VISTA_STYLE_FILE_PATH "styles/qwindowsvistastyle.dll")
    set(CMAKE_BUILD_TYPE Release)
else()
    set(QWINDOWS_FILE_PATH "platforms/qwindowsd.dll")
    set(QWINDOWS_VISTA_STYLE_FILE_PATH "styles/qwindowsvistastyled.dll")
    set(CMAKE_BUILD_TYPE Debug)
endif()

#VTK
set(VTK_PATH "C:/Users/kai/vtk-v9.1.0/vtk-v9.1.0")
set(VTK_LIBRARY_DIR "${VTK_PATH}/build")
set(VTK_DIR "${VTK_LIBRARY_DIR}/lib/cmake/vtk-9.1")

if(${CMAKE_BUILD_TYPE} STREQUAL  "Debug")
   set(CMAKE_BUILD_TYPE Debug)
elseif(${CMAKE_BUILD_TYPE} STREQUAL  "Release")
   set(CMAKE_BUILD_TYPE Release)
endif ()

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(OpenGL)
find_package(VTK REQUIRED COMPONENTS
    GUISupportQt
    RenderingOpenGL2
    IOGeometry
    IOLegacy
    IOImage
    RenderingCore
    InteractionStyle
    RenderingVolume
    ImagingCore
    ImagingGeneral
    RenderingVolume
    RenderingVolumeOpenGL2
    IOMINC
    ImagingStencil
)

vtk_module_autoinit(
    vtkRenderingOpenGL2
    vtkRenderingVolumeOpenGL2
    TARGETS VTKWidget
)

include_directories(${VTK_INCLUDE_DIRS})

set(SOURCES
    main.cpp
    MainWindow.cpp
    MainWindow.h
    MainWindow.ui
    NiiViewer.cpp
    NiiViewer.h
    NiiViewer.ui
    ControllerWidget.h
    ControllerWidget.cpp
    ControllerWidget.ui
    StatusViewer.h
    StatusViewer.cpp
    StatusViewer.ui
)

add_executable(VTKWidget
    ${SOURCES}
)

target_link_libraries(VTKWidget PRIVATE Qt5::Widgets)
target_link_libraries(VTKWidget PRIVATE
    VTK::GUISupportQt
    VTK::RenderingOpenGL2
    VTK::IOGeometry
    VTK::IOLegacy
    VTK::IOImage
    VTK::RenderingCore
    VTK::InteractionStyle
    VTK::RenderingVolume
    VTK::ImagingCore
    VTK::ImagingGeneral
    VTK::RenderingVolume
    VTK::RenderingVolumeOpenGL2
    VTK::IOMINC
    VTK::RenderingVolumeOpenGL2
    VTK::ImagingStencil
)

include(${VTK_USE_FILE})

set_target_properties(VTKWidget PROPERTIES
    WIN32_EXECUTABLE TRUE
)
