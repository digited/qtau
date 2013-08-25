#include "editor/ui/waveform.h"
#include "editor/audio/Source.h"

#include <qmath.h>

#include <qevent.h>
#include <QPainter>


qtauWaveform::qtauWaveform(QWidget *parent) :
    QWidget(parent), offset(0), wave(0), bgCache(0), bpm(120), beatWidth(ZOOM_NOTE_WIDTHS[DEFAULT_ZOOM_INDEX])
{
    //float noteDuration = 60.f / bpm; // 0.5 seconds each note, 4 notes (1 bar) in 2 seconds
}

qtauWaveform::~qtauWaveform()
{
    if (bgCache)
        delete bgCache;
}

inline void cycleU8(int &smpSt, int &smpEnd, float &hiVal, float &loVal, const quint8* data)
{
    for (int b = smpSt; b < smpEnd; ++b)
    {
        int iVal = (int)data[b] - 128;
        float val = (float)iVal / 127.f;

        hiVal = qMax(hiVal, val);
        loVal = qMin(loVal, val);
    }
}

inline void cycleS16(int &smpSt, int &smpEnd, float &hiVal, float &loVal, const qint16 *data)
{
    int sampleSize = 2;
    int byteSt  = smpSt  * sampleSize;
    int byteEnd = smpEnd * sampleSize;

    for (int b = byteSt; b < byteEnd; b += sampleSize)
    {
        float val = (float)data[b] / 32767.f;

        hiVal = qMax(hiVal, val);
        loVal = qMin(loVal, val);
    }
}

inline void cycleF32(int &smpSt, int &smpEnd, float &hiVal, float &loVal, const float *data)
{
    int sampleSize = 4;
    int byteSt  = smpSt  * sampleSize;
    int byteEnd = smpEnd * sampleSize;

    for (int b = byteSt; b < byteEnd; b += sampleSize)
    {
        float val = data[b];

        hiVal = qMax(hiVal, val);
        loVal = qMin(loVal, val);
    }
}

void qtauWaveform::updateCache()
{
    int requiredCacheWidth = width()/* * 2*/;

    if (bgCache && (bgCache->width() < requiredCacheWidth || bgCache->height() < height()))
    {
        delete bgCache;
        bgCache = 0;
    }

    if (!bgCache)
        bgCache = new QPixmap(requiredCacheWidth, this->rect().height());

    bgCache->fill(Qt::transparent);

    if (wave && !wave->data().isEmpty())
    {
        const QAudioFormat &fmt = wave->getAudioFormat();

        float fW = width(); // TODO: use pixmap width?
        float noteTime = 60.f / bpm; // in seconds
        float notesVisible = fW / beatWidth;
        qint64 timeVisible = notesVisible * noteTime * 1000000; // in microsec
        framesVisible = fmt.framesForDuration(timeVisible);

        int totalSamples = wave->size() * 8 / fmt.sampleSize();

        QVector<QLineF> lines;

        int smpSt = 0;
        int smpEnd = 0;

        bool noMoreSamples = false;
        const QAudioFormat::SampleType sampType = fmt.sampleType();
        float halfHeight = height() / 2;

        for (int i = 0; i < width(); ++i) // for each visible pixel of width
        {
            smpEnd = (float)(i+1) / fW * framesVisible * fmt.channelCount();

            if (smpEnd >= totalSamples)
            {
                noMoreSamples = true;
                smpEnd = totalSamples - 1;
            }

            float hiVal = -10.f;
            float loVal =  10.f;

            switch (sampType) // hoping that compiler will optimize const var + inline
            {
            case QAudioFormat::UnSignedInt: cycleU8 (smpSt, smpEnd, hiVal, loVal, (quint8*)wave->data().data());
                break;
            case QAudioFormat::SignedInt:   cycleS16(smpSt, smpEnd, hiVal, loVal, (qint16*)wave->data().data());
                break;
            case QAudioFormat::Float:       cycleF32(smpSt, smpEnd, hiVal, loVal, (float*) wave->data().data());
                break;
            default:
                vsLog::e("Waveform can't update cache because of unknown sample format of wave!");
            }

            lines.append(QLineF(i, halfHeight + hiVal * halfHeight, i, halfHeight + loVal * halfHeight));

            if (noMoreSamples) break;
            else               smpSt = smpEnd;
        }

        QPainter p(bgCache);

        QPen pen(p.pen());
        pen.setColor(Qt::green);
        pen.setWidth(1);
        p.setPen(pen);

        p.drawLines(lines);
    }

    update();
}

void qtauWaveform::configure(int tempo, int noteWidth)
{
    if (tempo != bpm || noteWidth != beatWidth)
    {
        bpm = tempo;
        beatWidth = noteWidth;
        updateCache();
    }
}

void qtauWaveform::setOffset(int off)
{
    if (offset != off)
    {
        // int oldOff = offset; use to possibly reuse an already cached data
        offset = off;

        bool cachedEnough = false;

        // TODO: see if there's enough data in pixmap to draw waveform without updating

        if (!cachedEnough)
            updateCache();
    }
}

void qtauWaveform::setAudio(qtauAudioSource *pcm)
{
    if (pcm != wave)
    {
        wave = pcm;
        updateCache();
    }
}

//---------------------------------------------------

void qtauWaveform::paintEvent(QPaintEvent  *)
{
    QPainter p(this);
    p.drawPixmap(0, 0, *bgCache);
}

void qtauWaveform::resizeEvent(QResizeEvent *)
{
    updateCache();
}

void qtauWaveform::mouseDoubleClickEvent(QMouseEvent *)
{
    //
}

void qtauWaveform::mouseMoveEvent(QMouseEvent *)
{
    //
}

void qtauWaveform::mousePressEvent(QMouseEvent *)
{
    //
}

void qtauWaveform::mouseReleaseEvent(QMouseEvent *)
{
    //
}

void qtauWaveform::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier)
        emit zoomed(e->delta());
    else
        emit scrolled(e->delta());
}
