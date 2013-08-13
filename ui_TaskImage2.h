/********************************************************************************
** Form generated from reading UI file 'TaskImage2.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TASKIMAGE2_H
#define UI_TASKIMAGE2_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TaskImage2
{
public:
    QLabel *image;
    QLabel *title;
    QLabel *stats;
    QTextEdit *textEdit;
    QPushButton *pushButton;

    void setupUi(QWidget *TaskImage2)
    {
        if (TaskImage2->objectName().isEmpty())
            TaskImage2->setObjectName(QString::fromUtf8("TaskImage2"));
        TaskImage2->resize(1440, 800);
        image = new QLabel(TaskImage2);
        image->setObjectName(QString::fromUtf8("image"));
        image->setGeometry(QRect(360, 90, 720, 480));
        image->setPixmap(QPixmap(QString::fromUtf8(":/imgs/task_image_3.jpg")));
        image->setScaledContents(true);
        title = new QLabel(TaskImage2);
        title->setObjectName(QString::fromUtf8("title"));
        title->setGeometry(QRect(280, 20, 960, 40));
        QFont font;
        font.setFamily(QString::fromUtf8("Helvetica"));
        font.setPointSize(36);
        font.setBold(true);
        font.setWeight(75);
        title->setFont(font);
        title->setAutoFillBackground(false);
        title->setScaledContents(false);
        stats = new QLabel(TaskImage2);
        stats->setObjectName(QString::fromUtf8("stats"));
        stats->setGeometry(QRect(710, 580, 70, 40));
        stats->setFont(font);
        stats->setAutoFillBackground(false);
        stats->setScaledContents(false);
        textEdit = new QTextEdit(TaskImage2);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setEnabled(true);
        textEdit->setGeometry(QRect(360, 630, 640, 140));
        pushButton = new QPushButton(TaskImage2);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setEnabled(true);
        pushButton->setGeometry(QRect(1000, 630, 90, 140));

        retranslateUi(TaskImage2);

        QMetaObject::connectSlotsByName(TaskImage2);
    } // setupUi

    void retranslateUi(QWidget *TaskImage2)
    {
        TaskImage2->setWindowTitle(QApplication::translate("TaskImage2", "Form", 0, QApplication::UnicodeUTF8));
        image->setText(QString());
        title->setText(QApplication::translate("TaskImage2", "Try to memorise as many items as possible below", 0, QApplication::UnicodeUTF8));
        stats->setText(QString());
        pushButton->setText(QApplication::translate("TaskImage2", "Submit", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TaskImage2: public Ui_TaskImage2 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TASKIMAGE2_H
