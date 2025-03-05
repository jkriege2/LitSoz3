cmake_minimum_required(VERSION 3.20)

project(litsoz3widgets)

include(GenerateExportHeader)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

qt_add_library(${PROJECT_NAME} SHARED )
add_library(litsoz3::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

generate_export_header(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Core5Compat Qt6::Network Qt6::Xml Qt6::Gui Qt6::Widgets Qt6::PrintSupport)
target_link_libraries(${PROJECT_NAME} PRIVATE litsoz3tools)
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
    qcompleterfromfile.h
    qcompleterplaintextedit.h
    qenhancedlineedit.h
    qmappablecombobox.h
    qspecialtoolbutton.h
    jkstarratingwidget.h
    jkstyledbutton.h
    jkcharacterscreenkeyboard.h
    flowlayout.h
    htmldelegate.h
    qenhancedtableview.h
    qmodernprogresswidget.h
    qcheckablestringlistmodel.h
    qrecentfilesmenu.h
    qfilenameslistedit.h
    jkverticalscrollarea.h
)

target_sources(${PROJECT_NAME}
  PRIVATE
    qcompleterfromfile.cpp
    qcompleterplaintextedit.cpp
    qenhancedlineedit.cpp
    qmappablecombobox.cpp
    qspecialtoolbutton.cpp
    jkstarratingwidget.cpp
    jkstyledbutton.cpp
    jkcharacterscreenkeyboard.cpp
    flowlayout.cpp
    htmldelegate.cpp
    qenhancedtableview.cpp
    qmodernprogresswidget.cpp
    qcheckablestringlistmodel.cpp
    qrecentfilesmenu.cpp
    qfilenameslistedit.cpp
    jkverticalscrollarea.cpp

    libwidgets.qrc
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
