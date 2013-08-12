#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QWidget>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
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
    void updateTimer();

protected:
//    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui_;
    FlashWidget *pFlashWidget_;

    QTimer *pTimer_;
    QDateTime startTime_;

    EyeTracker *pTracker_;
    QThread *pTrackerThread_;

    void setupUI();
    void setupTimer();
    void setupWorker();

    void start();
    void end();

    int  getInterval() { return 5; }
    void resetTimer() { timerCounter_ = 0; }

    int blinkCounter_ = 0;
    int timerCounter_ = 0;
};

#endif // MAINWINDOW_H
