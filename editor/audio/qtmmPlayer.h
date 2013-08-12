#ifndef QTMMPLAYER_H
#define QTMMPLAYER_H

#include <QObject>
#include "editor/audio/File.h"

#if defined(Q_WS_WIN)
#include <QtMultimedia>
#else
#include <QtMultimediaKit/QAudio>
#include <QtMultimediaKit/QAudioOutput>
#endif

class qtmmPlayer : public QObject
{
    Q_OBJECT

public:
    qtmmPlayer(QObject *parent = 0);
    ~qtmmPlayer();

    bool play(qtauAudio *a = 0);
    void pause();
    void stop();

signals:
    void playbackEnded();

protected slots:
    void finishedPlaying(QAudio::State state);

protected:
    QAudioOutput *audioOutput;

};

#endif // QTMMPLAYER_H