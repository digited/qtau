#ifndef QTAU_AUDIO_SOURCE_H
#define QTAU_AUDIO_SOURCE_H

#include <QIODevice>
#include <QBuffer>
#include <QAudioBuffer>

/// basic audio source is a wrapper for QBuffer, because QAudioBuffer is too limited
class qtauAudioSource : public QIODevice
{
    Q_OBJECT

public:
    explicit qtauAudioSource(QObject *parent = 0);
    explicit qtauAudioSource(const QBuffer& b, const QAudioFormat &f, QObject *parent = 0);

    bool   isSequential()   const { return false;         }
    qint64 pos()            const { return buf.pos();     }
    qint64 size()           const { return buf.size();    }
    bool   seek(qint64 pos)       { return buf.seek(pos); }
    bool   atEnd()          const { return buf.atEnd();   }
    bool   reset()                { return buf.reset();   }

    qint64 bytesAvailable() const { return buf.bytesAvailable(); }
    qint64 bytesToWrite()   const { return buf.bytesToWrite();   }

    QAudioBuffer getAudioBuffer() { return QAudioBuffer(buf.buffer(), fmt); }
    QAudioFormat getAudioFormat() { return fmt; }

    // should read all contents of file/socket and decode it to PCM in buf
    virtual qtauAudioSource* cacheAll() { return new qtauAudioSource(buf, fmt); }

public slots:
    //

protected:
    QBuffer      buf; // raw PCM data
    QAudioFormat fmt; // format of that raw PCM data

    qint64 readData (char       *data, qint64 maxSize) { return buf.read (data, maxSize); }
    qint64 writeData(const char *data, qint64 maxSize) { return buf.write(data, maxSize); }

};

//qtauAudioSource genWave(qint64 lenghMs, float frequencyHz, int sampleRate, bool stereo = false, QObject *parent = 0);


#endif // QTAU_AUDIO_SOURCE_H
