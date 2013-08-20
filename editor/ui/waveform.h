#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <QWidget>

class QPixmap;
class qtauAudioSource;

class qtauWaveform : public QWidget
{
    Q_OBJECT
public:
    explicit qtauWaveform(QWidget *parent = 0);
    ~qtauWaveform();

    void setOffset(int off);
    void setPCM(qtauAudioSource &pcm);

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
    qtauAudioSource *wave;

    QPixmap *bgCache;
    void updateCache();
};

#endif // WAVEFORM_H
