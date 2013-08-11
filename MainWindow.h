#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QDebug>
#include <QTimer>
#include <QWidget>
#include <QThread>
#include <QMainWindow>
#include <QStackedWidget>

#include "Utilities.h"
#include "EyeTracker.h"
#include "FlashWidget.h"

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
    void log(QString msg) { qDebug() << msg; }
    void stimulate();
    void updateCounter();

private:
    Ui::MainWindow *ui_;

    FlashWidget *pFlashWidget_;

    EyeTracker *pTracker_;
    QTimer *pTimer_;
    QThread *pTrackerThread_;

    int interval() { return 5; }
    void setupWorkInThread();

    int counter_ = 0;
};

#endif // MAINWINDOW_H
