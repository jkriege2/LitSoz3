cmake_minimum_required(VERSION 3.20)

project(litsoz3tools)

include(GenerateExportHeader)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

qt_add_library(${PROJECT_NAME} SHARED )
add_library(litsoz3::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

generate_export_header(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Core5Compat Qt6::Xml Qt6::Gui Qt6::Network Qt6::Widgets Qt6::PrintSupport)
# set default include paths
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
)
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
)
target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_EXPORTS)

target_sources(${PROJECT_NAME}
  PUBLIC FILE_SET HEADERS TYPE HEADERS
  FILES
    bibtools.h 
    ls3plugin.h 
    ls3pluginservices.h 
    ls3datastore.h 
    ls3selectiondisplaymodel.h 
    ls3importexport.h 
    ls3pluginbase.h 
    filenametools.h
    latextools.h 
    htmltools.h 
    crossreftools.h 
    ls3extractmetadataplugin.h 
    pubmedtools.h 
    ls3referencetreeitem.h 
    ls3referencetreemodel.h 
    htmltokenizer.h 
    ls3progressthread.h 
    filetools.h 
    modstools.h 
    csltools.h 
    languagetools.h 
    ls3tools.h
    cachingtools.h
    regextools.h
)
target_sources(${PROJECT_NAME}
  PRIVATE
    bibtools.cpp 
    ls3datastore.cpp 
    ls3selectiondisplaymodel.cpp 
    filenametools.cpp 
    latextools.cpp 
    htmltools.cpp 
    crossreftools.cpp 
    pubmedtools.cpp 
    ls3referencetreeitem.cpp 
    ls3referencetreemodel.cpp 
    htmltokenizer.cpp 
    ls3progressthread.cpp 
    filetools.cpp 
    modstools.cpp 
    csltools.cpp 
    languagetools.cpp 
    ls3tools.cpp
    cachingtools.cpp
    regextools.cpp

    lib.qrc
)

qt_add_translations(${PROJECT_NAME}
    TS_FILE_DIR translations
    QM_FILES_OUTPUT_VARIABLE qm_files
)
install(FILES ${qm_files} DESTINATION "${CMAKE_INSTALL_BINDIR}/translations")


include(GNUInstallDirs)
#install(DIRECTORY ./assets/ DESTINATION ${CMAKE_INSTALL_BINDIR}/assets)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}_TARGETS
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)
install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_export.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

qt_generate_deploy_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

# install export files
install(EXPORT ${PROJECT_NAME}_TARGETS
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    NAMESPACE ${jkqtplotter_namespace}
)
