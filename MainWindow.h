#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>
#include <QThread>
#include <QMainWindow>
#include <QDebug>
#include <QGraphicsBlurEffect>
#include <QtDeclarative/QDeclarativeView>

#include "EyeTracker.h"

namespace Ui {
    class MainWindow;
}

class Sleeper : public QThread {
public:
    static void usleep(unsigned long usecs) { QThread::usleep(usecs); }
    static void msleep(unsigned long msecs) { QThread::msleep(msecs); }
    static void sleep(unsigned long secs)   { QThread::sleep(secs); }
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void blinked() { resetCounter(); }
    void stimulate();
    void update();
    
private:  
    QDeclarativeView *pView_;
    EyeTracker *pTracker_;
    QTimer *pTimer_;

    int  interval() { return 5; }
    void resetCounter() { counter_ = 0; }
    void flash();
    void blur();

    int counter_ = 0;
};

#endif // MAINWINDOW_H
