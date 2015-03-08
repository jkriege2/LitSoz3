TEMPLATE = app
TARGET = litsoz3


include(../litsoz3.pri)

include(../libls3.pri)
include(../libls3widgets.pri)
include(../autoversioning.pri)
include(../extlibs/poppler.pri)

CONFIG(debug, debug|release) {
    message("building $$TARGET $$TEMPLATE in DEBUG mode, output is in $$LS3OUTPUT ")
} else {
    message("building $$TARGET $$TEMPLATE in RELEASE mode, output is in $$LS3OUTPUT ")
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



RESOURCES   = \
              languages.qrc \
    litsoz3.qrc

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

message("Qt Version: $$QT_MAJOR_VERSION . $$QT_MINOR_VERSION")
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets printsupport
    message("Qt Major Version >5, using special Qt5.x include syntax for widgets")
#    win32{
#        !contains(LIBS, -lopengl32): LIBS += -lopengl32
#        !contains(LIBS, -lglu32): LIBS += -lglu32
#    }
}
win32 {
    RC_FILE = litsoz3.rc
}

QT += gui xml webkitwidgets
CONFIG += exceptions rtti stl link_prl


macx{
  exists(ls3icon.icns) {
    ICON = ls3icon.icns
  }
}




ASSETSSTYLESHEET_FILES.files = ./assets/stylesheets/*.*
ASSETSSTYLESHEET_FILES.path = $${LS3OUTPUT}/assets/stylesheets/

ASSETS_FILES.files = ./assets/*.*
ASSETS_FILES.path = $${LS3OUTPUT}/assets/

ASSETSCOMPLETERS_FILES.files = ./assets/completers/*.*
ASSETSCOMPLETERS_FILES.path = $${LS3OUTPUT}/assets/completers/

ASSETSFDF_FILES.files = ./assets/fdf/*.*
ASSETSFDF_FILES.path = $${LS3OUTPUT}/assets/fdf/

ASSETSCSL_FILES.files = ./assets/csl/*.*
ASSETSCSL_FILES.path = $${LS3OUTPUT}/assets/csl/

ASSETSCSLLOCALES_FILES.files = ./assets/csl_locales/*.*
ASSETSCSLLOCALES_FILES.path = $${LS3OUTPUT}/assets/csl_locales/

ASSETSCSLLANG_FILES.files = ./assets/language_recognition/*.*
ASSETSCSLLANG_FILES.path = $${LS3OUTPUT}/assets/language_recognition/

INSTALLS += ASSETSSTYLESHEET_FILES ASSETS_FILES ASSETSCOMPLETERS_FILES ASSETSFDF_FILES ASSETSCSLLOCALES_FILES ASSETSCSL_FILES ASSETSCSLLANG_FILES


