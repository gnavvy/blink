/********************************************************************************
** Form generated from reading UI file 'PortalPage.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PORTALPAGE_H
#define UI_PORTALPAGE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFormLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PortalPage
{
public:
    QWidget *formLayoutWidget;
    QFormLayout *formLayout;

    void setupUi(QWidget *PortalPage)
    {
        if (PortalPage->objectName().isEmpty())
            PortalPage->setObjectName(QString::fromUtf8("PortalPage"));
        PortalPage->resize(1440, 800);
        formLayoutWidget = new QWidget(PortalPage);
        formLayoutWidget->setObjectName(QString::fromUtf8("formLayoutWidget"));
        formLayoutWidget->setGeometry(QRect(30, 30, 601, 741));
        formLayout = new QFormLayout(formLayoutWidget);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
        formLayout->setContentsMargins(0, 0, 0, 0);

        retranslateUi(PortalPage);

        QMetaObject::connectSlotsByName(PortalPage);
    } // setupUi

    void retranslateUi(QWidget *PortalPage)
    {
        PortalPage->setWindowTitle(QApplication::translate("PortalPage", "Form", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PortalPage: public Ui_PortalPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PORTALPAGE_H
