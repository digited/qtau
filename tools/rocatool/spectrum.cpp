#include "spectrum.h"
#include "editor/audio/Source.h"
#include "editor/Utils.h"

#include <QUrl>
#include <QFileInfo>
#include <QFile>
#include <qevent.h>

#include <QPainter>
#include <QPixmap>
#include <QMimeData>


Spectrum::Spectrum(QWidget *parent) :
    QWidget(parent), audio(0), spectrumCache(0)
{
    setAcceptDrops(false); // disabled for now - whole window will handle a single dropped item
}

void Spectrum::cacheSpectrum()
{
    // cache audio pcm data to pixmap for fast rendering here
}

void Spectrum::paintEvent(QPaintEvent *e)
{
    QPainter p(this);

    QBrush brush(p.brush());
    brush.setStyle(Qt::Dense5Pattern);
    brush.setColor(QColor(0xffffeab2));

    p.setBrush(brush);
    p.drawRect(e->rect());

    p.drawText(e->rect(), "Here be spectrum!", QTextOption(Qt::AlignCenter));
}

void Spectrum::resizeEvent(QResizeEvent *)
{
    cacheSpectrum();
    update();
}

void Spectrum::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasFormat("text/uri-list"))
        e->acceptProposedAction();
}

void Spectrum::dragMoveEvent(QDragMoveEvent *e)
{
    if (e->mimeData()->hasFormat("text/uri-list"))
        e->acceptProposedAction();
}

void Spectrum::dropEvent(QDropEvent *e)
{
    QList<QUrl> uris;

    foreach (const QByteArray &uriData, e->mimeData()->data("text/uri-list").split('\n'))
        if (!uriData.isEmpty())
            uris << QUrl::fromEncoded(uriData).toLocalFile().remove('\r');

    qDebug() << uris;

    QFileInfo fi(uris.first().toString());

    if (fi.exists() && !fi.isDir() && fi.suffix() == "wav") // accepting only first one, if it's a ".wav"
        emit audioDropped(fi.absoluteFilePath());
}

void Spectrum::loadWav(qtauAudioSource &a)
{
    if (audio)
        delete audio;

    audio = &a;
    cacheSpectrum();
    update();
}
