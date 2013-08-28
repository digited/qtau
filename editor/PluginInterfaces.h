#ifndef PLUGININTERFACES_H
#define PLUGININTERFACES_H

#include <QtPlugin>
#include "tools/utauloid/ust.h"

class qtauAudioSource;

class ISynth
{
public:
    virtual ~ISynth() {}

    virtual QString name()                          = 0;
    virtual QString description()                   = 0;
    virtual QString version()                       = 0;

    virtual bool setVoicebank(const QString&)       = 0;

    virtual bool setVocals(const ust&)              = 0;
    virtual bool setVocals(QStringList)             = 0;

    virtual bool synthesize(const qtauAudioSource&) = 0;
    virtual bool synthesize(const QString&)         = 0;

    virtual bool isVbReady()                        = 0;
    virtual bool isVocalsReady()                    = 0;

    // if synth can stream data as it's being created
    virtual bool supportsStreaming()                = 0;
};

Q_DECLARE_INTERFACE(ISynth, "qtau.ISynth/1.0")

#endif // PLUGININTERFACES_H
