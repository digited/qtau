#ifndef ROCATOOL_H
#define ROCATOOL_H

#include <QtWidgets/QMainWindow>

class qtauAudioSource;
class qtmmPlayer;

class Spectrum;
class QIcon;
class QSlider;
class QPushButton;


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
    void onLoadWavBefore(QString fileName);
    void onLoadWavAfter (QString fileName);

    void onF0Moved(int);
    void onF1Moved(int);
    void onF2Moved(int);
    void onF3Moved(int);

    void onPlayBefore();
    void onPlayAfter();

protected:
    Spectrum *before;
    Spectrum *after;

    qtauAudioSource *wavBefore;
    qtauAudioSource *wavAfter;

    qtmmPlayer *player;

    QSlider *F0;
    QSlider *F1;
    QSlider *F2;
    QSlider *F3;

    QPushButton *playBefore;
    QPushButton *playAfter;

    QIcon *playIcon;
    QIcon *stopIcon;

    bool playingBefore;
    bool playingAfter;

    void doStopBefore();
    void doStopAfter();
    void doPlayBefore();
    void doPlayAfter();

    qtauAudioSource* loadAudio(const QString &fileName);

private:
    Ui::RocaWindow *ui;

};

#endif // ROCATOOL_H
