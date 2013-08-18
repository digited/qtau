#ifndef QTAU_AUDIO_PLAYER_H
#define QTAU_AUDIO_PLAYER_H

#include <QObject>

class QAudioOutput;
class qtauAudioSource;


class qtmmPlayer : public QObject
{
    Q_OBJECT

public:
    qtmmPlayer(QObject *parent = 0);
    ~qtmmPlayer();

    bool play(qtauAudioSource *a = 0);
    void pause();
    void stop();
    void reset() { stop(); play(); }

signals:
    void playbackEnded();

protected:
    QAudioOutput *audioOutput;

};

#endif // QTAU_AUDIO_PLAYER_H
