QT += core widgets network multimedia

TARGET = RocaTool
TEMPLATE = app

INCLUDEPATH = ../..

HEADERS = \
    rocatool.h \
    spectrum.h \
    ../../editor/audio/Source.h \
    ../../editor/audio/Player.h \
    ../../editor/audio/CodecBase.h \
    ../../editor/audio/Codec.h \
    ../../editor/Utils.h \
    Interface.h

SOURCES = \
    main.cpp \
    rocatool.cpp \
    spectrum.cpp \
    ../../editor/audio/Source.cpp \
    ../../editor/audio/Player.cpp \
    ../../editor/audio/CodecBase.cpp \
    ../../editor/audio/Codec.cpp \
    ../../editor/Utils.cpp

QMAKE_CXXFLAGS += -Wunused-parameter

FORMS += \
    rocatool_window.ui

RESOURCES += \
    res.qrc
