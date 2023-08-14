/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *m_scene1;
    QPushButton *m_scene2;
    QPushButton *m_scene3;
    QPushButton *m_scene4;
    QStackedWidget *stackedWidget;
    QWidget *page;
    QWidget *page_2;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(723, 441);
        MainWindow->setMinimumSize(QSize(723, 441));
        MainWindow->setMaximumSize(QSize(723, 441));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        m_scene1 = new QPushButton(centralwidget);
        m_scene1->setObjectName(QString::fromUtf8("m_scene1"));
        m_scene1->setGeometry(QRect(40, 100, 93, 28));
        m_scene2 = new QPushButton(centralwidget);
        m_scene2->setObjectName(QString::fromUtf8("m_scene2"));
        m_scene2->setGeometry(QRect(180, 100, 93, 28));
        m_scene3 = new QPushButton(centralwidget);
        m_scene3->setObjectName(QString::fromUtf8("m_scene3"));
        m_scene3->setGeometry(QRect(40, 240, 93, 28));
        m_scene4 = new QPushButton(centralwidget);
        m_scene4->setObjectName(QString::fromUtf8("m_scene4"));
        m_scene4->setGeometry(QRect(180, 240, 93, 28));
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        stackedWidget->setGeometry(QRect(330, 30, 361, 361));
        page = new QWidget();
        page->setObjectName(QString::fromUtf8("page"));
        stackedWidget->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName(QString::fromUtf8("page_2"));
        stackedWidget->addWidget(page_2);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 723, 26));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        m_scene1->setText(QCoreApplication::translate("MainWindow", "PPT", nullptr));
        m_scene2->setText(QCoreApplication::translate("MainWindow", "Word", nullptr));
        m_scene3->setText(QCoreApplication::translate("MainWindow", "Excel", nullptr));
        m_scene4->setText(QCoreApplication::translate("MainWindow", "\345\234\272\346\231\2574", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
