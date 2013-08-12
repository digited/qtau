#include "qtsingleapplication/qtsingleapplication.h"
#include "editor/Controller.h"
#include <QDebug>
#include <QIcon>


int main(int argc, char *argv[])
{
    QtSingleApplication app("QTau", argc, argv);
    //app.setQuitOnLastWindowClosed(false);

    // trying to connect to an already running instance
    if (app.isRunning())
    {
        QString paramStr("qtau_request");

        for (int i = 1; i < argc; ++i)
            paramStr.append(QString(" %1").arg(argv[i]));

        bool ok = app.sendMessage(paramStr);

        if (!ok)
            qDebug() << "Error: could not send message to an already running instance, "
                     << "it may be frozen or overloaded";

        return !ok; // 0 if ok is true
    }
    else
    {
        app.setWindowIcon(QIcon(":/images/appicon_ouka_alice.png"));
        qtauController c;
        c.run(); // create UI that can immediately call back, thus requires already created & inited controller

        // receive messages from any other future instances of this app
        QObject::connect(&app, SIGNAL(messageReceived(const QString&)), &c, SLOT(onAppMessage(const QString&)));

        return app.exec();
    }

    return -1;
}
