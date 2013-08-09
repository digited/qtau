#ifndef DYNDRAWER_H
#define DYNDRAWER_H

#include <QWidget>
#include <QLabel>

#include "QTau/Utils.h"

class QPixmap;


class qtauDynLabel : public QLabel
{
    Q_OBJECT

public:
    explicit qtauDynLabel(const QString& txt = "", QWidget *parent = 0);
    ~qtauDynLabel();

    typedef enum {
        off = 0,
        front,
        back
    } EState;

    EState state();
    void setState(EState s);

signals:
    void leftClicked();
    void rightClicked();

protected:
    void mousePressEvent(QMouseEvent * event);

    EState _state;

};

//----------------------------------------------

class qtauDynDrawer : public QWidget
{
    Q_OBJECT

public:
    qtauDynDrawer(QWidget *parent = 0);
    ~qtauDynDrawer();

    void setOffset(int off);
    void configure(const noteSetup &newSetup);

signals:
    void scrolled(int delta);
    void zoomed  (int delta);

public slots:
    //

protected:
    void paintEvent           (QPaintEvent  *event);
    void resizeEvent          (QResizeEvent *event);

    void mouseDoubleClickEvent(QMouseEvent  *event);
    void mouseMoveEvent       (QMouseEvent  *event);
    void mousePressEvent      (QMouseEvent  *event);
    void mouseReleaseEvent    (QMouseEvent  *event);
    void wheelEvent           (QWheelEvent  *event);

    int offset;

    noteSetup ns;

    QPixmap *bgCache;
    void updateCache();

};

#endif // DYNDRAWER_H
