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

void qtauWaveform::updateCache()
{
    // draw waveform to cache
    if (bgCache->width() < this->width() * 2)
    {
        delete bgCache;
        bgCache = new QPixmap(this->rect().height(), this->width() * 2);
    }

    bgCache->fill(Qt::transparent);

    if (wave)
    {
        const QByteArray   &pcm = wave->data();
        //const QAudioFormat &fmt = wave->getAudioFormat();

        if (!pcm.isEmpty())
        {
            //qint64 framesVisible = fmt.framesForDuration(60.f / bpm);
        }
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
        offset = off;

        bool cachedEnough = false;

        //

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
