#include "QTau/ui/piano.h"
#include <qevent.h>
#include <qpainter.h>
#include <QDebug>

#include "QTau/ui/Config.h"


qtauPiano::qtauPiano(QWidget *parent) :
    QWidget(parent), offset(0,0), pressedKey(0)
{
    // setup widget
    setFocusPolicy(Qt::NoFocus);
    setMouseTracking(true);

    initPiano(ns.baseOctave, ns.numOctaves);
}

void qtauPiano::initPiano(int baseOctave, int octavesNum)
{
    octaves.clear();
    pressedKey = 0;

    // make octave keyblocks from up to bottom, from last block to first
    QRect thisR(this->geometry());
    QRect r(0, 0, thisR.width()-2, ns.note.height() * 12); // magical -2, because magic
    int octNum = baseOctave + octavesNum - 1; // including base num

    for (int i = 0; i < octavesNum; ++i, --octNum)
    {
        r.moveTop(i * r.height());
        octaves.append(octave(r, octNum));
    }
}

qtauPiano::~qtauPiano()
{
    //
}

void qtauPiano::setOffset(int voff)
{
    if (offset.height() != voff)
    {
        offset.setHeight(voff);
        update();
    }
}

void qtauPiano::configure(const noteSetup &newSetup)
{
    ns = newSetup;
    initPiano(ns.baseOctave, ns.numOctaves);
    update();
}

//--------------------------------------------

void qtauPiano::paintEvent(QPaintEvent *event)
{
    QVector<QRect> whites;
    QVector<QRect> blacks;
    QVector<QRect> whLit;
    QVector<QRect> blLit;

    QRect trR(event->rect()); // translated rect
    trR.moveTo(trR.x() + offset.width(), trR.y() + offset.height());

    QRect drR; // rect with drawing coordinates, translated from virtual keys space to widget area

    // determine what octave keyblocks are in repaint rect
    foreach (const octave &o, octaves)
        if (trR.intersects(o.c))  // render whatever keys in octave are visible
        {
            // draw whites
            for (int i = 0; i < 12; ++i)
                if (o.keys[i].c.intersects(trR))
                {
                    drR = o.keys[i].c;
                    drR.moveTo(drR.x() - offset.width(), drR.y() - offset.height());

                    if (o.keys[i].state != Passive)
                    {
                        if (o.keys[i].isBlack) blLit.append(drR);
                        else                   whLit.append(drR);
                    }
                    else
                    {
                        if (o.keys[i].isBlack) blacks.append(drR);
                        else                   whites.append(drR);
                    }
                }
        }

    if (!(whites.isEmpty() && blacks.isEmpty()))
    {
        QPainter p(this);
        QPen pen = p.pen();
        pen.setStyle(Qt::SolidLine);
        pen.setColor(Qt::black);
        pen.setWidth(1);
        p.setPen(pen);

        p.setBrush(Qt::SolidPattern);

        if (!whites.isEmpty())
        {
            p.setBrush(Qt::white);
            p.drawRects(whites);
        }

        if (!whLit.isEmpty())
        {
            p.setBrush(Qt::lightGray);
            p.drawRects(whLit);
        }

        if (!blacks.isEmpty())
        {
            p.setBrush(QColor(DEFCOLOR_BLACK_KEY_BG));
            p.drawRects(blacks);
        }

        if (!blLit.isEmpty())
        {
            p.setBrush(Qt::gray);
            p.drawRects(blLit);
        }
    }
}

void qtauPiano::resizeEvent(QResizeEvent * event)
{
    for (int i = 0; i < octaves.size(); ++i)
        octaves[i].setWidth(event->size().width());

    emit heightChanged(event->size().height());
}


//------------ input handling ---------------

void qtauPiano::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPoint pos(event->pos());
    pos.setX(pos.x() + offset.width());
    pos.setY(pos.y() + offset.height());

    for (int i = 0; i < octaves.size(); ++i)
        if (octaves[i].c.contains(pos))
        {
            key *k = octaves[i].keyAt(pos);

            if (k != 0) // found pressed key
            {
                emit keyPressed (k->number / 12, k->octIndex);
                emit keyReleased(k->number / 12, k->octIndex);
            }
        }
}

void qtauPiano::mouseMoveEvent(QMouseEvent */*event*/)
{
//    QPoint pos(event->pos());
//    pos.setX(pos.x() + offset.width());
//    pos.setY(pos.y() + offset.height());

//    for (int i = 0; i < octaves.size(); ++i)
//        if (octaves[i].c.contains(pos))
//        {
//            key *k = octaves[i].keyAt(pos);

//            if (k != 0) // found pressed key
//            {
//                // TODO: hover animation?
//            }
//        }
}

void qtauPiano::mousePressEvent(QMouseEvent *event)
{
    QPoint pos(event->pos());
    pos.setX(pos.x() + offset.width());
    pos.setY(pos.y() + offset.height());

    for (int i = 0; i < octaves.size(); ++i)
        if (octaves[i].c.contains(pos))
        {
            key *k = octaves[i].keyAt(pos);

            if (k != 0) // found pressed key
            {
                // drawing it pressed
                k->state = Pressed;
                QRect keyR(k->c);
                keyR.moveTo(keyR.x() - offset.width(), keyR.y() - offset.height());
                update(keyR);

                pressedKey = k;

                emit keyPressed(k->number / 12, k->octIndex);
            }
        }
}

void qtauPiano::mouseReleaseEvent(QMouseEvent *event)
{
    key *k = 0;

    if (pressedKey != 0)
    {
        k = pressedKey;
        pressedKey = 0;
    }
    else
    {
        QPoint pos(event->pos());
        pos.setX(pos.x() + offset.width());
        pos.setY(pos.y() + offset.height());

        for (int i = 0; i < octaves.size(); ++i)
            if (octaves[i].c.contains(pos))
                k = octaves[i].keyAt(pos);
    }

    if (k != 0)
    {
        // unhold, drawing default
        k->state = Passive;
        QRect keyR(k->c);
        keyR.moveTo(keyR.x() - offset.width(), keyR.y() - offset.height());
        update(keyR);

        emit keyReleased(k->number / 12, k->octIndex);
    }
}

void qtauPiano::wheelEvent(QWheelEvent *event)
{
    emit scrolled(event->delta());
}
