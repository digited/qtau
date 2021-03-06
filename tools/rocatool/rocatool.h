#ifndef ROCATOOL_H
#define ROCATOOL_H

#include <QtWidgets/QMainWindow>
#include "Interface.h"

class qtauAudioSource;
class qtmmPlayer;

class Spectrum;
class QIcon;
class QSlider;
class QPushButton;
class QLineEdit;


namespace Ui {
class RocaWindow;
}

class RocaTool : public QMainWindow
{
    Q_OBJECT

public:
    explicit RocaTool(QWidget *parent = 0);
    ~RocaTool();

signals:
    //

public slots:
    void onLoadWav(QString fileName);

    void onbF1(int);
    void onbF2(int);
    void onbF3(int);
    void onaF1(int);
    void onaF2(int);
    void onaF3(int);

    void onbS1(int);
    void onbS2(int);
    void onbS3(int);
    void onaS1(int);
    void onaS2(int);
    void onaS3(int);

    void bF1valEdited();
    void bF2valEdited();
    void bF3valEdited();
    void aF1valEdited();
    void aF2valEdited();
    void aF3valEdited();

    void bS1valEdited();
    void bS2valEdited();
    void bS3valEdited();
    void aS1valEdited();
    void aS2valEdited();
    void aS3valEdited();

    void onPlay();
    void onVolumeSet(int);
    void onPlaybackEnded();

protected:
    void dragEnterEvent(QDragEnterEvent *);
    void dragMoveEvent(QDragMoveEvent *);
    void dropEvent(QDropEvent *);

    Spectrum *spectrumBefore;
    Spectrum *spectrumAfter;

    qtauAudioSource *wavBefore;
    qtauAudioSource *wavAfter;

    qtmmPlayer *player;
    QSlider    *soundLevel;

    //-------- slider groups ----------
    QSlider *bF1;
    QSlider *bF2;
    QSlider *bF3;

    QSlider *bS1;
    QSlider *bS2;
    QSlider *bS3;

    QSlider *aF1;
    QSlider *aF2;
    QSlider *aF3;

    QSlider *aS1;
    QSlider *aS2;
    QSlider *aS3;

    QLineEdit *bF1val;
    QLineEdit *bF2val;
    QLineEdit *bF3val;

    QLineEdit *bS1val;
    QLineEdit *bS2val;
    QLineEdit *bS3val;

    QLineEdit *aF1val;
    QLineEdit *aF2val;
    QLineEdit *aF3val;

    QLineEdit *aS1val;
    QLineEdit *aS2val;
    QLineEdit *aS3val;
    //-------------------------------

    qtauAudioSource* loadAudio(const QString &fileName);

    void updateSpectrum1();
    void updateSpectrum2();

    bool needsSynthesis;
    bool playing;

    QByteArray spectrumDataBefore;
    QByteArray spectrumDataAfter;

private:
    Ui::RocaWindow *ui;

};

#endif // ROCATOOL_H
