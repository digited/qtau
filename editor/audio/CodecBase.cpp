#include "editor/audio/CodecBase.h"
#include "editor/Utils.h"


typedef struct {
    char   chunkID[4];
    qint32 chunkSize;
    char   chunkFormat[4];
} riffHeader;


qtauAudioSource* qtauWavCodec::cacheAll()
{
    // read all contents of iodevice into buffer, make format from riff header and cut it from pcm data
    qtauAudioSource *result = 0;
    bool headerOK = false;

    if (dev->bytesAvailable() > 0)
    {
        // read the RIFF header
        if (!dev->isSequential())
            dev->reset();

        riffHeader riff;
        qint64 bytesRead = dev->read((char*)&riff, sizeof(riffHeader));
        riff.chunkSize   = read32_le((quint8*)&riff.chunkSize);

        if (bytesRead == 12 && riff.chunkSize > 0 &&
            strcmp(riff.chunkID, "RIFF") == 0 && strcmp(riff.chunkFormat, "WAVE") == 0)
            headerOK = findFormatChunk() && findDataChunk();
        else
            vsLog::e("Wav codec couldn't read RIFF header");
    }
    else
        vsLog::e("Audio Wav: empty data");

    if (headerOK)
    {
        if (!dev->isSequential())
        {
            //a.data.buffer().remove(0, _data_chunk_location);
        }
    }

    return result;
}


qtauWavCodec::qtauWavCodec(QIODevice &d, QObject *parent) :
    qtauAudioCodec(d, parent)
{
    //
}


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

SampleFormat _sample_format;
int     _frames_left_in_chunk;

int _channel_count;
int _sample_rate;
int _bits_per_sample;
bool _sampleformat_unsigned;

quint64 _data_chunk_location;  // bytes
int     _data_chunk_length;    // in frames


bool qtauWavCodec::findFormatChunk()
{
    bool result = false;

    if (!dev->isSequential())
        dev->seek(12);

    // search for a format chunk
    while (true)
    {
        char    chunk_start[8];
        quint64 size =  dev->read(chunk_start, 8);
        quint32 chunk_length = read32_le((quint8*)&chunk_start[4]);

        // if we couldn't read enough, we're done
        if (size == 8)
        {
            if (memcmp(chunk_start, "fmt ", 4) == 0 && chunk_length >= 16) // found format chunk
            {
                char chunk[16];
                size = dev->read(&chunk[0], 16);

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
                        if (dev->seek(dev->pos() + chunk_length))
                        {
                            // figure out the sample format
                            if (bits_per_sample == 8 || bits_per_sample == 16)
                            {
                                if (bits_per_sample == 8)
                                {
                                    fmt.setSampleType(QAudioFormat::UnSignedInt);
                                    fmt.setSampleSize(8);
                                }
                                else
                                {
                                    fmt.setSampleType(QAudioFormat::SignedInt);
                                    fmt.setSampleSize(16);
                                }

                                // store the other important .wav attributes
                                fmt.setChannelCount(channel_count);
                                fmt.setSampleRate(samples_per_second);

                                result = true;
                                break;
                            }
                            else {
                                vsLog::e(QString("Audio Wav: wrong bits per sample %1").arg(bits_per_sample));
                                break;
                            }
                        }
                        else {
                            vsLog::e("Audio Wav: unexpected end of stream near end of reading");
                            break;
                        }
                    }
                    else {
                        vsLog::e(QString("Audio Wav: invalid format %1 %2 %3")
                                 .arg(format_tag).arg(channel_count).arg(bits_per_sample));
                        break;
                    }
                }
                else {
                    vsLog::e("Audio Wav: format chunk is too short");
                    break;
                }
            }
            else {
                if (!dev->seek(dev->pos() + chunk_length))
                {
                    vsLog::e("Audio Wav: unexpected end of stream");
                    break;
                }
            }
        }
        else {
            vsLog::e("Audio Wav: couldn't read format chunk. At all. This is BAD. Seriously.");
            break;
        }
    }

    return result;
}


bool qtauWavCodec::findDataChunk()
{
    bool result = false;

    if (!dev->isSequential())
        dev->seek(12);

    // search for a data chunk
    while (true)
    {
        char    chunk[8];
        quint64 size = dev->read(chunk, 8);
        quint32 chunk_length = read32_le((quint8*)&chunk[4]);

        if (size == 8)
        {
            if (memcmp(chunk, "data", 4) == 0) // found data chunk
            {
                // calculate the frame size so we can truncate the data chunk
                int frame_size = _channel_count * GetSampleSize(_sample_format);

                _data_chunk_location  = dev->pos();
                _data_chunk_length    = chunk_length / frame_size;
                _frames_left_in_chunk = _data_chunk_length;

                result = true;
                break;
            }
            else {
                if (!dev->seek(dev->pos() + chunk_length))
                {
                    vsLog::e("Audio Wav: unexpected end of stream");
                    break;
                }
            }
        }
        else {
            vsLog::e("Audio Wav: couldn't read chunk header");
            break;
        }
    }

    return result;
}

qtauAudioSource* qtauFlacCodec::cacheAll() { return 0; }
qtauAudioSource* qtauOggCodec::cacheAll() { return 0; }
