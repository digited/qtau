#include "editor/ui/waveform.h"
#include "editor/audio/Source.h"

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

inline void drawCacheFromU8(int totalSamples, float framesVisible, int channelCount,
                            const QByteArray &data, QVector<QLine> &lines)
{
    //
}

inline void drawCacheFromS16(int totalSamples, float framesVisible, int channelCount,
                             const QByteArray &data, QVector<QLine> &lines)
{
    //
}

inline void drawCacheFromFloat(int totalSamples, float framesVisible, int channelCount,
                               const QByteArray &data, QVector<QLine> &lines)
{
    //
}

void qtauWaveform::updateCache()
{
    // draw waveform to cache
    if (bgCache->width() < this->width()/* * 2*/)
    {
        delete bgCache;
        bgCache = new QPixmap(this->rect().height(), this->width()/* * 2*/);
    }

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

        QVector<QLine> lines;

        switch (fmt.sampleType())
        {
        case QAudioFormat::UnSignedInt: drawCacheFromU8   (totalSamples, framesVisible, fmt.channelCount(), wave->data(), lines);
            break;
        case QAudioFormat::SignedInt:   drawCacheFromS16  (totalSamples, framesVisible, fmt.channelCount(), wave->data(), lines);
            break;
        case QAudioFormat::Float:       drawCacheFromFloat(totalSamples, framesVisible, fmt.channelCount(), wave->data(), lines);
            break;
        default:
            vsLog::e("Waveform can't update cache because of unknown sample format of wave!");
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

void qtauWaveform::paintEvent(QPaintEvent  *e)
{
    QPainter p(this);
    p.fillRect(e->rect(), QColor(Qt::red));
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
