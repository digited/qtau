#ifndef PLUGININTERFACES_H
#define PLUGININTERFACES_H

#include <QtPlugin>
#include "tools/utauloid/ust.h"

class qtauSession;
class qtauAudio;


class IResampler
{
public:
    virtual ~IResampler() {}

    virtual QString name()                  = 0;
    virtual QString description()           = 0;
    virtual QString version()               = 0;

    virtual bool setVoicebank(QString)      = 0;

    virtual bool setVocals(const ust&)      = 0;
    virtual bool setVocals(QStringList)     = 0;

    virtual bool resample(const qtauAudio&) = 0;
    virtual bool resample(QString)          = 0;

    virtual bool isVbReady()                = 0;
    virtual bool isVocalsReady()            = 0;

    virtual bool supportsStreaming()        = 0; // if resampler can synthesize wav gradually
};


Q_DECLARE_INTERFACE(IResampler, "com.qtau.awesomeness.IResampler/1.0")

#endif // PLUGININTERFACES_H
