cmake_minimum_required(VERSION 3.16)
project(ImageProcessor VERSION 0.1 LANGUAGES CXX)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OpenCV_DIR "C:/Users/Himanshu Pancholi/Downloads/OpenCV-MinGW-Build-OpenCV-4.5.5-x64/OpenCV-MinGW-Build-OpenCV-4.5.5-x64")


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
# OpenCV config with relative path

find_package(OpenCV REQUIRED)
add_subdirectory(nodeeditor)
set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
)

set(HPP_HEADER_FILES
    include/
)
include_directories(${OpenCV_INCLUDE_DIRS})
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ImageProcessor
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ImageShowModel.cpp
        ImageShowModel.h
        ImageLoaderModel.h
        ImageLoaderModel.cpp
        PixmapData.h
        BrightnessNode.h
        BrightnessNode.cpp
        Splitter.h
        Splitter.cpp
        BlurNode.cpp
        BlurNode.h
        Threshold.h
        Threshold.cpp
        edge.h
        edge.cpp
    )
else()
    if(ANDROID)
        add_library(ImageProcessor SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(ImageProcessor
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Include OpenCV headers and link libraries
target_include_directories(ImageProcessor PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(ImageProcessor PRIVATE Qt${QT_VERSION_MAJOR}::Widgets QtNodes ${OpenCV_LIBS})

# MOVED: Now the copy DLLs command comes AFTER the target is defined
file(GLOB OpenCV_DLLS "${OpenCV_DIR}/bin/*.dll")

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ImageProcessor)
endif()
set_target_properties(ImageProcessor PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
include(GNUInstallDirs)
install(TARGETS ImageProcessor
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ImageProcessor)
endif()
