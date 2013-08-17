#include "waveform.h"

qtauWaveform::qtauWaveform(QWidget *parent) :
    QWidget(parent)
{
    //
}

qtauWaveform::~qtauWaveform()
{
    //
}

void qtauWaveform::updateCache()
{
    //
}

void qtauWaveform::setOffset(int off)
{
    offset = off;
}

void qtauWaveform::setPCM(qtauPCM &pcm)
{
    wave = &pcm;
}

//---------------------------------------------------

void qtauWaveform::paintEvent(QPaintEvent  *)
{
    //
}

void qtauWaveform::resizeEvent(QResizeEvent *)
{
    //
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

void qtauWaveform::wheelEvent(QWheelEvent *)
{
    //
}
