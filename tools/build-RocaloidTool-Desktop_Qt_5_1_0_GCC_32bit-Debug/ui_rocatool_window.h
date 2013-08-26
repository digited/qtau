/********************************************************************************
** Form generated from reading UI file 'rocatool_window.ui'
**
** Created by: Qt User Interface Compiler version 5.1.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ROCATOOL_WINDOW_H
#define UI_ROCATOOL_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RocaWindow
{
public:
    QAction *actionQuit;
    QAction *actionPlay;
    QWidget *centralwidget;
    QMenuBar *menuBar;
    QMenu *menuActions;

    void setupUi(QMainWindow *RocaWindow)
    {
        if (RocaWindow->objectName().isEmpty())
            RocaWindow->setObjectName(QStringLiteral("RocaWindow"));
        RocaWindow->resize(600, 500);
        RocaWindow->setMinimumSize(QSize(600, 500));
        actionQuit = new QAction(RocaWindow);
        actionQuit->setObjectName(QStringLiteral("actionQuit"));
        actionPlay = new QAction(RocaWindow);
        actionPlay->setObjectName(QStringLiteral("actionPlay"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/b_play.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPlay->setIcon(icon);
        centralwidget = new QWidget(RocaWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        RocaWindow->setCentralWidget(centralwidget);
        menuBar = new QMenuBar(RocaWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 25));
        menuActions = new QMenu(menuBar);
        menuActions->setObjectName(QStringLiteral("menuActions"));
        RocaWindow->setMenuBar(menuBar);

        menuBar->addAction(menuActions->menuAction());
        menuActions->addAction(actionPlay);
        menuActions->addSeparator();
        menuActions->addAction(actionQuit);

        retranslateUi(RocaWindow);

        QMetaObject::connectSlotsByName(RocaWindow);
    } // setupUi

    void retranslateUi(QMainWindow *RocaWindow)
    {
        RocaWindow->setWindowTitle(QApplication::translate("RocaWindow", "Rocaloid Test Tool", 0));
        actionQuit->setText(QApplication::translate("RocaWindow", "Quit", 0));
        actionQuit->setShortcut(QApplication::translate("RocaWindow", "Ctrl+Q", 0));
        actionPlay->setText(QApplication::translate("RocaWindow", "Play", 0));
        actionPlay->setShortcut(QApplication::translate("RocaWindow", "Space", 0));
        menuActions->setTitle(QApplication::translate("RocaWindow", "Actions", 0));
    } // retranslateUi

};

namespace Ui {
    class RocaWindow: public Ui_RocaWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ROCATOOL_WINDOW_H
