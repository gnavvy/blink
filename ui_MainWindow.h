/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QStackedWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *pagePortal;
    QPushButton *buttonStart;
    QPushButton *buttonEnd;
    QWidget *widget;
    QWidget *pageContent;

    void setupUi(QStackedWidget *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1440, 800);
        pagePortal = new QWidget();
        pagePortal->setObjectName(QString::fromUtf8("pagePortal"));
        buttonStart = new QPushButton(pagePortal);
        buttonStart->setObjectName(QString::fromUtf8("buttonStart"));
        buttonStart->setGeometry(QRect(20, 10, 114, 32));
        buttonEnd = new QPushButton(pagePortal);
        buttonEnd->setObjectName(QString::fromUtf8("buttonEnd"));
        buttonEnd->setGeometry(QRect(20, 40, 114, 32));
        widget = new QWidget(pagePortal);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(440, 500, 120, 80));
        MainWindow->addWidget(pagePortal);
        pageContent = new QWidget();
        pageContent->setObjectName(QString::fromUtf8("pageContent"));
        MainWindow->addWidget(pageContent);

        retranslateUi(MainWindow);

        MainWindow->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QStackedWidget *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "StackedWidget", 0, QApplication::UnicodeUTF8));
        buttonStart->setText(QApplication::translate("MainWindow", "Start", 0, QApplication::UnicodeUTF8));
        buttonEnd->setText(QApplication::translate("MainWindow", "End", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
