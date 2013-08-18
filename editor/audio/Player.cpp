#include "editor/audio/Player.h"
#include "editor/audio/Source.h"
#include "editor/Utils.h"

#include <QAudioOutput>


qtmmPlayer::qtmmPlayer(QObject *parent) :
    QObject(parent), audioOutput(0)
{
    vsLog::d("QtMultimedia :: supported output devices and codecs:");
    QList<QAudioDeviceInfo> advs = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);

    foreach (QAudioDeviceInfo i, advs)
        vsLog::d(QString("%1 %2").arg(i.deviceName()).arg(i.supportedCodecs().join(' ')));
}

qtmmPlayer::~qtmmPlayer()
{
    //
}


bool qtmmPlayer::play(qtauAudioSource *a)
{
    bool result = false;

    if (a != 0)
    {
        if (audioOutput)
        {
            if (audioOutput->state() != QAudio::IdleState ||
                audioOutput->state() != QAudio::StoppedState)
                stop();

            delete audioOutput;
        }

        if (a->size() > 0)
        {
            QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());

            if (info.isFormatSupported(a->getAudioFormat()))
            {
                audioOutput = new QAudioOutput(a->getAudioFormat(), this);
                connect(audioOutput, SIGNAL(stateChanged(QAudio::State)), SLOT(finishedPlaying(QAudio::State)));
                audioOutput->start(a);

                result = true;
            }
            else
                qDebug() << "audio format not supported by backend, cannot play audio.";
        }
        else
            qDebug() << "no audio data to play";
    }
    else
        if (audioOutput)
        {
            if (audioOutput->state() == QAudio::SuspendedState) // continue playing
            {
                audioOutput->resume();
                result = true;
            }
            else if (audioOutput->state() == QAudio::StoppedState)
            {
                audioOutput->start();
                result = true;
            }
        }

    return result;
}

void qtmmPlayer::pause()
{
    if (audioOutput)
        audioOutput->suspend();
}

void qtmmPlayer::stop()
{
    if (audioOutput)
        audioOutput->stop();
}
