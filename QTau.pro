#-------------------------------------------------
# http://github.com/digited/qtau
#-------------------------------------------------

QT += core gui network

TARGET = QTau
TEMPLATE = app

CONFIG += audio_backend_qtmm

audio_backend_vlc {
    LIBS += -lvlc -lvlccore
    SOURCES += editor/audio/vlcPlayer.cpp
    HEADERS += editor/audio/vlcPlayer.h
}

audio_backend_qtmm {
    windows:QT += multimedia

    !windows:INCLUDEPATH += /usr/include/QtMultimediaKit /usr/include/QtMobility
    !windows:LIBS += -lQtMultimediaKit

    SOURCES += editor/audio/qtmmPlayer.cpp
    HEADERS += editor/audio/qtmmPlayer.h
}

INCLUDEPATH += tools

windows {
RC_FILE = editor/res/qtau_win.rc
}

SOURCES += \
    editor/main.cpp \
    editor/mainwindow.cpp \
    editor/qtsingleapplication/qtsinglecoreapplication.cpp \
    editor/qtsingleapplication/qtsingleapplication.cpp \
    editor/qtsingleapplication/qtlockedfile_win.cpp \
    editor/qtsingleapplication/qtlockedfile_unix.cpp \
    editor/qtsingleapplication/qtlockedfile.cpp \
    editor/qtsingleapplication/qtlocalpeer.cpp \
    editor/audio/File.cpp \
    editor/Session.cpp \
    editor/Controller.cpp \
    tools/utauloid/ust.cpp \
    editor/ui/piano.cpp \
    editor/ui/noteEditor.cpp \
    editor/ui/dynDrawer.cpp \
    editor/ui/meter.cpp \
    editor/Utils.cpp \
    editor/ui/noteEditorHandlers.cpp

HEADERS  += \
    editor/mainwindow.h \
    editor/qtsingleapplication/qtsinglecoreapplication.h \
    editor/qtsingleapplication/qtsingleapplication.h \
    editor/qtsingleapplication/qtlockedfile.h \
    editor/qtsingleapplication/qtlocalpeer.h \
    editor/PluginInterfaces.h \
    editor/audio/File.h \
    editor/Events.h \
    editor/Controller.h \
    editor/Session.h \
    tools/utauloid/ust.h \
    editor/ui/piano.h \
    editor/ui/noteEditor.h \
    editor/ui/dynDrawer.h \
    editor/ui/meter.h \
    editor/ui/Config.h \
    editor/Utils.h \
    editor/NoteEvents.h \
    editor/ui/noteEditorHandlers.h

FORMS    += editor/ui/mainwindow.ui

RESOURCES += editor/res/qtau.qrc

QMAKE_CXXFLAGS += -Wunused-parameter
