TEMPLATE = app
TARGET = litsoz3


include(../litsoz3.pri)

include(../libls3.pri)
include(../libls3widgets.pri)
include(../autoversioning.pri)
include(../extlibs/poppler.pri)

release {
    message("building $$TARGET $$TEMPLATE in RELEASE mode, output is in $$LS3OUTPUT ")
} else {
    message("building $$TARGET $$TEMPLATE in DEBUG mode, output is in $$LS3OUTPUT ")
}
macx {
    message("build system is macx")
}
win32 {
    message("build system is win32")
}
unix {
    message("build system is unix")
}

DESTDIR = $$LS3OUTPUT

QMAKE_RPATHDIR += $$DESTDIR


INCLUDEPATH += . \

HEADERS     = mainwindow.h \
              detailpane.h \
              optionsdialog.h \
              programoptions.h \
              fdf.h \
              extendedpane.h \
              contentspane.h \
              ls3dsstringtablemodel.h \
              ls3datastorexml.h \
              ls3summaryproxymodel.h \
              ls3summarysortfilterproxymodel.h \
              selectionlist.h \
              previewstylemanager.h \
              searchresultlist.h \
              searchresultmodel.h \
              searchdialog.h \
              exportdialog.h \
              importdialog.h \
              referencetreedockwidget.h \
    threadoverview.h \
    ls3progressthreadwidget.h \
    fileviewpane.h \
    popplerpdfwidget.h



RESOURCES   = litsoz3.qrc \
              languages.qrc

SOURCES     = main.cpp \
              mainwindow.cpp \
              detailpane.cpp \
              optionsdialog.cpp \
              programoptions.cpp \
              fdf.cpp \
              extendedpane.cpp \
              contentspane.cpp \
              ls3datastorexml.cpp \
              ls3dsstringtablemodel.cpp \
              ls3summaryproxymodel.cpp \
              ls3summarysortfilterproxymodel.cpp \
              selectionlist.cpp \
              previewstylemanager.cpp \
              searchresultlist.cpp \
              searchresultmodel.cpp \
              searchdialog.cpp \
              exportdialog.cpp \
              importdialog.cpp \
              referencetreedockwidget.cpp \
    threadoverview.cpp \
    ls3progressthreadwidget.cpp \
    fileviewpane.cpp \
    popplerpdfwidget.cpp

FORMS       = optionsdialog.ui \
              about.ui \
              aboutplugins.ui \
              selectionlist.ui \
              searchresultlist.ui \
              searchdialog.ui \
              exportdialog.ui \
              importdialog.ui \
              referencetreedockwidget.ui \
    ls3progressthreadwidget.ui

TRANSLATIONS= $$LS3OUTPUT/translations/de.litsoz3.ts

QT += xml gui network

win32 {
    RC_FILE = litsoz3.rc
}

QT += gui xml
CONFIG += exceptions rtti stl link_prl


macx{
  exists(ls3icon.icns) {
    ICON = ls3icon.icns
  }
}


