INCLUDEPATH += $$PWD/libopenjpeg/include/openjpeg-2.3
LIBS += -L$$PWD/libopenjpeg/lib/ -llibopenjp2.dll
DEFINES += LS3_HAS_LIBOPENJPEG
