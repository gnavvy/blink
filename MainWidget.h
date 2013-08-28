#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <algorithm>
#include <QWidget>
#include <QtWebKit>
#include <QMediaPlayer>
#include <QVideoWidget>
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
    void onPauseButtonClicked();
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
    const QString SCHEME_HTTP = "http";
    const QString SCHEME_FILE = "file";

    QGridLayout *baseLayout = nullptr;
    MaskView *maskView = nullptr;
    QWebView *webView = nullptr;

    EyeTracker *eyeTracker = nullptr;
    QThread *eyeTrackerThread = nullptr;
    QTimer *fatigueTimer = nullptr;
    QDateTime timestamp;
    QDateTime timestart;

    int blinkCounter = 0;
    bool toFlash = false;
    bool toBlur  = true;
    bool stimulusEnabled = false;
    bool paused = false;

    std::vector<QPushButton*> taskButtons;
    std::vector<QUrl> taskUrls;
    QPushButton *buttonStart = nullptr;
    QPushButton *buttonPause = nullptr;
    QPushButton *buttonFinish = nullptr;

    // -- functions -- //
    void setupEyeTracker();
    void setupTimers();
    void setupViews();
    void setupTasks();
    void outputLog(const QString &msg);
};

#endif // MAINWIDGET_H
