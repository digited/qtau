#ifndef QTAU_AUDIO_CODECBASE_H
#define QTAU_AUDIO_CODECBASE_H

#include "editor/audio/Codec.h"


class qtauWavCodec : public qtauAudioCodec
{
    Q_OBJECT
    friend class qtauWavCodecFactory;

public:
    qtauAudioSource* cacheAll();

protected:
    qtauWavCodec(QIODevice &d, QObject *parent = 0);

    bool findFormatChunk();
    bool findDataChunk();

};

class qtauWavCodecFactory : public qtauAudioCodecFactory
{
public:
    qtauWavCodecFactory() { _ext = "wav"; _mime = "audio/wav"; }
    qtauAudioCodec* make(QIODevice &d, QObject *parent = 0) { return new qtauWavCodec(d, parent); }
};

//----------------

class qtauFlacCodec : public qtauAudioCodec
{
    Q_OBJECT
    friend class qtauFlacCodecFactory;

public:
    qtauAudioSource* cacheAll();

protected:
    qtauFlacCodec(QIODevice &d, QObject *parent = 0);

};

class qtauFlacCodecFactory : public qtauAudioCodecFactory
{
public:
    qtauFlacCodecFactory() { _ext = "flac"; _mime = "audio/flac"; }
    qtauAudioCodec* make(QIODevice &d, QObject *parent = 0) { return new qtauFlacCodec(d, parent); }
};

//----------------

class qtauOggCodec : public qtauAudioCodec
{
    Q_OBJECT
    friend class qtauOggCodecFactory;

public:
    qtauAudioSource* cacheAll();

protected:
    qtauOggCodec(QIODevice &d, QObject *parent = 0);

};

class qtauOggCodecFactory : public qtauAudioCodecFactory
{
public:
    qtauOggCodecFactory() { _ext = "ogg"; _mime = "audio/ogg"; }
    qtauAudioCodec* make(QIODevice &d, QObject *parent = 0) { return new qtauOggCodec(d, parent); }
};


#endif // QTAU_AUDIO_CODECBASE_H
