#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QtWebKit>
#include "EyeTracker.h"
#include "MaskView.h"

class MainWidget : public QWidget {
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = 0);
    virtual ~MainWidget();
    
signals:
    
public slots:
    void onStartButtonClicked();
    void onFinishButtonClicked();
    void onFatigueTimerTimeOut();
    void onTaskButtonClicked();
    void onBlinkDectected();

private:
    // -- member -- //
    const int FATIGUE_LIMIT = 3000;  // 3s
    const int DEFAULT_WIDTH = 1280;
    const int DEFAULT_HEIGHT = 800;
    const int NUM_TASKS = 6;
    const QString TEXT_ENABLE_STIMULUS = "Enable Stimulus";
    const QString TEXT_DISABLE_STIMULUS = "Disable Stimulus";

    QGridLayout *baseLayout = nullptr;
    QWebView *webView = nullptr;
    MaskView *maskView = nullptr;

    EyeTracker *eyeTracker = nullptr;
    QThread *eyeTrackerThread = nullptr;
    QTimer *fatigueTimer = nullptr;
    QDateTime timestamp;
    QDateTime timestart;

    int blinkCounter = 0;
    bool toFlash = false;
    bool toBlur  = true;
    bool stimulusEnabled = false;

    std::vector<QPushButton*> taskButtons;
    std::vector<QUrl> taskUrls;
    QPushButton *buttonStart = nullptr;
    QPushButton *buttonFinish = nullptr;

    // -- functions -- //
    void setupEyeTracker();
    void setupTimers();
    void setupViews();
    void setupTasks();
    void outputLog(const QString &msg);
    
};

#endif // MAINWIDGET_H
