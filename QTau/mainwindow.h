#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QTau/Utils.h"
#include "tools/utauloid/ust.h"

class qtauController;
class qtauSession;
class qtauEvent;
class qtauPiano;
class qtauNoteEditor;
class qtauMeterBar;
class qtauDynDrawer;
class qtauDynLabel;

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

public slots:
    void onLog(const QString&, int);
    void onOpenUST();
    void onSaveUST();

    void notesVScrolled(int);
    void notesHScrolled(int);
    void vertScrolled(int);
    void horzScrolled(int);

    void onEditorRMBScrolled(QPoint, QPoint);
    void onEditorRequestOffset(QPoint);

    void onPianoHeightChanged(int newHeight);
    void onNoteEditorWidthChanged(int newWidth);

    void onQuit();

    void onPlay();
    void onStop();
    void onPlayReset();
    void onReplay();

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

    qtauDynLabel   *fgDynLbl, *bgDynLbl;

    QScrollBar     *hscr;
    QScrollBar     *vscr;
    QSlider        *zoom;

    QTextEdit      *logpad;

    EPlayState      playState;

    QList<QToolBar*> toolbars;
    void enableToolbars(bool enable = true);

    bool logHasErrors;

};

#endif // MAINWINDOW_H
