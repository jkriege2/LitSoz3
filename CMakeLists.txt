cmake_minimum_required(VERSION 3.20)

# Add folder where are supportive functions
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


# Basic information about project

project(litsoz3 
          VERSION 3.0 
          LANGUAGES CXX
          HOMEPAGE_URL "https://github.com/jkriege2/LitSoz3"
          )


# Set additional project information
set(COMPANY "Jan Krieger")
set(COPYRIGHT "Copyright (c) 2013-2025 Jan Krieger. All rights reserved.")

# default CXX settings 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)
# Instruct CMake to run moc+rcc+uic automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    SET(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/dist" CACHE PATH "LitSoz3 install prefix" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

set(CMAKE_INSTALL_BINDIR "." CACHE STRING "binary subdir")
set(CMAKE_INSTALL_INCLUDEDIR "sdk_include" CACHE STRING "sdk-include-subdir")
set(CMAKE_INSTALL_LIBDIR "sdk_lib" CACHE STRING "sdk-lib-subdir")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/modules/")

# include Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets Pdf PdfWidgets Xml Gui Network PrintSupport LinguistTools Core5Compat)
qt_standard_project_setup(
    I18N_SOURCE_LANGUAGE en         # optional - this is the default
    I18N_TRANSLATED_LANGUAGES de
)


#set(QT_DEPLOY_PLUGINS_DIR "qtplugins")

#include("${QT_DEPLOY_SUPPORT}")
set(QT_DEPLOY_PLUGINS_DIR "qtplugins" CACHE STRING "Qt PLugins install dir" FORCE)



add_subdirectory(extlibs)
add_subdirectory(lib)
add_subdirectory(libwidgets)
add_subdirectory(application)
add_subdirectory(plugins)
