#include "QTau/audio/File.h"
#include <qfileinfo.h>
#include <QDebug>


QMap<QString, IFormatData*> formatDataFactory;

void IFormatData::registerFormat()
{
    if (!formatDataFactory.contains(_ext))
        formatDataFactory[_ext] = this;
}

//------------------------------------------------

inline quint16 read16_le(const quint8* b) { return b[0] + (b[1] << 8); }
inline quint16 read16_be(const quint8* b) { return (b[0] << 8) + b[1]; }
inline quint32 read32_le(const quint8* b) { return read16_le(b) + (read16_le(b + 2) << 16); }
inline quint32 read32_be(const quint8* b) { return (read16_be(b) << 16) + read16_be(b + 2); }

/// Storage formats for sample data.
enum SampleFormat {
    SF_U8,  /// unsigned 8-bit integer [0,255]
    SF_S16  /// signed 16-bit integer in host endianness [-32768,32767]
};

inline int GetSampleSize(SampleFormat format)
{
    switch (format)
    {
    case SF_U8:  return 1;
    case SF_S16: return 2;
    default:     return 0;
    }
}


class qtauWavData : public IFormatData
{
protected:
    // from format chunk
    SampleFormat _sample_format;
    int     _frames_left_in_chunk;


    bool findFormatChunk(QBuffer &b)
    {
        bool result = false;
        b.seek(12);

        // search for a format chunk
        while (true)
        {
            char    chunk_start[8];
            quint64 size =  b.read(&chunk_start[0], 8);
            quint32 chunk_length = read32_le((quint8*)&chunk_start[4]);

            // if we couldn't read enough, we're done
            if (size == 8)
            {
                if (memcmp(&chunk_start[0], "fmt ", 4) == 0 && chunk_length >= 16) // found format chunk
                {
                    char chunk[16];
                    size = b.read(&chunk[0], 16);

                    // could we read the entire format chunk?
                    if (size >= 16)
                    {
                        chunk_length -= size;

                        // parse the memory into useful information
                        quint16 format_tag         = read16_le((quint8*)&chunk[0]);
                        quint16 channel_count      = read16_le((quint8*)&chunk[2]);
                        quint32 samples_per_second = read32_le((quint8*)&chunk[4]);
                        //quint32 bytes_per_second   = read32_le((quint8*)&chunk[8]);
                        //quint16 block_align        = read16_le((quint8*)&chunk[12]);
                        quint16 bits_per_sample    = read16_le((quint8*)&chunk[14]);

                        // format_tag must be 1 (WAVE_FORMAT_PCM)
                        // we only support mono and stereo
                        if (format_tag == 1 && channel_count <= 2)
                        {
                            // skip the rest of the chunk
                            if (b.seek(b.pos() + chunk_length))
                            {
                                // figure out the sample format
                                if (bits_per_sample == 8 || bits_per_sample == 16)
                                {
                                    _sample_format = (bits_per_sample == 8) ? SF_U8 : SF_S16;
                                    _bits_per_sample = bits_per_sample;
                                    _sampleformat_unsigned = bits_per_sample == 8;

                                    // store the other important .wav attributes
                                    _channel_count = channel_count;
                                    _sample_rate   = samples_per_second;

                                    result = true;
                                    break;
                                }
                                else {
                                    qDebug() << "Audio Wav: wroing bits per sample" << bits_per_sample;
                                    break;
                                }
                            }
                            else {
                                qDebug() << "Audio Wav: unexpected end of stream near end of reading";
                                break;
                            }
                        }
                        else {
                            qDebug() << "Audio Wav: invalid format" << format_tag
                                     << channel_count << bits_per_sample;
                            break;
                        }
                    }
                    else {
                        qDebug() << "Audio Wav: format chunk is too short";
                        break;
                    }
                }
                else {
                    if (!b.seek(b.pos() + chunk_length))
                    {
                        qDebug() << "Audio Wav: unexpected end of stream";
                        break;
                    }
                }
            }
            else {
                qDebug() << "Audio Wav: couldn't read format chunk. At all. This is BAD. Seriously.";
                break;
            }
        }

        return result;
    }


    bool findDataChunk(QBuffer &b)
    {
        bool result = false;
        b.seek(12);

        // search for a data chunk
        while (true)
        {
            char    chunk[8];
            quint64 size = b.read(&chunk[0], 8);
            quint32 chunk_length = read32_le((quint8*)&chunk[4]);

            if (size == 8)
            {
                if (memcmp(&chunk[0], "data", 4) == 0) // found data chunk
                {
                    // calculate the frame size so we can truncate the data chunk
                    int frame_size = _channel_count * GetSampleSize(_sample_format);

                    _data_chunk_location  = b.pos();
                    _data_chunk_length    = chunk_length / frame_size;
                    _frames_left_in_chunk = _data_chunk_length;

                    result = true;
                    break;
                }
                else {
                    if (!b.seek(b.pos() + chunk_length))
                    {
                        qDebug() << "Audio Wav: unexpected end of stream";
                        break;
                    }
                }
            }
            else {
                qDebug() << "Audio Wav: couldn't read chunk header";
                break;
            }
        }

        return result;
    }

public:
    qtauWavData() { _ext = "wav"; _mime = "audio/wav"; registerFormat(); }

    qtauWavData(const qtauWavData &other)
    {
        _ext                   = other._ext;
        _mime                  = other._mime;

        _sample_format         = other._sample_format;
        _channel_count         = other._channel_count;
        _sample_rate           = other._sample_rate;
        _bits_per_sample       = other._bits_per_sample;
        _sampleformat_unsigned = other._sampleformat_unsigned;

        _data_chunk_location   = other._data_chunk_location;
        _data_chunk_length     = other._data_chunk_length;
        _frames_left_in_chunk  = other._frames_left_in_chunk;
    }


    bool process(qtauAudio &a)
    {
        bool result = false;

        if (a.data.size() > 0)
        {
            // read the RIFF header
            a.data.seek(0);
            char riff[12];
            qint64 size = a.data.read(&riff[0], 12);

            int riff_length = read32_le((quint8*)&riff[4]);

            if (size == 12 && riff_length > 0 &&
                memcmp(&riff[0], "RIFF", 4) == 0 &&
                memcmp(&riff[8], "WAVE", 4) == 0)
                result = findFormatChunk(a.data) && findDataChunk(a.data);
            else
                qDebug() << "Audio Wav: couldn't read RIFF header";
        }
        else
            qDebug() << "Audio Wav: empty data";

        if (result)
        {
            if (a.format)
                delete a.format;

            a.format = new qtauWavData(*this);

            // removing header, leaving only uncompressed pcm data
            a.data.buffer().remove(0, _data_chunk_location);
        }

        return result;
    }
};


class qtauFlacData : public IFormatData
{
public:
    qtauFlacData() { _ext = "flac"; _mime = "audio/flac"; registerFormat(); }

    bool process(qtauAudio&)
    {
        return false;
    }
};


class qtauOggData : public IFormatData
{
public:
    qtauOggData() { _ext = "ogg"; _mime = "audio/ogg"; registerFormat(); }

    bool process(qtauAudio&)
    {
        return false;
    }
};


//-- registration ----
qtauWavData  wavData;
qtauFlacData flacData;
qtauOggData  oggData;
//--------------------


//------------------------------------------------------------

qtauAudio::qtauAudio(QString fileName, QObject *parent) :
    QObject(parent), format(0)
{
    data.open(QBuffer::ReadWrite);

    if (!fileName.isEmpty())
    {
        QFileInfo fi(fileName);

        if (fi.exists() && !fi.isDir())
        {
            QFile f(fileName);

            if (f.open(QFile::ReadOnly))
            {
                quint64 written = data.write(f.readAll());
                qDebug() << "Done reading" << written << "of" << f.size() << "bytes to audio buffer";

                f.close();

                if (fi.suffix().isEmpty())
                {
                    qDebug() << "File" << fileName << "doesn't have extension, loading raw";

                    // TODO: try to figure out what type is it?
                }
                else {
                    if (formatDataFactory.contains(fi.suffix()))
                        formatDataFactory[fi.suffix()]->process(*this);
                }
            }
            else
                qDebug() << "Could not load file" << fileName;
        }
        else
            qDebug() << "Could not find file" << fileName;
    }
}

qtauAudio::~qtauAudio()
{
    //
}

