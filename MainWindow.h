#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>
#include <QThread>
#include <QMainWindow>
#include <QGraphicsBlurEffect>
#include <QtDeclarative/QDeclarativeView>

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
    
private:
    QDeclarativeView *view;
    QTimer *timer;

    void flash();
    void blur();

private slots:
    void stimulate();
};

#endif // MAINWINDOW_H
