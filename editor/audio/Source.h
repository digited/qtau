#ifndef QTAU_AUDIO_SOURCE_H
#define QTAU_AUDIO_SOURCE_H

#include <QIODevice>
#include <QBuffer>
#include <QAudioBuffer>


class qtauAudioSource : public QBuffer
{
    Q_OBJECT

public:
    explicit qtauAudioSource(QObject *parent = 0);
    explicit qtauAudioSource(const QBuffer& b, const QAudioFormat &f, QObject *parent = 0);

    QAudioBuffer getAudioBuffer() { return QAudioBuffer(this->buffer(), fmt); }
    QAudioFormat getAudioFormat() { return fmt; }

    // should read all contents of file/socket and decode it to PCM in buf
    virtual bool cacheAll() { return true; }

protected:
    QAudioFormat fmt; // format of that raw PCM data

};

//qtauAudioSource genWave(qint64 lenghMs, float frequencyHz, int sampleRate, bool stereo = false, QObject *parent = 0);


#endif // QTAU_AUDIO_SOURCE_H
