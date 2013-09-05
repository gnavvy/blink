#ifndef USERSTUDY1_H
#define USERSTUDY1_H

#include <random>
#include <algorithm>
#include <QLabel>
#include <QTimer>
#include <QWidget>
#include <QThread>
#include <QVector>
#include <QString>
#include <QDateTime>
#include <QPushButton>
#include <QGridLayout>
#include "EyeTracker.h"
#include "MaskView.h"
#include "MemTest.h"

class UserStudy1 : public QWidget {
    Q_OBJECT
public:
    explicit UserStudy1(QWidget *parent = 0);
    
signals:
    
public slots:
    void onStartButtonClicked();
    void onPauseButtonClicked();
    void onFinishButtonClicked();
    void onTaskButtonClicked();
    void onBlinkDectected();
    void onCvFrameReady(QImage img);

private:
    // -- member variable -- //
    const int FATIGUE_LIMIT = 3000;  // 3s
    const int NUM_TASKS = 6;
    const QString TEXT_ENABLE_STIMULUS = "Enable Stimulus";
    const QString TEXT_DISABLE_STIMULUS = "Disable Stimulus";
    const QString SCHEME_HTTP = "http";
    const QString SCHEME_FILE = "file";
    const QString SCHEME_TASK = "task";

    QGridLayout *gridLayout;
    QStackedWidget *contentStack;
    QStackedWidget *viewStack;

    MaskView *maskView;
    QWebView *webView;
    MemTest *memtestView;
    QLabel *cameraView;

    EyeTracker *eyeTracker;
    QThread *eyeTrackerThread;
    QDateTime timestamp;
    QDateTime timestart;

    int blinkCounter = 0;
    bool cameraViewEnabled = false;

    QVector<QPushButton*> taskButtons;
    QVector<QUrl> taskUrls;
    QPushButton *buttonStart;
    QPushButton *buttonPause;
    QPushButton *buttonFinish;
    QPushButton *buttonCurrentTask;

    // -- member function -- //
    void setupEyeTracker();
    void setupViews();
    void setupTasks();
    void outputLog(const QString &msg);
};

#endif // USERSTUDY2_H
