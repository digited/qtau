#ifndef AUDIO_H
#define AUDIO_H

#include <QObject>
#include <QBuffer>
#include <QMap>

class IFormatData;


class qtauAudio : public QObject
{
    Q_OBJECT

public:
    explicit qtauAudio(QString fileName = "", QObject *parent = 0);
    ~qtauAudio();

    QMap<QString, QString> tags;

    IFormatData* format;
    QBuffer      data;

protected:
    //

};


class IFormatData
{
    friend class qtauAudio;

public:
    virtual ~IFormatData() {}

    QString ext()  { return _ext;  }
    QString mime() { return _mime; }

    QString _ext;
    QString _mime;

    int _channel_count;
    int _sample_rate;
    int _bits_per_sample;
    bool _sampleformat_unsigned;

    quint64 _data_chunk_location;  // bytes
    int     _data_chunk_length;    // in frames

protected:
    void registerFormat();

    virtual bool process     (qtauAudio& a) = 0;
    //virtual bool convertToPCM(qtauAudio &a) = 0;
};


#endif // AUDIO_H
