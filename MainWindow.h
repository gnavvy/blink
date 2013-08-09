#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>
#include <QThread>
#include <QMainWindow>
#include <QDebug>
#include <QStackedWidget>
#include <QGraphicsBlurEffect>
#include <QtDeclarative/QDeclarativeView>

#include "Utilities.h"
#include "EyeTracker.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QStackedWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void resetCounter() { counter_ = 0; }
    void stimulate();
    void update();
    
private:  
    QDeclarativeView *pView_;
    EyeTracker *pTracker_;
    QTimer *pTimer_;

    int  interval() { return 5; }
//    void resetCounter() { counter_ = 0; }
    void flash();
    void blur();

    int counter_ = 0;
};

#endif // MAINWINDOW_H
