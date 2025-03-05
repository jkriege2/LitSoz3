cmake_minimum_required(VERSION 3.20)

project(latextools)

qt_add_plugin(${PROJECT_NAME} SHARED )

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Core5Compat Qt6::Gui Qt6::Network Qt6::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE litsoz3tools litsoz3widgets)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${PROJECT_NAME} 
  PRIVATE
    ls3pluginlatextools.h 
    ls3bibteximporter.h 
    ls3bibtexexporter.h 
    bibtexexportdialog.h


    ls3pluginlatextools.cpp 
    ls3bibteximporter.cpp 
    ls3bibtexexporter.cpp 
    bibtexexportdialog.cpp


    bibtexexportdialog.ui

    latextools.qrc
)


qt_add_translations(${PROJECT_NAME}
    TS_FILE_DIR translations
    QM_FILES_OUTPUT_VARIABLE qm_files
)
install(FILES ${qm_files} DESTINATION "${CMAKE_INSTALL_BINDIR}/translations")

include(GNUInstallDirs)
#install(DIRECTORY ./assets/ DESTINATION ${CMAKE_INSTALL_BINDIR}/assets)

install(TARGETS ${PROJECT_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/plugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR}/plugins
)


qt_generate_deploy_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
