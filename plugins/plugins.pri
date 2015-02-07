INCLUDEPATH += ../../lib/ \
               ../../libwidgets/ 

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/
LS3OUTPUT = ../../output
DESTDIR = $$LS3OUTPUT/plugins



CONFIG(debug, debug|release) {
    message("building $$TARGET $$TEMPLATE in DEBUG mode, output is in $$LS3OUTPUT ")
} else {
    message("building $$TARGET $$TEMPLATE in RELEASE mode, output is in $$LS3OUTPUT ")
}


DEFINES += QTLIB_IN_DLL LIB_IN_DLL

CONFIG += link_prl

include(../libls3.pri)

HEADERS +=

SOURCES +=




