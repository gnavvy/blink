/********************************************************************************
** Form generated from reading UI file 'TaskVideo.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TASKVIDEO_H
#define UI_TASKVIDEO_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>
#include <phonon/seekslider.h>
#include <phonon/volumeslider.h>

QT_BEGIN_NAMESPACE

class Ui_TaskVideo
{
public:
    Phonon::VolumeSlider *volumeSlider;
    Phonon::SeekSlider *seekSlider;

    void setupUi(QWidget *TaskVideo)
    {
        if (TaskVideo->objectName().isEmpty())
            TaskVideo->setObjectName(QString::fromUtf8("TaskVideo"));
        TaskVideo->resize(1440, 800);
        volumeSlider = new Phonon::VolumeSlider(TaskVideo);
        volumeSlider->setObjectName(QString::fromUtf8("volumeSlider"));
        volumeSlider->setGeometry(QRect(240, 750, 180, 40));
        seekSlider = new Phonon::SeekSlider(TaskVideo);
        seekSlider->setObjectName(QString::fromUtf8("seekSlider"));
        seekSlider->setGeometry(QRect(480, 750, 720, 40));

        retranslateUi(TaskVideo);

        QMetaObject::connectSlotsByName(TaskVideo);
    } // setupUi

    void retranslateUi(QWidget *TaskVideo)
    {
        TaskVideo->setWindowTitle(QApplication::translate("TaskVideo", "Form", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TaskVideo: public Ui_TaskVideo {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TASKVIDEO_H
