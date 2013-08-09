#ifndef QTMMPLAYER_H
#define QTMMPLAYER_H

#include <QObject>
#include "QTau/audio/File.h"
#include <QtMultimedia>

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
