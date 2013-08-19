#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QMap>
#include <QDir>

class MainWindow;
class qtauEventManager;
class qtauSynth;
class qtmmPlayer;
class qtauAudio;
class qtauSession;
class IResampler;


// main class of QTau that ties everything together
class qtauController : public QObject
{
    Q_OBJECT

public:
    explicit qtauController(QObject *parent = 0);
    ~qtauController();

    bool run(); // app startup & setup, window creation

signals:
    //

public slots:
    void onAppMessage(const QString& msg);

    void onLoadUST(QString fileName);
    void onSaveUST(QString fileName, bool rewrite);

    void onLoadAudio(QString fileName);

    void playAudio();
    void stopAudio();
    void backAudio();
    void repeatAudio();

    void pianoKeyPressed(int,int);
    void pianoKeyReleased(int,int);

protected:
    qtmmPlayer *player;
    qtauAudio  *audio;
    MainWindow *mw;
    qtauEventManager *events;

    QMap<QString, qtauSession*> sessions;
    qtauSession *activeSession;

    bool setupTranslations();
    bool setupPlugins();
    bool setupVoicebanks();

    void initResampler(IResampler *r);
    QMap<QString, IResampler*> synths;

    QDir pluginsDir;

    void newEmptySession();
};

#endif // CONTROLLER_H
