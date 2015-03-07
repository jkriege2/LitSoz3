#-------------------------------------------------
#
# Project created by QtCreator 2015-03-07T13:26:13
#
#-------------------------------------------------

TARGET = poppler_QT5_demo
TEMPLATE = app

QT       += core gui xml gui network printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(../../extlibs/poppler.pri)


SOURCES += \
    abstractinfodock.cpp \
    documentobserver.cpp \
    embeddedfiles.cpp \
    fonts.cpp \
    info.cpp \
    main_viewer.cpp \
    metadata.cpp \
    navigationtoolbar.cpp \
    optcontent.cpp \
    pageview.cpp \
    permissions.cpp \
    thumbnails.cpp \
    toc.cpp \
    viewer.cpp

HEADERS  += \
    abstractinfodock.h \
    documentobserver.h \
    embeddedfiles.h \
    fonts.h \
    info.h \
    metadata.h \
    navigationtoolbar.h \
    optcontent.h \
    pageview.h \
    permissions.h \
    thumbnails.h \
    toc.h \
    viewer.h

FORMS    +=


