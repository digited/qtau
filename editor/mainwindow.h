#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QUrl>

#include "editor/Utils.h"
#include "tools/utauloid/ust.h"

class qtauController;
class qtauSession;
class qtauEvent;
class qtauPiano;
class qtauNoteEditor;
class qtauMeterBar;
class qtauDynDrawer;
class qtauDynLabel;
class qtauWaveform;

class QScrollBar;
class QSlider;
class QToolBar;
class QTabWidget;
class QTextEdit;
class QToolBar;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool setController(qtauController &c, qtauSession &s);

    typedef enum {
        Stopped = 0,
        Playing,
        Paused,
        Repeat
    } EPlayState;

private:
    Ui::MainWindow *ui;

signals:
    void loadUST(QString fileName);
    void saveUST(QString fileName, bool rewrite);

    void loadAudio(QString fileName);

public slots:
    void onLog(const QString&, int);
    void onOpenUST();
    void onSaveUST();
    void onSaveUSTAs();

    void onVocalAudioChanged();
    void onMusicAudioChanged();

    void notesVScrolled(int);
    void notesHScrolled(int);
    void vertScrolled(int);
    void horzScrolled(int);

    void onEditorRMBScrolled(QPoint, QPoint);
    void onEditorRequestOffset(QPoint);

    void onPianoHeightChanged(int newHeight);
    void onNoteEditorWidthChanged(int newWidth);

    void onQuit();

    void onPlay(qint64 offset);
    void onPause();
    void onStop();

    void onUndo();
    void onRedo();
    void onDelete();

    void onEditMode(bool);
    void onGridSnap(bool);
    void onQuantizeSelected(int);
    void onNotelengthSelected(int);

    void dynBtnLClicked();
    void dynBtnRClicked();

    void onTabSelected(int);
    void onZoomed(int);
    void onEditorZoomed(int);

    void onDocReloaded();
    void onDocStatus(bool);
    void onUndoStatus(bool);
    void onRedoStatus(bool);
    void onDocEvent(qtauEvent*);

protected:
    qtauSession    *doc;
    noteSetup       ns;
    QTabWidget     *tabs;

    qtauPiano      *piano;
    qtauNoteEditor *noteEditor;
    qtauDynDrawer  *drawZone;
    qtauMeterBar   *meter;

    qtauWaveform   *vocalWave;
    qtauWaveform   *musicWave;

    QWidget        *vocalWavePanel; // used to switch its visibility, hidden by default
    QWidget        *musicWavePanel;
    QWidget        *drawZonePanel;

    qtauDynLabel   *fgDynLbl, *bgDynLbl;

    QScrollBar     *hscr;
    QScrollBar     *vscr;
    QSlider        *zoom;

    QTextEdit      *logpad;

    EPlayState      playState;

    QList<QToolBar*> toolbars;
    void enableToolbars(bool enable = true);

    bool logHasErrors;
    QString docName;

    void dragEnterEvent(QDragEnterEvent *);
    void dragMoveEvent(QDragMoveEvent *);
    void dropEvent(QDropEvent *);

};

#endif // MAINWINDOW_H
