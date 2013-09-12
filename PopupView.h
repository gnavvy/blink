#ifndef POPUPVIEW_H
#define POPUPVIEW_H

#include <QLabel>
#include <QWidget>
#include <QtWidgets>
#include <QGridLayout>

class PopupView : public QWidget {
    Q_OBJECT
public:
    explicit PopupView(QWidget *parent = 0);
};

#endif // POPUPVIEW_H
