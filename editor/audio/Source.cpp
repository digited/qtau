#include "editor/audio/Source.h"
#include "editor/Utils.h"

qtauAudioSource::qtauAudioSource(QObject *parent) :
    QBuffer(parent)
{
}

qtauAudioSource::qtauAudioSource(const QBuffer& b, const QAudioFormat &f, QObject *parent) :
    QBuffer(parent), fmt(f)
{
    if (b.size() > 0)
    {
        open(QIODevice::WriteOnly);
        write(b.data());
        close();
    }
    else
        vsLog::d("Copying audio source with an empty buffer - what was the point of copying then?");
}

qtauAudioSource::~qtauAudioSource()
{
    if (isOpen())
        close();
}

// -------------------- generators ----------------------------

//qtauAudioSource genWave(qint64 lenghMs, float frequencyHz, int sampleRate, bool stereo, QObject *parent)
//{
//    return qtauAudioSource(parent);
//}
