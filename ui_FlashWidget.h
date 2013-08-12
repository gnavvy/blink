/********************************************************************************
** Form generated from reading UI file 'FlashWidget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FLASHWIDGET_H
#define UI_FLASHWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FlashWidget
{
public:

    void setupUi(QWidget *FlashWidget)
    {
        if (FlashWidget->objectName().isEmpty())
            FlashWidget->setObjectName(QString::fromUtf8("FlashWidget"));
        FlashWidget->resize(400, 300);

        retranslateUi(FlashWidget);

        QMetaObject::connectSlotsByName(FlashWidget);
    } // setupUi

    void retranslateUi(QWidget *FlashWidget)
    {
        FlashWidget->setWindowTitle(QApplication::translate("FlashWidget", "Form", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class FlashWidget: public Ui_FlashWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FLASHWIDGET_H
