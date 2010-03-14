TARGET = neoMythProgrammer
CONFIG += n64_plugin
TEMPLATE = app
DESTDIR = ../../bin
OBJECTS_DIR = build
RCC_DIR = build
MOC_DIR = build
UI_DIR = build

SOURCES += \
    ../../src/gui/main.cpp \
    ../../src/gui/neomainwindow.cpp \
    ../../src/gui/n64romproperties.cpp \
    ../../src/core/*.cpp
HEADERS += \
    ../../src/gui/neomainwindow.h \
    ../../src/gui/n64romproperties.h \
    ../../src/core/*.h
INCLUDEPATH += \
    ../../src/core
FORMS += \
    ../../src/gui/neomainwindow.ui \
    ../../src/gui/n64romproperties.ui
RESOURCES += ../../src/gui/resources.qrc

n64_plugin {
DEFINES += use_n64_plugin
SOURCES += \
    ../../src/conle_n64_plugin/*.c \
    ../../src/conle_n64_plugin/SuperN64_Engine/*.c \
    ../../src/conle_n64_plugin/SN64PLUG_NeoOpenProgrammer/*.c
HEADERS += \
    ../../src/conle_n64_plugin/*.h \
    ../../src/conle_n64_plugin/SuperN64_Engine/*.h \
    ../../src/conle_n64_plugin/SN64PLUG_NeoOpenProgrammer/*.h
INCLUDEPATH += \
    ../../src/conle_n64_plugin \
    ../../src/conle_n64_plugin/SuperN64_Engine \
    ../../src/conle_n64_plugin/SN64PLUG_NeoOpenProgrammer
}

win32 {
LIBS += -lsetupapi -lkernel32
RC_FILE = ../../src/gui/rsrc.rc
}

LIBS += -lusb
