#include "editor/audio/Codec.h"
#include "editor/Utils.h"


qtauAudioCodec::qtauAudioCodec(QIODevice &d, QObject *parent) :
    qtauAudioSource(parent)
{
    dev = &d;
}

//---------------------------------------------------

qtauCodecRegistry::~qtauCodecRegistry()
{
    foreach (qtauAudioCodecFactory *f, codecsByMime.values())
        delete f;
}

qtauCodecRegistry* qtauCodecRegistry::instance()
{
    static qtauCodecRegistry *singleRegistry = 0;

    if (!singleRegistry)
        singleRegistry = new qtauCodecRegistry();

    return singleRegistry;
}

qtauAudioCodec* qtauCodecRegistry::getCodecByMime(const QString &mime, QIODevice &d, QObject *parent )
{
    qtauAudioCodec *result = 0;

    if (!mime.isEmpty() && codecsByMime.contains(mime))
        result = codecsByMime[mime]->make(d, parent);

    return result;
}

qtauAudioCodec* qtauCodecRegistry::getCodecByExt(const QString &ext, QIODevice &d, QObject *parent)
{
    qtauAudioCodec *result = 0;

    if (!ext.isEmpty() && codecsByExt.contains(ext))
        result = codecsByExt[ext]->make(d, parent);

    return result;
}

bool qtauCodecRegistry::addCodec(qtauAudioCodecFactory *factory, bool replace)
{
    bool result = false;

    if (!factory)
        vsLog::e("Trying to register codec without a factory");
    else if (!factory->mime().isEmpty())
    {
        QString mime = factory->mime();
        bool alreadyRegistered = codecsByMime.contains(mime);

        if (!alreadyRegistered || replace)
        {
            if (alreadyRegistered)
            {
                vsLog::i(QString("Replacing codec for %1").arg(mime));
                delete codecsByMime[mime];
            }
            else
                vsLog::i(QString("Setting codec for %1").arg(mime));

            codecsByMime[mime] = factory;
            codecsByExt[factory->ext()] = factory;
            result = true;
        }
        else
            vsLog::e(QString("Codec for %1 is already registered, won't replace").arg(mime));
    }
    else
        vsLog::e("Trying to register codec without MIME type!");

    return result;
}
