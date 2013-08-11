#include "mainwindow.h"

#include "QTau/Session.h"
#include "QTau/Controller.h"
#include "QTau/Events.h"
#include "QTau/PluginInterfaces.h"
#include "QTau/audio/qtmmPlayer.h"
#include "QTau/Utils.h"

#include <QApplication>
#include <QPluginLoader>


qtauController::qtauController(QObject *parent) :
    QObject(parent), audio(0), activeSession(0)
{
    events = new qtauEventManager(this);
    player = new qtmmPlayer(this);

    setupTranslations();
    setupPlugins();
    setupVoicebanks();
}

qtauController::~qtauController()
{
    delete mw;
}

//------------------------------------------

bool qtauController::run()
{
    mw = new MainWindow();
    mw->show();

    // TODO: load previous one from settings
    newEmptySession();
    mw->setController(*this, *this->activeSession);

    return true;
}


bool qtauController::setupTranslations()
{
    return false;
}


bool qtauController::setupPlugins()
{
    pluginsDir = QDir(qApp->applicationDirPath());

#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS")
    {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    pluginsDir.cd("plugins");

    vsLog::i("Searching for plugins in " + pluginsDir.absolutePath());

    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();

        if (plugin)
        {
            IResampler* rsmplr = qobject_cast<IResampler *>(plugin);

            if (rsmplr)
                initResampler(rsmplr);
        }
    }

    return false;
}


void qtauController::initResampler(IResampler *r)
{
    if (!synths.contains(r->name()))
    {
        vsLog::s("Adding resampler " + r->name());
        synths[r->name()] = r;

        // TODO: do something
    }
    else
        vsLog::d("Resampler " + r->name() + " is already registered!");
}


bool qtauController::setupVoicebanks()
{
    return false;
}

void qtauController::newEmptySession()
{
    activeSession = new qtauSession(this);
}


//------------------------------------------

void qtauController::onLoadUST(QString fileName)
{
    if (!fileName.isEmpty())
    {
        if (!activeSession)
            newEmptySession();

        activeSession->loadUST(fileName);
    }
    else
        vsLog::d("Controller: empty UST file name");
}

void qtauController::onSaveUST(QString fileName, bool rewrite)
{
    if (activeSession && !activeSession->isSessionEmpty())
    {
        QFile uf(fileName);

        if (uf.open(QFile::WriteOnly))
        {
            if (uf.size() == 0 || rewrite)
            {
                uf.seek(0);
                uf.write(activeSession->ustBinary());
                uf.close();

                activeSession->setModified(false);

                vsLog::s("UST saved to " + fileName);
            }
            else
                vsLog::e("File " + fileName + " is not empty, rewriting cancelled");
        }
        else
            vsLog::e("Could not open file " + fileName + " to save UST");
    }
    else
        vsLog::e("Trying to save ust from empty session!");
}


// TODO: temp code, rewrite properly
void qtauController::synthesize()
{
//    if (activeSession && !activeSession->isEmpty())
//    {
//        if (!synths.isEmpty())
//        {
//            IResampler *r = synths.values().first();
//            QDir teto(qApp->applicationDirPath());
//            teto.cd("../workdir");

//            if (teto.cd("vb/teto"))
//            {
//                qDebug() << "Teto found!";
//                r->setVoicebank(teto.absolutePath());
//                r->setVocals(activeSession->ustRef());

//                audio = new qtauAudio("", this);
//                r->resample(*audio);
//                player->play(audio);
//            }
//            else
//                qDebug() << "Teto not found!";
//        }
//        else
//            qDebug() << "Controller: no synthesizers registered!";
//    }
//    else
//        qDebug() << "Controller: nothing to synthesize!";
}


void qtauController::onAppMessage(const QString &msg)
{
    vsLog::i("IPC message: " + msg);
    mw->setWindowState(Qt::WindowActive); // TODO: test
}

void qtauController::pianoKeyPressed(int /*octaveNum*/, int /*keyNum*/)
{
    //qDebug() << "piano key pressed: " << octaveNum << keyNum;
}

void qtauController::pianoKeyReleased(int /*octaveNum*/, int /*keyNum*/)
{
    //qDebug() << "piano key released: " << octaveNum << keyNum;
}
