cmake_minimum_required(VERSION 3.20)

project(litsoz3app)

qt_add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
)

find_package(Poppler)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Xml Qt6::Gui Qt6::Core5Compat Qt6::Widgets Qt6::PrintSupport Qt6::Network)
target_link_libraries(${PROJECT_NAME} PRIVATE litsoz3::litsoz3tools)
target_link_libraries(${PROJECT_NAME} PRIVATE litsoz3::litsoz3widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Poppler::PopplerQt6)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${PROJECT_NAME} 
  PRIVATE
    mainwindow.h
    detailpane.h
    optionsdialog.h
    programoptions.h
    fdf.h
    extendedpane.h
    contentspane.h
    ls3dsstringtablemodel.h
    ls3datastorexml.h
    ls3summaryproxymodel.h
    ls3summarysortfilterproxymodel.h
    selectionlist.h
    previewstylemanager.h
    searchresultlist.h
    searchresultmodel.h
    searchdialog.h
    exportdialog.h
    importdialog.h
    referencetreedockwidget.h
    threadoverview.h
    ls3progressthreadwidget.h
    fileviewpane.h
    #ls3pdfwidget.h
    dlgeditcategory.h
    #zoomselector.h
    #qenhancedpdfview.h
    popplerpdfwidget.h



  
    main.cpp
    mainwindow.cpp
    detailpane.cpp
    optionsdialog.cpp
    programoptions.cpp
    fdf.cpp
    extendedpane.cpp
    contentspane.cpp
    ls3datastorexml.cpp
    ls3dsstringtablemodel.cpp
    ls3summaryproxymodel.cpp
    ls3summarysortfilterproxymodel.cpp
    selectionlist.cpp
    previewstylemanager.cpp
    searchresultlist.cpp
    searchresultmodel.cpp
    searchdialog.cpp
    exportdialog.cpp
    importdialog.cpp
    referencetreedockwidget.cpp
    threadoverview.cpp
    ls3progressthreadwidget.cpp
    fileviewpane.cpp
    #ls3pdfwidget.cpp
    dlgeditcategory.cpp
    #zoomselector.cpp
    #qenhancedpdfview.cpp
    popplerpdfwidget.cpp

    
    litsoz3app.rc


    optionsdialog.ui
    about.ui
    aboutplugins.ui
    selectionlist.ui
    searchresultlist.ui
    searchdialog.ui
    exportdialog.ui
    importdialog.ui
    referencetreedockwidget.ui
    ls3progressthreadwidget.ui
    dlgeditcategory.ui
    litsoz3app.qrc
    languages.qrc
)

qt_add_resources(${PROJECT_NAME}
    litsoz3app.qrc
    languages.qrc
)


qt_add_translations(${PROJECT_NAME}
    TS_FILE_DIR translations
    QM_FILES_OUTPUT_VARIABLE qm_files
)
install(FILES ${qm_files} DESTINATION "${CMAKE_INSTALL_BINDIR}/translations")


include(GNUInstallDirs)
install(DIRECTORY ./assets/ DESTINATION ${CMAKE_INSTALL_BINDIR}/assets)
install(DIRECTORY ./stylesheets/ DESTINATION ${CMAKE_INSTALL_BINDIR}/stylesheets)
install(DIRECTORY ./test_db/ DESTINATION ${CMAKE_INSTALL_BINDIR}/test_db)

install(TARGETS ${PROJECT_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})


