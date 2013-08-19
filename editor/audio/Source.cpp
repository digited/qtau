#include "Source.h"
#include <QDebug>

qtauAudioSource::qtauAudioSource(QObject *parent) :
    QIODevice(parent)
{
}

qtauAudioSource::qtauAudioSource(const QBuffer& b, const QAudioFormat &f, QObject *parent) :
    QIODevice(parent), fmt(f)
{
    if (b.size() > 0)
    {
        buf.open(QIODevice::WriteOnly);
        buf.write(b.data());
        buf.close();
    }
}

// -------------------- generators ----------------------------

//qtauAudioSource genWave(qint64 lenghMs, float frequencyHz, int sampleRate, bool stereo, QObject *parent)
//{
//    return qtauAudioSource(parent);
//}
