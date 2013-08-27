#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtGui>
#include <QtCore>
#include <QIcon>

#include "editor/Controller.h"
#include "editor/Session.h"

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QComboBox>

#include <QFileDialog>

#include "editor/ui/Config.h"
#include "editor/ui/piano.h"
#include "editor/ui/dynDrawer.h"
#include "editor/ui/noteEditor.h"
#include "editor/ui/meter.h"
#include "editor/ui/waveform.h"

#include "editor/audio/Codec.h"

const int CONST_NUM_BARS            = 128; // 128 bars "is enough for everyone"

const int CONST_MIN_PIANO_WIDTH     = 110;
const int CONST_MIN_PIANO_HEIGHT    = 40;
const int CONST_METER_HEIGHT        = 20;
const int CONST_WAVEFORM_MIN_HEIGHT = 50;
const int CONST_DRAWZONE_MIN_HEIGHT = 100;
const int CONST_DYN_BUTTONS         = 10;

const QString dynLblOFFcss = QString("QLabel { color : %1; }").arg(DEFCOLOR_DYNBTN_OFF);
const QString dynLblBGcss  = QString("QLabel { color : %1; }").arg(DEFCOLOR_DYNBTN_BG);
const QString dynLblFGcss  = QString("QLabel { color : %1; background-color : %2; }")
        .arg(DEFCOLOR_DYNBTN_ON).arg(DEFCOLOR_DYNBTN_ON_BG);


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), doc(0), fgDynLbl(0), bgDynLbl(0), logHasErrors(false)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/appicon_ouka_alice.png"));
    setWindowTitle("QTau");
    setAcceptDrops(true);
    setContextMenuPolicy(Qt::NoContextMenu);

    //-----------------------------------------

    QLabel *meterLabel = new QLabel(QString("%1/%2") .arg(ns.notesInBar).arg(ns.noteLength), this);
    QLabel *tempoLabel = new QLabel(QString("%1 bpm").arg(ns.tempo),                         this);

    QHBoxLayout *bpmHBL = new QHBoxLayout();
    bpmHBL->setContentsMargins(0,0,0,0);
    bpmHBL->addSpacing(5);
    bpmHBL->addWidget(meterLabel);
    bpmHBL->addWidget(tempoLabel);
    bpmHBL->addSpacing(5);

    QFrame *tempoPanel = new QFrame(this);
    tempoPanel->setMinimumSize(CONST_MIN_PIANO_WIDTH, CONST_METER_HEIGHT);
    tempoPanel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    tempoPanel->setContentsMargins(1,0,1,1);
    tempoPanel->setFrameStyle(QFrame::Panel | QFrame::Raised);

    tempoPanel->setLayout(bpmHBL);

    meter = new qtauMeterBar(this);
    meter->setMinimumHeight(CONST_METER_HEIGHT);
    meter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    meter->setContentsMargins(0,0,0,0);

    piano = new qtauPiano(ui->centralWidget);
    piano->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    piano->setMinimumSize(CONST_MIN_PIANO_WIDTH, CONST_MIN_PIANO_HEIGHT);
    piano->setContentsMargins(0,0,0,0);

    zoom = new QSlider(Qt::Horizontal, ui->centralWidget);
    zoom->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    zoom->setRange(0, CONST_ZOOMS - 1);
    zoom->setSingleStep(1);
    zoom->setPageStep(1);
    zoom->setValue(DEFAULT_ZOOM_INDEX);
    zoom->setMinimumWidth(CONST_MIN_PIANO_WIDTH);
    zoom->setGeometry(0,0,CONST_MIN_PIANO_WIDTH,10);
    zoom->setContentsMargins(0,0,0,0);

    noteEditor = new qtauNoteEditor(ui->centralWidget);
    noteEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    noteEditor->setContentsMargins(0,0,0,0);

    hscr = new QScrollBar(Qt::Horizontal, ui->centralWidget);
    vscr = new QScrollBar(Qt::Vertical,   ui->centralWidget);

    hscr->setContentsMargins(0,0,0,0);
    vscr->setContentsMargins(0,0,0,0);
    hscr->setRange(0, ns.note.width() * ns.notesInBar * CONST_NUM_BARS);
    vscr->setRange(0, ns.note.height() * 12 * ns.numOctaves);
    hscr->setSingleStep(ns.note.width());
    vscr->setSingleStep(ns.note.height());
    hscr->setContextMenuPolicy(Qt::NoContextMenu);
    vscr->setContextMenuPolicy(Qt::NoContextMenu);

    //---- vocal and music waveform panels, hidden until synthesized (vocal wave) and/or loaded (music wave)

    QScrollBar *dummySB = new QScrollBar(this);
    dummySB->setOrientation(Qt::Vertical);
    dummySB->setRange(0,0);
    dummySB->setEnabled(false);

    QFrame *vocalControls = new QFrame(this);
    vocalControls->setContentsMargins(0,0,0,0);
    vocalControls->setMinimumSize(CONST_MIN_PIANO_WIDTH, CONST_WAVEFORM_MIN_HEIGHT);
    vocalControls->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    vocalControls->setFrameStyle(QFrame::Panel | QFrame::Raised);

    vocalWave = new qtauWaveform(this);
    vocalWave->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    vocalWave->setMinimumHeight(CONST_WAVEFORM_MIN_HEIGHT);
    vocalWave->setContentsMargins(0,0,0,0);

    QHBoxLayout *vocalWaveL = new QHBoxLayout();
    vocalWaveL->setContentsMargins(0,0,0,0);
    vocalWaveL->setSpacing(0);
    vocalWaveL->addWidget(vocalControls);
    vocalWaveL->addWidget(vocalWave);
    vocalWaveL->addWidget(dummySB);

    vocalWavePanel = new QWidget(this);
    vocalWavePanel->setContentsMargins(0,0,0,0);
    vocalWavePanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    vocalWavePanel->setLayout(vocalWaveL);
    vocalWavePanel->setVisible(false);

    //---------

    QScrollBar *dummySB2 = new QScrollBar(this);
    dummySB2->setOrientation(Qt::Vertical);
    dummySB2->setRange(0,0);
    dummySB2->setEnabled(false);

    QFrame *musicControls = new QFrame(this);
    musicControls->setContentsMargins(0,0,0,0);
    musicControls->setMinimumSize(CONST_MIN_PIANO_WIDTH, CONST_WAVEFORM_MIN_HEIGHT);
    musicControls->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    musicControls->setFrameStyle(QFrame::Panel | QFrame::Raised);

    musicWave = new qtauWaveform(this);
    musicWave->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    musicWave->setMinimumHeight(CONST_WAVEFORM_MIN_HEIGHT);
    musicWave->setContentsMargins(0,0,0,0);

    QHBoxLayout *musicWaveL = new QHBoxLayout();
    musicWaveL->setContentsMargins(0,0,0,0);
    musicWaveL->setSpacing(0);
    musicWaveL->addWidget(musicControls);
    musicWaveL->addWidget(musicWave);
    musicWaveL->addWidget(dummySB2);

    musicWavePanel = new QWidget(this);
    musicWavePanel->setContentsMargins(0,0,0,0);
    musicWavePanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    musicWavePanel->setLayout(musicWaveL);
    musicWavePanel->setVisible(false);

    //---- notes' dynamics setup area --------------

    QGridLayout *dynBtnL = new QGridLayout();

    QString btnNames[CONST_DYN_BUTTONS] = {"VEL", "DYN", "BRE", "BRI", "CLE", "OPE", "GEN", "POR", "PIT", "PBS"};

    for (int i = 0; i < CONST_DYN_BUTTONS; ++i)
    {
        qtauDynLabel *l = new qtauDynLabel(btnNames[i], this);
        l->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        dynBtnL->addWidget(l, i / 2, i % 2, 1, 1);

        l->setStyleSheet(dynLblOFFcss);
        l->setFrameStyle(QFrame::Box);
        l->setLineWidth(1);

        connect(l, SIGNAL(leftClicked()),  SLOT(dynBtnLClicked()));
        connect(l, SIGNAL(rightClicked()), SLOT(dynBtnRClicked()));
    }

    dynBtnL->setRowStretch(CONST_DYN_BUTTONS / 2, 100);

    QFrame *dynBtnPanel = new QFrame(this);
    dynBtnPanel->setContentsMargins(0,0,0,0);
    dynBtnPanel->setMinimumSize(CONST_MIN_PIANO_WIDTH, CONST_DRAWZONE_MIN_HEIGHT);
    dynBtnPanel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    dynBtnPanel->setFrameStyle(QFrame::Panel | QFrame::Raised);

    dynBtnPanel->setLayout(dynBtnL);

    drawZone = new qtauDynDrawer(ui->centralWidget);
    drawZone->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    drawZone->setMinimumHeight(CONST_DRAWZONE_MIN_HEIGHT);
    drawZone->setContentsMargins(0,0,0,0);

    QScrollBar *dummySB3 = new QScrollBar(this);
    dummySB3->setOrientation(Qt::Vertical);
    dummySB3->setRange(0,0);
    dummySB3->setEnabled(false);

    QHBoxLayout *singParamsL = new QHBoxLayout();
    singParamsL->setContentsMargins(0,0,0,0);
    singParamsL->setSpacing(0);
    singParamsL->addWidget(dynBtnPanel);
    singParamsL->addWidget(drawZone);
    singParamsL->addWidget(dummySB3);

    drawZonePanel = new QWidget(this);
    drawZonePanel->setContentsMargins(0,0,0,0);
    drawZonePanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    drawZonePanel->setLayout(singParamsL);

    //---- Combining editor panels into hi-level layout ------

    QGridLayout *gl = new QGridLayout();
    gl->setContentsMargins(0,0,0,0);
    gl->setSpacing(0);

    gl->addWidget(tempoPanel, 0, 0, 1, 1);
    gl->addWidget(meter,      0, 1, 1, 1);
    gl->addWidget(piano,      1, 0, 1, 1);
    gl->addWidget(zoom,       2, 0, 1, 1);
    gl->addWidget(noteEditor, 1, 1, 1, 1);
    gl->addWidget(hscr,       2, 1, 1, 1);
    gl->addWidget(vscr,       1, 2, 1, 1);

    QWidget *editorUpperPanel = new QWidget(this);
    editorUpperPanel->setContentsMargins(0,0,0,0);
    editorUpperPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    editorUpperPanel->setMaximumSize(9000,9000);

    editorUpperPanel->setLayout(gl);

    QSplitter *spl = new QSplitter(Qt::Vertical, this);
    spl->setContentsMargins(0,0,0,0);
    spl->addWidget(editorUpperPanel);
    spl->addWidget(vocalWavePanel);
    spl->addWidget(musicWavePanel);
    spl->addWidget(drawZonePanel);
    spl->setStretchFactor(0, 1);
    spl->setStretchFactor(1, 0);
    spl->setStretchFactor(2, 0);
    spl->setStretchFactor(3, 0);

    QVBoxLayout *edVBL = new QVBoxLayout();
    edVBL->setContentsMargins(0,0,0,0);
    edVBL->addWidget(spl);

    QWidget *editorPanel = new QWidget(this);
    editorPanel->setContentsMargins(0,0,0,0);
    editorPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    editorPanel->setMaximumSize(9000,9000);

    editorPanel->setLayout(edVBL);

    //---- Voicebank setup tab ---------------------

    QWidget *voicesPanel = new QWidget(this);
    voicesPanel->setContentsMargins(0,0,0,0);
    voicesPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    voicesPanel->setMaximumSize(9000,9000);

    //---- Plugins setup tab -----------------------

    QWidget *pluginsPanel = new QWidget(this);
    pluginsPanel->setContentsMargins(0,0,0,0);
    pluginsPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pluginsPanel->setMaximumSize(9000,9000);

    //---- Settings tab ----------------------------

    QWidget *settingsPanel = new QWidget(this);
    settingsPanel->setContentsMargins(0,0,0,0);
    settingsPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    settingsPanel->setMaximumSize(9000,9000);

    //---- Documentation tab -----------------------

    QWidget *docsPanel = new QWidget(this);
    docsPanel->setContentsMargins(0,0,0,0);
    docsPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    docsPanel->setMaximumSize(9000,9000);

    QTextEdit *docpad = new QTextEdit(this);
    docpad->setReadOnly(true);
    docpad->setUndoRedoEnabled(false);
    docpad->setContextMenuPolicy(Qt::NoContextMenu);

    QFile embeddedDocTxt(":/tr/documentation_en.txt");

    if (embeddedDocTxt.open(QFile::ReadOnly))
    {
        QTextStream ts(&embeddedDocTxt);
        ts.setAutoDetectUnicode(true);
        ts.setCodec("UTF-8");

        docpad->setText(ts.readAll());
        embeddedDocTxt.close();
    }

    QGridLayout *docL = new QGridLayout();
    docL->setContentsMargins(0,0,0,0);
    docL->addWidget(docpad, 0, 0, 1, 1);

    docsPanel->setLayout(docL);

    //---- Log tab ---------------------------------

    QWidget *logPanel = new QWidget(this);
    logPanel->setContentsMargins(0,0,0,0);
    logPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    logPanel->setMaximumSize(9000,9000);

    logpad = new QTextEdit(this);
    logpad->setReadOnly(true);
    logpad->setUndoRedoEnabled(false);
    logpad->setContextMenuPolicy(Qt::NoContextMenu);
    logpad->setStyleSheet("p, pre { white-space: 1.2; }");

    QGridLayout *logL = new QGridLayout();
    logL->setContentsMargins(0,0,0,0);
    logL->addWidget(logpad, 0, 0, 1, 1);

    logPanel->setLayout(logL);
    connect(vsLog::instance(), SIGNAL(message(const QString&,int)), SLOT(onLog(const QString&, int)));

    //---- Combining tabs togeter ------------------

    tabs = new QTabWidget(this);
    tabs->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tabs->setContentsMargins(0,0,0,0);
    tabs->setMaximumSize(9000, 9000);
    tabs->setTabPosition(QTabWidget::South);
    tabs->setMovable(false); // just to be sure

    tabs->addTab(editorPanel,   QIcon(":/images/b_notes.png"),    tr("Editor"));
    tabs->addTab(voicesPanel,   QIcon(":/images/b_mic.png"),      tr("Voices"));
    tabs->addTab(pluginsPanel,  QIcon(":/images/b_plug.png"),     tr("Plugins"));
    tabs->addTab(settingsPanel, QIcon(":/images/b_gear.png"),     tr("Settings"));
    tabs->addTab(docsPanel,     QIcon(":/images/b_manual.png"),   tr("Documentation"));
    tabs->addTab(logPanel,      QIcon(":/images/b_envelope.png"), tr("Log"));

    tabs->widget(0)->setContentsMargins(0,0,0,0);
    tabs->widget(1)->setContentsMargins(0,0,0,0);
    tabs->widget(2)->setContentsMargins(0,0,0,0);
    tabs->widget(3)->setContentsMargins(0,0,0,0);
    tabs->widget(4)->setContentsMargins(0,0,0,0);
    tabs->widget(5)->setContentsMargins(0,0,0,0);

    connect(tabs, SIGNAL(currentChanged(int)), SLOT(onTabSelected(int)));

    QVBoxLayout *vbl = new QVBoxLayout();
    vbl->setContentsMargins(0,0,0,0);
    vbl->addWidget(tabs);
    ui->centralWidget->setContentsMargins(0,0,0,0);
    ui->centralWidget->setLayout(vbl);

    //---- Toolbars --------------------------------

    QToolBar *fileTB   = new QToolBar("Fileops",  this);
    QToolBar *playerTB = new QToolBar("Playback", this);
    QToolBar *toolsTB  = new QToolBar("Toolset",  this);

    fileTB  ->setFloatable(false);
    playerTB->setFloatable(false);
    toolsTB ->setFloatable(false);

    fileTB->addAction(ui->actionSave);
    fileTB->addAction(ui->actionUndo);
    fileTB->addAction(ui->actionRedo);

    playerTB->addAction(ui->actionPlay);
    playerTB->addAction(ui->actionStop);
    playerTB->addAction(ui->actionBack);
    playerTB->addAction(ui->actionRepeat);

    QLabel *quantizeLbl = new QLabel("  " + tr("Quantize") + ": ");
    QLabel *lengthLbl   = new QLabel("  " + tr("Length") + ": ");
    QComboBox *quantizeCombo = new QComboBox(this);
    QComboBox *lengthCombo   = new QComboBox(this);
    quantizeCombo->addItems(QStringList() << "1/4" << "1/8" << "1/16" << "1/32" << "1/64");
    lengthCombo  ->addItems(QStringList() << "1/4" << "1/8" << "1/16" << "1/32" << "1/64");
    quantizeCombo->setCurrentIndex(3);
    lengthCombo  ->setCurrentIndex(3);
    connect(quantizeCombo, SIGNAL(currentIndexChanged(int)), SLOT(onQuantizeSelected(int)));
    connect(lengthCombo,   SIGNAL(currentIndexChanged(int)), SLOT(onNotelengthSelected(int)));

    toolsTB->addAction(ui->actionEdit_Mode);
    toolsTB->addAction(ui->actionGrid_Snap);
    toolsTB->addWidget(quantizeLbl);
    toolsTB->addWidget(quantizeCombo);
    toolsTB->addWidget(lengthLbl);
    toolsTB->addWidget(lengthCombo);

    addToolBar(fileTB);
    addToolBar(playerTB);
    addToolBar(toolsTB);

    toolbars.append(fileTB);
    toolbars.append(playerTB);
    toolbars.append(toolsTB);

    //----------------------------------------------

    connect(piano,      SIGNAL(heightChanged(int)), SLOT(onPianoHeightChanged(int)));
    connect(noteEditor, SIGNAL(widthChanged(int)),  SLOT(onNoteEditorWidthChanged(int)));

    connect(meter,      SIGNAL(scrolled(int)),      SLOT(notesHScrolled(int)));
    connect(piano,      SIGNAL(scrolled(int)),      SLOT(notesVScrolled(int)));
    connect(drawZone,   SIGNAL(scrolled(int)),      SLOT(notesHScrolled(int)));
    connect(noteEditor, SIGNAL(vscrolled(int)),     SLOT(notesVScrolled(int)));
    connect(noteEditor, SIGNAL(hscrolled(int)),     SLOT(notesHScrolled(int)));
    connect(vocalWave,  SIGNAL(scrolled(int)),      SLOT(notesHScrolled(int)));
    connect(musicWave,  SIGNAL(scrolled(int)),      SLOT(notesHScrolled(int)));
    connect(vscr,       SIGNAL(valueChanged(int)),  SLOT(vertScrolled(int)));
    connect(hscr,       SIGNAL(valueChanged(int)),  SLOT(horzScrolled(int)));

    connect(noteEditor, SIGNAL(rmbScrolled(QPoint,QPoint)), SLOT(onEditorRMBScrolled(QPoint,QPoint)));
    connect(noteEditor, SIGNAL(requestsOffset(QPoint)),     SLOT(onEditorRequestOffset(QPoint)));

    connect(zoom,       SIGNAL(valueChanged(int)),  SLOT(onZoomed(int)));
    connect(meter,      SIGNAL(zoomed(int)),        SLOT(onEditorZoomed(int)));
    connect(noteEditor, SIGNAL(zoomed(int)),        SLOT(onEditorZoomed(int)));
    connect(drawZone,   SIGNAL(zoomed(int)),        SLOT(onEditorZoomed(int)));
    connect(vocalWave,  SIGNAL(zoomed(int)),        SLOT(onEditorZoomed(int)));
    connect(musicWave,  SIGNAL(zoomed(int)),        SLOT(onEditorZoomed(int)));

    connect(ui->actionQuit,    SIGNAL(triggered()), SLOT(onQuit()));
    connect(ui->actionOpen,    SIGNAL(triggered()), SLOT(onOpenUST()));
    connect(ui->actionSave,    SIGNAL(triggered()), SLOT(onSaveUST()));
    connect(ui->actionSave_as, SIGNAL(triggered()), SLOT(onSaveUSTAs()));

    connect(ui->actionUndo,    SIGNAL(triggered()), SLOT(onUndo()));
    connect(ui->actionRedo,    SIGNAL(triggered()), SLOT(onRedo()));
    connect(ui->actionDelete,  SIGNAL(triggered()), SLOT(onDelete()));

    connect(ui->actionEdit_Mode, SIGNAL(triggered(bool)), SLOT(onEditMode(bool)));
    connect(ui->actionGrid_Snap, SIGNAL(triggered(bool)), SLOT(onGridSnap(bool)));

    vsLog::instance()->enableHistory(false);
    vsLog::s(QString("Launching QTau %1 @ %2").arg(QTAU_VERSION).arg(__DATE__));

    vsLog::i("//---------------------------------------------");
    vsLog::r(); // print stored messages from program startup
    vsLog::i("//---------------------------------------------");
    vsLog::n();
}


MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::setController(qtauController &c, qtauSession &s)
{
    // NOTE: bind what uses qtauSession only here (menu/toolbar button states etc)
    doc = &s;

    connect(noteEditor, SIGNAL(editorEvent(qtauEvent*)), doc, SLOT(onUIEvent(qtauEvent*)));

    connect(doc, SIGNAL(dataReloaded()),       SLOT(onDocReloaded()));
    connect(doc, SIGNAL(modifiedStatus(bool)), SLOT(onDocStatus(bool)));
    connect(doc, SIGNAL(undoStatus(bool)),     SLOT(onUndoStatus(bool)));
    connect(doc, SIGNAL(redoStatus(bool)),     SLOT(onRedoStatus(bool)));

    connect(doc, SIGNAL(onEvent(qtauEvent*)), SLOT(onDocEvent(qtauEvent*)));

    connect(doc, SIGNAL(vocalSet()),          SLOT(onVocalAudioChanged()));
    connect(doc, SIGNAL(musicSet()),          SLOT(onMusicAudioChanged()));
    //-----------------------------------------------------------------------

    connect(this, SIGNAL(loadUST(QString)), &c, SLOT(onLoadUST(QString)));
    connect(this, SIGNAL(saveUST(QString,bool)), &c, SLOT(onSaveUST(QString,bool)));

    connect(this, SIGNAL(loadAudio(QString)), &c, SLOT(onLoadAudio(QString)));

    connect(ui->actionPlay,   SIGNAL(triggered()), &c, SLOT(playAudio()));
    connect(ui->actionStop,   SIGNAL(triggered()), &c, SLOT(stopAudio()));
    connect(ui->actionBack,   SIGNAL(triggered()), &c, SLOT(backAudio()));
    connect(ui->actionRepeat, SIGNAL(triggered()), &c, SLOT(repeatAudio()));

    //-----------------------------------------------------------------------
    connect(piano, SIGNAL(keyPressed(int,int)),  &c, SLOT(pianoKeyPressed (int,int)));
    connect(piano, SIGNAL(keyReleased(int,int)), &c, SLOT(pianoKeyReleased(int,int)));
    //-----------------------------------------------------------------------

    // widget configuration - maybe read app settings here?
    noteEditor->setRMBScrollEnabled(!ui->actionEdit_Mode->isChecked());
    noteEditor->setEditingEnabled  ( ui->actionEdit_Mode->isChecked());
    noteEditor->setFocus();

    return true;
}

void MainWindow::onOpenUST()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open UST"), "", tr("UTAU Sequence Text Files (*.ust)"));

    if (!fileName.isEmpty())
        emit loadUST(fileName);
}

void MainWindow::onSaveUST()
{
    if (doc->documentFile().isEmpty())
        onSaveUSTAs();
    else
        emit saveUST(doc->documentFile(), true);
}

void MainWindow::onSaveUSTAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save UST"), "", tr("UTAU Sequence Text Files (*.ust)"));

    if (!fileName.isEmpty())
        emit saveUST(fileName, true);
}

void MainWindow::notesVScrolled(int delta)
{
    if (delta > 0 && vscr->value() > 0) // scroll up
        delta = -ns.note.height();
    else if (delta < 0 && vscr->value() < vscr->maximum()) // scroll down
        delta = ns.note.height();
    else
        delta = 0;

    if (delta != 0)
        vscr->setValue(vscr->value() + delta);
}

void MainWindow::notesHScrolled(int delta)
{
    if (delta > 0 && hscr->value() > 0) // scroll left
        delta = -ns.note.width();
    else if (delta < 0 && hscr->value() < hscr->maximum()) // scroll right
        delta = ns.note.width();
    else
        delta = 0;

    if (delta != 0)
        hscr->setValue(hscr->value() + delta);
}

void MainWindow::vertScrolled(int delta)
{
    piano->setOffset(delta);
    noteEditor->setVOffset(delta);
}

void MainWindow::horzScrolled(int delta)
{
    noteEditor->setHOffset(delta);
    meter     ->setOffset (delta);
    drawZone  ->setOffset (delta);

    if (vocalWavePanel->isVisible())
        vocalWave->setOffset(delta);

    if (musicWavePanel->isVisible())
        musicWave->setOffset(delta);
}

void MainWindow::onEditorRMBScrolled(QPoint mouseDelta, QPoint origOffset)
{
    // moving editor space in reverse of mouse delta
    int hOff = qMax(qMin(origOffset.x() - mouseDelta.x(), hscr->maximum()), 0);
    int vOff = qMax(qMin(origOffset.y() - mouseDelta.y(), vscr->maximum()), 0);

    hscr->setValue(hOff);
    vscr->setValue(vOff);
}

void MainWindow::onEditorRequestOffset(QPoint off)
{
    off.setX(qMax(qMin(off.x(), hscr->maximum()), 0));
    off.setY(qMax(qMin(off.y(), vscr->maximum()), 0));

    hscr->setValue(off.x());
    vscr->setValue(off.y());
}

void MainWindow::onPianoHeightChanged(int newHeight)
{
    vscr->setMaximum(ns.note.height() * 12 * ns.numOctaves - newHeight + 1);
    vscr->setPageStep(piano->geometry().height());
}

void MainWindow::onNoteEditorWidthChanged(int newWidth)
{
    hscr->setMaximum(ns.note.width() * ns.notesInBar * CONST_NUM_BARS - newWidth + 1);
    hscr->setPageStep(noteEditor->geometry().width());
}

void MainWindow::onPlay(qint64 offset)
{
    offset += 0;
    ui->actionPlay->setIcon(QIcon(":/images/b_pause.png"));
    ui->actionPlay->setText(tr("Pause"));
    playState = Playing;
}

void MainWindow::onPause()
{
    ui->actionPlay->setIcon(QIcon(":/images/b_play.png"));
    ui->actionPlay->setText(tr("Play"));
    playState = Paused;
}

void MainWindow::onStop()
{
    ui->actionPlay->setIcon(QIcon(":/images/b_play.png"));
    ui->actionPlay->setText(tr("Play"));
    playState = Stopped;
}

void MainWindow::onUndo()
{
    if (doc->canUndo())
        doc->undo();
    else
        ui->actionUndo->setEnabled(false);
}

void MainWindow::onRedo()
{
    if (doc->canRedo())
        doc->redo();
    else
        ui->actionRedo->setEnabled(false);
}

void MainWindow::onDelete()
{
    noteEditor->deleteSelected();
}

void MainWindow::onEditMode(bool toggled)
{
    noteEditor->setEditingEnabled  ( toggled);
    noteEditor->setRMBScrollEnabled(!toggled);
}

void MainWindow::onGridSnap(bool toggled)
{
    vsLog::d(QString("Grid snap is %1").arg((toggled ? "enabled" : "disabled")));
}

void MainWindow::onQuantizeSelected(int index)
{
    int newQuant = 4 * (int)(pow(2, index) + 0.001);

    if (newQuant != ns.quantize)
    {
        ns.quantize = newQuant;
        noteEditor->configure(ns);
    }
}

void MainWindow::onNotelengthSelected(int index)
{
    int newNoteLength = 4 * (int)(pow(2, index) + 0.001);

    if (newNoteLength != ns.length)
    {
        ns.length = newNoteLength;
        noteEditor->configure(ns);
    }
}

void MainWindow::dynBtnLClicked()
{
    qtauDynLabel* l= qobject_cast<qtauDynLabel*>(sender());

    if (l && (fgDynLbl == 0 || l != fgDynLbl))
    {
        if (fgDynLbl)
        {
            fgDynLbl->setState(qtauDynLabel::off);
            fgDynLbl->setStyleSheet(dynLblOFFcss);
        }

        if (l == bgDynLbl)
        {
            bgDynLbl->setState(qtauDynLabel::off);
            bgDynLbl->setStyleSheet(dynLblOFFcss);
            bgDynLbl = 0;
        }

        l->setStyleSheet(dynLblFGcss);
        fgDynLbl = l;
    }
}

void MainWindow::dynBtnRClicked()
{
    qtauDynLabel* l= qobject_cast<qtauDynLabel*>(sender());

    if (l)
    {
        if (bgDynLbl != 0 && l == bgDynLbl)
        {
            // clicking on same dynkey - switch it off
            bgDynLbl->setState(qtauDynLabel::off);
            bgDynLbl->setStyleSheet(dynLblOFFcss);
            bgDynLbl = 0;
        }
        else
        {   // clicking on other dynkey
            if (bgDynLbl)
            {   // switch off previous one, if any
                bgDynLbl->setState(qtauDynLabel::off);
                bgDynLbl->setStyleSheet(dynLblOFFcss);
                bgDynLbl = 0;
            }

            if (l != fgDynLbl)
            {   // clicking on not-foreground dynkey
                l->setStyleSheet(dynLblBGcss);
                bgDynLbl = l;
            }
        }
    }
}

void MainWindow::onLog(const QString &msg, int type)
{
    QString color = "black";

    switch((vsLog::msgType)type)
    {
    case vsLog::error:
        color = "red";
        tabs->setTabText(tabs->count() - 1, tr("Log") + " (!)");
        logHasErrors = true;
        break;
    case vsLog::success:
        color = "green";
        break;
    default: break;
    }

    logpad->moveCursor(QTextCursor::End);
    logpad->insertHtml(QString("<pre style=\"color: %1;\">%2</pre><p></p>").arg(color).arg(msg));
}

void MainWindow::enableToolbars(bool enable)
{
    foreach (QToolBar *t, toolbars)
        t->setVisible(enable);      //t->setEnabled(enable);
}

void MainWindow::onTabSelected(int index)
{
    enableToolbars(index == 0);

    if (index == tabs->count() - 1 && logHasErrors) // log with errors selected, revert alert style
    {
        tabs->setTabText(tabs->count() - 1, tr("Log"));
        logHasErrors = false;
    }
}

void MainWindow::onZoomed(int z)
{
    assert(z >= 0 && z < CONST_ZOOMS); // because if it isn't, you're DOOMED

    // modify note data and send it to widgets
    ns.note.setWidth(ZOOM_NOTE_WIDTHS[z]);

    meter     ->configure(ns);
    piano     ->configure(ns);
    noteEditor->configure(ns);
    drawZone  ->configure(ns);

    if (vocalWavePanel->isVisible())
        vocalWave->configure(ns.tempo, ns.note.width());

    if (musicWavePanel->isVisible())
        musicWave->configure(ns.tempo, ns.note.width());

    // modify scrollbar sizes and position
    double hscr_val = (double)hscr->value() / hscr->maximum();
    hscr->setMaximum(ns.note.width()  * ns.notesInBar * CONST_NUM_BARS - noteEditor->width() + 1);
    hscr->setValue(hscr->maximum() * hscr_val);
    hscr->setSingleStep(ns.note.width());

    horzScrolled(hscr->value());
}

void MainWindow::onEditorZoomed(int delta)
{
    if (delta != 0)
        if ((delta > 0 && zoom->value() >= 0) ||
            (delta < 0 && zoom->value() < CONST_ZOOMS))
            zoom->setValue(zoom->value() + ((delta > 0) ? 1 : -1));
}

void MainWindow::onQuit()
{
    close();
}

void MainWindow::onDocReloaded()
{
    setWindowTitle(doc->documentName() + " - QTau");
}

void MainWindow::onDocStatus(bool isModified)
{
    QString newDocName = doc->documentName();

    if (docName != newDocName)
        docName = newDocName;

    setWindowTitle((isModified ? "*" : "") + docName + " - QTau");

    ui->actionSave->setEnabled(isModified);
}

void MainWindow::onUndoStatus(bool canUndo)
{
    ui->actionUndo->setEnabled(canUndo);
}

void MainWindow::onRedoStatus(bool canRedo)
{
    ui->actionRedo->setEnabled(canRedo);
}


void MainWindow::onDocEvent(qtauEvent* event)
{
    if (event->type() >= ENoteEvents::add && event->type() <= ENoteEvents::effect)
        noteEditor->onEvent(event);
}

void MainWindow::onVocalAudioChanged()
{
    // show vocal waveform panel and send audioSource to it for generation
    vocalWavePanel->setVisible(true);
    vocalWave->setAudio(doc->getVocal());

    ui->actionPlay->setEnabled(true); // it isn't at startup because nothing to play
}

void MainWindow::onMusicAudioChanged()
{
    // show & fill music waveform panel
    musicWavePanel->setVisible(true);
    musicWave->setAudio(doc->getMusic());

    ui->actionPlay->setEnabled(true);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    // accepting filepaths
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    // accepting filepaths
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> uris;

    foreach (const QByteArray &uriData, event->mimeData()->data("text/uri-list").split('\n'))
        if (!uriData.isEmpty())
            uris << QUrl::fromEncoded(uriData).toLocalFile().remove('\r');

    if (!uris.isEmpty())
    {
        QFileInfo fi(uris.first().toString());

        if (uris.size() > 1)
            vsLog::i("Multiple URIs dropped, currently using only first one");

        if (fi.exists() && !fi.isDir() && !fi.suffix().isEmpty()) // if it's an existing file with some extension
        {
            // maybe it's a note/lyrics file? (ust/vsq/vsqx/midi)
            if (fi.suffix() == "ust") // TODO: support many, or do something like audio codecs registry
            {
                emit loadUST(fi.absoluteFilePath());
            }
            else if (isAudioExtSupported(fi.suffix())) // or maybe it's an audio? (try audio codecs)
            {
                emit loadAudio(fi.absoluteFilePath());
            }
            else
                vsLog::e("File extension not supported: " + fi.suffix());
        }
    }
}
