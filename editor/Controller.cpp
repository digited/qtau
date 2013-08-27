#include "mainwindow.h"

#include "editor/Session.h"
#include "editor/Controller.h"
#include "editor/PluginInterfaces.h"
#include "editor/Utils.h"

#include "editor/audio/Player.h"
#include "editor/audio/CodecBase.h"

#include <QtWidgets/QApplication>
#include <QPluginLoader>


qtauController::qtauController(QObject *parent) :
    QObject(parent), activeSession(0)
{
    qtauCodecRegistry::instance()->addCodec(new qtauWavCodecFactory());
    //qtauCodecRegistry::instance()->addCodec(new qtauFlacCodecFactory());
    //qtauCodecRegistry::instance()->addCodec(new qtauOggCodecFactory());

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

                activeSession->setFilePath(fileName);
                activeSession->setSaved();

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


void qtauController::onAppMessage(const QString &msg)
{
    vsLog::i("IPC message: " + msg);
    mw->setWindowState(Qt::WindowActive); // TODO: test
}

void qtauController::pianoKeyPressed(int /*octaveNum*/, int /*keyNum*/)
{
    // should play "a" of corresponding pitch
    //qDebug() << "piano key pressed: " << octaveNum << keyNum;
}

void qtauController::pianoKeyReleased(int /*octaveNum*/, int /*keyNum*/)
{
    //qDebug() << "piano key released: " << octaveNum << keyNum;
}

void qtauController::onLoadAudio(QString fileName)
{
    if (!fileName.isEmpty())
    {
        QFileInfo fi(fileName);

        if (fi.exists() && !fi.isDir() && !fi.suffix().isEmpty())
        {
            QFile f(fileName);

            if (f.open(QFile::ReadOnly))
            {
                qtauAudioCodec  *ac = codecForExt(fi.suffix(), f, this);
                bool cached = ac->cacheAll();
                f.close();

                if (cached)
                    activeSession->setBackgroundAudio(*ac);
                else
                    vsLog::e("Error caching audio");

                f.close();
            }
            else
                vsLog::e("Could not open file " + fileName);
        }
        else
            vsLog::e("Wrong file name: " + fileName);
    }
    else
        vsLog::e("Controller was requested to load audio with empty filename! Spam hatemail to admin@microsoft.com");
}

void qtauController::playAudio()
{
    if (activeSession)
    {
        if (playState.session != activeSession)
        {
            player->stop();
            playState.session = activeSession;

            qtauAudioSource *a = activeSession->getAudio();

            if (a)
            {
                playState.state = Playing;
                a->reset();
                player->play(a);
            }
        }
        else
        {
            switch (playState.state)
            {
            case Repeating:
            case Playing:  player->pause(); playState.state = Paused;  break;
            case Stopped:
            case Paused:   player->play();  playState.state = Playing; break;

            default:
                vsLog::e(QString("Unknown player state in controller!").arg(playState.state));
            }
        }
    }
    else
        vsLog::e("Someone asked controller to play something, but no session yet!");
}

void qtauController::stopAudio()
{
    player->stop();
}

void qtauController::backAudio()
{
    player->stop();

    if (playState.session && playState.session->getAudio())
    {
        qtauAudioSource *a = playState.session->getAudio();

        playState.state = Playing;
        a->reset();
        player->play(a);
    }
}

void qtauController::repeatAudio()
{
    //
}

void qtauController::onAudioPlaybackEnded()
{
    playState.state = Stopped;

    if (playState.session && playState.session->getAudio())
    {
        playState.session->getAudio()->reset();
        playState.session->onPlaybackFinished();
    }
}
