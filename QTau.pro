#-------------------------------------------------
# https://gitorious.org/lauloid/lauloid
# 2013 Tobias Platen, digited
#-------------------------------------------------

QT += core gui network

TARGET = QTau
TEMPLATE = app

CONFIG += audio_backend_qtmm

audio_backend_vlc {
    LIBS += -lvlc -lvlccore
    SOURCES += QTau/audio/vlcPlayer.cpp
    HEADERS += QTau/audio/vlcPlayer.h
}

audio_backend_qtmm {
    QT += multimedia
    SOURCES += QTau/audio/qtmmPlayer.cpp
    HEADERS += QTau/audio/qtmmPlayer.h
}

INCLUDEPATH += tools

windows {
RC_FILE = QTau/res/qtau_win.rc
}

SOURCES += QTau/main.cpp \
    QTau/mainwindow.cpp \
    QTau/qtsingleapplication/qtsinglecoreapplication.cpp \
    QTau/qtsingleapplication/qtsingleapplication.cpp \
    QTau/qtsingleapplication/qtlockedfile_win.cpp \
    QTau/qtsingleapplication/qtlockedfile_unix.cpp \
    QTau/qtsingleapplication/qtlockedfile.cpp \
    QTau/qtsingleapplication/qtlocalpeer.cpp \
    QTau/audio/File.cpp \
    QTau/Session.cpp \
    QTau/Controller.cpp \
    tools/utauloid/ust.cpp \
    QTau/ui/piano.cpp \
    QTau/ui/noteEditor.cpp \
    QTau/ui/dynDrawer.cpp \
    QTau/ui/meter.cpp \
    QTau/Utils.cpp \
    QTau/ui/noteEditorHandlers.cpp

HEADERS  += QTau/mainwindow.h \
    QTau/qtsingleapplication/qtsinglecoreapplication.h \
    QTau/qtsingleapplication/qtsingleapplication.h \
    QTau/qtsingleapplication/qtlockedfile.h \
    QTau/qtsingleapplication/qtlocalpeer.h \
    QTau/PluginInterfaces.h \
    QTau/audio/File.h \
    QTau/Events.h \
    QTau/Controller.h \
    QTau/Session.h \
    tools/utauloid/ust.h \
    QTau/ui/piano.h \
    QTau/ui/noteEditor.h \
    QTau/ui/dynDrawer.h \
    QTau/ui/meter.h \
    QTau/ui/Config.h \
    QTau/Utils.h \
    QTau/NoteEvents.h \
    QTau/ui/noteEditorHandlers.h

FORMS    += QTau/ui/mainwindow.ui

RESOURCES += QTau/res/qtau.qrc

TRANSLATIONS = QTau/res/lauloid_de.ts QTau/res/lauloid_jp.ts
