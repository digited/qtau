#include "Source.h"

qtauAudioSource::qtauAudioSource(QObject *parent) :
    QIODevice(parent)
{
}

qtauAudioSource::qtauAudioSource(const QBuffer& b, const QAudioFormat &f)
{
    //
}

// -------------------- generators ----------------------------

//qtauAudioSource genWave(qint64 lenghMs, float frequencyHz, int sampleRate, bool stereo, QObject *parent)
//{
//    return qtauAudioSource(parent);
//}
