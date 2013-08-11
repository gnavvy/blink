#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QWidget>
#include <QThread>
#include <QDateTime>
#include <QMainWindow>
#include <QTextStream>
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
    void onBlinkDetected();
    void outputLog(const QString &msg);
    void log(QString msg) { qDebug() << msg; }
    void stimulate();
    void updateCounter();

private:
    Ui::MainWindow *ui_;
    FlashWidget *pFlashWidget_;

    QTimer *pTimer_;

    EyeTracker *pTracker_;
    QThread *pTrackerThread_;

    void setupUI();
    void setupTimer();
    void setupWorker();

    int  getInterval() { return 5; }
    void resetCounter() { counter_ = 0; }

    int counter_ = 0;
};

#endif // MAINWINDOW_H
