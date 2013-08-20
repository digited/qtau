#include "rocatool.h"
#include "ui_rocatool_window.h"

#include "editor/Utils.h"
#include "spectrum.h"

#include <QtWidgets/QSlider>
#include <QtWidgets/QPushButton>

#include <QUrl>
#include <QFileInfo>
#include <QFile>

#include <qevent.h>
#include <QGridLayout>

#include "editor/audio/Player.h"
#include "editor/audio/Codec.h"
#include "editor/audio/CodecBase.h"

const QString playStr = QObject::tr("Play");
const QString stopStr = QObject::tr("Stop");


RocaTool::RocaTool(QWidget *parent) :
    QMainWindow(parent), wavBefore(0), wavAfter(0), playingBefore(false), playingAfter(false),
    ui(new Ui::RocaWindow)
{
    ui->setupUi(this);
    player = new qtmmPlayer(this);
    qtauCodecRegistry::instance()->addCodec(new qtauWavCodecFactory());

    // ---------- creating UI --------------------
    QGridLayout *gl = new QGridLayout();

    before = new Spectrum(this);
    after  = new Spectrum(this);

    before->setMinimumSize(200, 200);
    after ->setMinimumSize(200, 200);

    before->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    after ->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    F0 = new QSlider(Qt::Horizontal, this);
    F1 = new QSlider(Qt::Horizontal, this);
    F2 = new QSlider(Qt::Horizontal, this);
    F3 = new QSlider(Qt::Horizontal, this);

    playBefore = new QPushButton(playStr, this);
    playAfter  = new QPushButton(playStr, this);

    playIcon = new QIcon(":/images/b_play.png");
    stopIcon = new QIcon(":/images/b_stop.png");

    playBefore->setIcon(*playIcon);
    playAfter ->setIcon(*playIcon);

    gl->addWidget(before, 0, 0, 1, 2);
    gl->addWidget(after,  0, 2, 1, 2);

    gl->addWidget(playBefore, 1, 1, 1, 1);
    gl->addWidget(playAfter,  1, 2, 1, 1);

    gl->addWidget(F0,     2, 1, 1, 2);
    gl->addWidget(F1,     3, 1, 1, 2);
    gl->addWidget(F2,     4, 1, 1, 2);
    gl->addWidget(F3,     5, 1, 1, 2);
    //--------------------------------------------

    // --------- binding widgets -----------------
    connect(before, SIGNAL(audioDropped(QString)), SLOT(onLoadWavBefore(QString)));
    connect(after,  SIGNAL(audioDropped(QString)), SLOT(onLoadWavAfter (QString)));

    connect(F0, SIGNAL(valueChanged(int)), SLOT(onF0Moved(int)));
    connect(F1, SIGNAL(valueChanged(int)), SLOT(onF1Moved(int)));
    connect(F2, SIGNAL(valueChanged(int)), SLOT(onF2Moved(int)));
    connect(F3, SIGNAL(valueChanged(int)), SLOT(onF3Moved(int)));

    connect(playBefore, SIGNAL(clicked()), SLOT(onPlayBefore()));
    connect(playAfter,  SIGNAL(clicked()), SLOT(onPlayAfter()));
    //--------------------------------------------

    ui->centralwidget->setLayout(gl);
}

RocaTool::~RocaTool()
{
    delete playIcon;
    delete stopIcon;
}

qtauAudioSource* RocaTool::loadAudio(const QString &fileName)
{
    qtauAudioSource *result = 0;

    QFileInfo fi(fileName);
    QFile f(fileName);

    if (fi.suffix() == "wav" && f.open(QFile::ReadOnly))
    {
        result = codecForExt("wav", f, this);

        if (result)
        {
            if (!result->cacheAll())
            {
                vsLog::e("Could not parse " + fileName);
                delete result;
                result = 0;
            }
        }
        else
            vsLog::e("No codec for wav");

        f.close();
    }
    else
        vsLog::e("Could not open " + fileName);

    return result;
}

void RocaTool::onLoadWavBefore(QString fileName)
{
    qtauAudioSource *audio = loadAudio(fileName);

    if (audio)
    {
        doStopBefore(); // halt thy sounds
        doStopAfter();

        wavBefore = audio;
        before->loadWav(*audio); // previous object will be deleted in spectrum here
    }
}

void RocaTool::onLoadWavAfter(QString fileName)
{
    qtauAudioSource *audio = loadAudio(fileName);

    if (audio)
    {
        doStopBefore();
        doStopAfter();

        wavAfter = audio;
        after->loadWav(*audio); // prev will be deleted here
    }
}

//----- ui callbacks ------------
void RocaTool::onF0Moved(int)
{
    //
}

void RocaTool::onF1Moved(int)

{
    //
}

void RocaTool::onF2Moved(int)
{
    //
}

void RocaTool::onF3Moved(int)
{
    //
}


//----------- playback controls ---------------------
void RocaTool::doStopBefore()
{
    player->stop();

    if (wavBefore)
        wavBefore->reset(); // QtMultimedia AudioOutput just reads, can't seek, so "stop" actually "pause"es playback

    playBefore->setIcon(*playIcon);
    playBefore->setText(playStr);

    playingBefore = false;
}

void RocaTool::doStopAfter()
{
    player->stop();

    if (wavAfter)
        wavAfter->reset();

    playAfter->setIcon(*playIcon);
    playAfter->setText(playStr);

    playingAfter = false;
}

void RocaTool::doPlayBefore()
{
    if (wavBefore)
    {
        player->play(wavBefore);
        playBefore->setIcon(*stopIcon);
        playBefore->setText(stopStr);

        playingBefore = true;
    }
}

void RocaTool::doPlayAfter()
{
    if (wavAfter)
    {
        player->play(wavAfter);
        playAfter->setIcon(*stopIcon);
        playAfter->setText(stopStr);

        playingBefore = true;
    }
}

void RocaTool::onPlayBefore()
{
    if (playingBefore)
        doStopBefore();
    else
    {
        if (playingAfter)
            doStopAfter();

        doPlayBefore();
    }
}

void RocaTool::onPlayAfter()
{
    if (playingAfter)
        doStopAfter();
    else
    {
        if (playingBefore)
            doStopBefore();

        doPlayAfter();
    }
}
