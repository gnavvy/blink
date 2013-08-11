#ifndef FLASHWIDGET_H
#define FLASHWIDGET_H

#include <QWidget>
#include <QtDeclarative/QDeclarativeView>
#include "Utilities.h"

//namespace Ui {
//    class FlashWidget;
//}

class FlashWidget : public QWidget {
    Q_OBJECT
public:
    explicit FlashWidget(QWidget *parent = 0);
    ~FlashWidget();
    void Flash();
private:
    QDeclarativeView *pView_;
//    Ui::FlashWidget *ui;
};

#endif // FLASHWIDGET_H
