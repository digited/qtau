#ifndef QTAU_AUDIO_FILE_H
#define QTAU_AUDIO_FILE_H

#include "editor/audio/Source.h"
#include <QMap>

// codec is intermediate between buffered PCM data and some source of encoded audio
class qtauAudioCodec : public qtauAudioSource
{
    Q_OBJECT
    friend class qtauAudioCodecFactory;

public:
    bool isSequential() const { return dev->isSequential(); }

protected:
    QIODevice *dev;

    qtauAudioCodec(QIODevice &d, QObject *parent = 0);

};

class qtauAudioCodecFactory
{
public:
    virtual qtauAudioCodec* make(QIODevice &d, QObject *parent = 0) = 0;
    virtual ~qtauAudioCodecFactory() {}

    const QString &ext () const { return _ext;  }
    const QString &mime() const { return _mime; }

protected:
    QString _ext;
    QString _mime;
};



class qtauCodecRegistry
{
public:
    ~qtauCodecRegistry();

    static qtauCodecRegistry* instance();

    // returns new codec if factory is registered for this mime/ext, else returns 0
    qtauAudioCodec* getCodecByMime(const QString &mime, QIODevice &d, QObject *parent = 0);
    qtauAudioCodec* getCodecByExt (const QString &ext,  QIODevice &d, QObject *parent = 0);

    bool addCodec(qtauAudioCodecFactory *factory, bool replace = false);

protected:
    qtauCodecRegistry() {}
    Q_DISABLE_COPY(qtauCodecRegistry)

    QMap<QString, qtauAudioCodecFactory*> codecsByMime; // mime is primary when adding/replacing codecs
    QMap<QString, qtauAudioCodecFactory*> codecsByExt;

};


inline qtauAudioCodec* codecForMime(const QString &mime, QIODevice &d, QObject *parent = 0)
{
    return qtauCodecRegistry::instance()->getCodecByMime(mime, d, parent);
}

inline qtauAudioCodec* codecForExt(const QString &ext, QIODevice &d, QObject *parent = 0)
{
    return qtauCodecRegistry::instance()->getCodecByExt(ext, d, parent);
}

#endif // QTAU_AUDIO_FILE_H
