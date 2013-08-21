#include "rocatool.h"
#include "ui_rocatool_window.h"

#include "editor/Utils.h"
#include "spectrum.h"

#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QToolBar>
#include <QGridLayout>

#include <QUrl>
#include <QFileInfo>
#include <QFile>
#include <QMimeData>
#include <qevent.h>

#include "editor/audio/Player.h"
#include "editor/audio/Codec.h"
#include "editor/audio/CodecBase.h"

const QString playStr = QObject::tr("Play");
const QString stopStr = QObject::tr("Stop");


inline QLabel* makeLabel(const QString &txt, Qt::Alignment align, QWidget *parent = 0)
{
    QLabel *l = new QLabel(txt, parent);
    l->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    l->setMinimumWidth(50);
    l->setAlignment(align);

    return l;
}

inline QLabel* makeLeftLabel (const QString &txt, QWidget *parent = 0) { return makeLabel(txt, Qt::AlignRight, parent); }
inline QLabel* makeRightLabel(const QString &txt, QWidget *parent = 0) { return makeLabel(txt, Qt::AlignLeft,  parent); }

inline QSlider* makeSlider(int max, QWidget *parent = 0)
{
    QSlider *s = new QSlider(Qt::Horizontal, parent);
    s->setMaximum(max);
    return s;
}

inline QSlider* makeFreqSlider    (QWidget *parent = 0) { return makeSlider(6000, parent); }
inline QSlider* makeStrengthSlider(QWidget *parent = 0) { return makeSlider(30,   parent); }

inline void makeRowOfSliders(int row, QGridLayout *gl, const QString &txt, QSlider *bS, QLabel *bL, QSlider *aS, QLabel *aL)
{
    QLabel *rl1 = makeLeftLabel(txt);
    QLabel *rl2 = makeLeftLabel(txt);

    gl->addWidget(rl1, row, 0, 1, 1);
    gl->addWidget(bS,  row, 1, 1, 1);
    gl->addWidget(bL,  row, 2, 1, 1);
    gl->addWidget(rl2, row, 3, 1, 1);
    gl->addWidget(aS,  row, 4, 1, 1);
    gl->addWidget(aL,  row, 5, 1, 1);
}


RocaTool::RocaTool(QWidget *parent) :
    QMainWindow(parent), wavBefore(0), wavAfter(0), ui(new Ui::RocaWindow)
{
    ui->setupUi(this);
    setAcceptDrops(true);
    player = new qtmmPlayer(this);
    qtauCodecRegistry::instance()->addCodec(new qtauWavCodecFactory());

    // ---------- creating UI --------------------
    QGridLayout *gl = new QGridLayout();

    spectrumBefore = new Spectrum(this);
    spectrumAfter  = new Spectrum(this);

    spectrumBefore->setMinimumSize(200, 200);
    spectrumAfter ->setMinimumSize(200, 200);

    spectrumBefore->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    spectrumAfter ->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    bF1 = makeFreqSlider(this);
    bF2 = makeFreqSlider(this);
    bF3 = makeFreqSlider(this);
    aF1 = makeFreqSlider(this);
    aF2 = makeFreqSlider(this);
    aF3 = makeFreqSlider(this);

    bS1 = makeStrengthSlider(this);
    bS2 = makeStrengthSlider(this);
    bS3 = makeStrengthSlider(this);
    aS1 = makeStrengthSlider(this);
    aS2 = makeStrengthSlider(this);
    aS3 = makeStrengthSlider(this);

    bF1val = makeRightLabel("0", this);
    bF2val = makeRightLabel("0", this);
    bF3val = makeRightLabel("0", this);
    aF1val = makeRightLabel("0", this);
    aF2val = makeRightLabel("0", this);
    aF3val = makeRightLabel("0", this);

    bS1val = makeRightLabel("0", this);
    bS2val = makeRightLabel("0", this);
    bS3val = makeRightLabel("0", this);
    aS1val = makeRightLabel("0", this);
    aS2val = makeRightLabel("0", this);
    aS3val = makeRightLabel("0", this);

    QLabel *befLbl = new QLabel("Before");
    QLabel *aftLbl = new QLabel("After");
    befLbl->setAlignment(Qt::AlignCenter);
    aftLbl->setAlignment(Qt::AlignCenter);

    gl->addWidget(befLbl, 0, 1, 1, 1);
    gl->addWidget(aftLbl,  0, 4, 1, 1);

    makeRowOfSliders(1, gl, "F1", bF1, bF1val, aF1, aF1val);
    makeRowOfSliders(2, gl, "F2", bF2, bF2val, aF2, aF2val);
    makeRowOfSliders(3, gl, "F3", bF3, bF3val, aF3, aF3val);

    makeRowOfSliders(4, gl, "S1", bS1, bS1val, aS1, aS1val);
    makeRowOfSliders(5, gl, "S2", bS2, bS2val, aS2, aS2val);
    makeRowOfSliders(6, gl, "S3", bS3, bS3val, aS3, aS3val);

    gl->addWidget(spectrumBefore, 7, 0, 1, 3);
    gl->addWidget(spectrumAfter,  7, 3, 1, 3);

    QToolBar *tb = new QToolBar(this);
    tb->addAction(ui->actionPlay);
    tb->setFloatable(false);
    addToolBar(tb);
    //--------------------------------------------

    // --------- binding widgets -----------------
    connect(ui->actionQuit, SIGNAL(triggered()), SLOT(close()));
    connect(ui->actionPlay, SIGNAL(triggered()), SLOT(onPlay()));

    connect(bF1, SIGNAL(valueChanged(int)), SLOT(onbF1(int)));
    connect(bF2, SIGNAL(valueChanged(int)), SLOT(onbF2(int)));
    connect(bF3, SIGNAL(valueChanged(int)), SLOT(onbF3(int)));

    connect(aF1, SIGNAL(valueChanged(int)), SLOT(onaF1(int)));
    connect(aF2, SIGNAL(valueChanged(int)), SLOT(onaF2(int)));
    connect(aF3, SIGNAL(valueChanged(int)), SLOT(onaF3(int)));

    connect(bS1, SIGNAL(valueChanged(int)), SLOT(onbS1(int)));
    connect(bS2, SIGNAL(valueChanged(int)), SLOT(onbS2(int)));
    connect(bS3, SIGNAL(valueChanged(int)), SLOT(onbS3(int)));

    connect(aS1, SIGNAL(valueChanged(int)), SLOT(onaS1(int)));
    connect(aS2, SIGNAL(valueChanged(int)), SLOT(onaS2(int)));
    connect(aS3, SIGNAL(valueChanged(int)), SLOT(onaS3(int)));
    //--------------------------------------------

    ui->centralwidget->setLayout(gl);
}

RocaTool::~RocaTool()
{
    //
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

void RocaTool::onLoadWav(QString fileName)
{
    qtauAudioSource *audio = loadAudio(fileName);

    if (audio)
    {
        setWindowTitle(fileName + " :: Rocaloid Test Tool");
        player->stop();

        wavBefore = audio;
        spectrumBefore->loadWav(*audio); // previous object will be deleted in spectrum here
    }
}


//----- ui callbacks ------------
// before-synth Formants
void RocaTool::onbF1(int val) { bF1val->setText(QString("%1").arg(val)); }
void RocaTool::onbF2(int val) { bF2val->setText(QString("%1").arg(val)); }
void RocaTool::onbF3(int val) { bF3val->setText(QString("%1").arg(val)); }

// after-synth Formants
void RocaTool::onaF1(int val) { aF1val->setText(QString("%1").arg(val)); }
void RocaTool::onaF2(int val) { aF2val->setText(QString("%1").arg(val)); }
void RocaTool::onaF3(int val) { aF3val->setText(QString("%1").arg(val)); }

// before-synth Strength
void RocaTool::onbS1(int val) { bS1val->setText(QString("%1").arg((float)val / 10.f)); }
void RocaTool::onbS2(int val) { bS2val->setText(QString("%1").arg((float)val / 10.f)); }
void RocaTool::onbS3(int val) { bS3val->setText(QString("%1").arg((float)val / 10.f)); }

// after-synth Strength
void RocaTool::onaS1(int val) { aS1val->setText(QString("%1").arg((float)val / 10.f)); }
void RocaTool::onaS2(int val) { aS2val->setText(QString("%1").arg((float)val / 10.f)); }
void RocaTool::onaS3(int val) { aS3val->setText(QString("%1").arg((float)val / 10.f)); }

//----------- playback controls ---------------------
void RocaTool::onPlay()
{
    player->stop();

    if (wavAfter)
        player->play(wavAfter);
    else if (wavBefore)
        player->play(wavBefore);
}

void RocaTool::synthesizeWav()
{
    //
}

void RocaTool::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasFormat("text/uri-list"))
        e->acceptProposedAction();
}

void RocaTool::dragMoveEvent(QDragMoveEvent *e)
{
    if (e->mimeData()->hasFormat("text/uri-list"))
        e->acceptProposedAction();
}

void RocaTool::dropEvent(QDropEvent *e)
{
    QList<QUrl> uris;

    foreach (const QByteArray &uriData, e->mimeData()->data("text/uri-list").split('\n'))
        if (!uriData.isEmpty())
            uris << QUrl::fromEncoded(uriData).toLocalFile().remove('\r');

    qDebug() << uris;

    QFileInfo fi(uris.first().toString());

    if (fi.exists() && !fi.isDir() && fi.suffix() == "wav") // accepting only first one, if it's a ".wav"
        onLoadWav(fi.absoluteFilePath());
}
