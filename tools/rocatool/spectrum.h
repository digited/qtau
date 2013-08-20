#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <QWidget>

class qtauAudioSource;
class QPixmap;


class Spectrum : public QWidget
{
    Q_OBJECT
public:
    explicit Spectrum(QWidget *parent = 0);

    void loadWav(qtauAudioSource &a);
    
signals:
    void audioDropped(QString fileName);
    
public slots:
    //

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);

    void dragEnterEvent(QDragEnterEvent *);
    void dragMoveEvent(QDragMoveEvent *);
    void dropEvent(QDropEvent *);

    qtauAudioSource *audio;
    QPixmap *spectrumCache;

    void cacheSpectrum();
    
};

#endif // SPECTRUM_H
