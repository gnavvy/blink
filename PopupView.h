#ifndef POPUPVIEW_H
#define POPUPVIEW_H

#include <QTimer>
#include <QWidget>
#include <QtWidgets>

class PopupView : public QWidget {
    Q_OBJECT
public:
    explicit PopupView(QWidget *parent = 0);
    void flash();

signals:
    
public slots:
    void onTimerTimeout();

private:
    QTimer *timer;
    int switchCount;
};

#endif // POPUPVIEW_H
