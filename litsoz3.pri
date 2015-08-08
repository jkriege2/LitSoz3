
DEFINES += LS3LIB_IN_DLL

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/
LS3OUTPUT = $$PWD/output

CONFIG(debug, debug|release) {
    message("DEBUG mode.")
    #linux:QMAKE_CXXFLAGS += -fsanitize=address

}
