#ifndef USERSTUDY2_H
#define USERSTUDY2_H

#include <QLabel>
#include <QTimer>
#include <QWidget>
#include <QThread>
#include <QVector>
#include <QString>
#include <QDateTime>
#include <QPushButton>
#include <QGridLayout>
#include <QWebView>
#include <QtWebKit>
#include "MaskView.h"
#include "EyeTracker.h"
#include "PopupView.h"

class UserStudy2 : public QWidget {
    Q_OBJECT
public:
    explicit UserStudy2(QWidget *parent = 0);
    
signals:
    
public slots:
    void onStartButtonClicked();
    void onPauseButtonClicked();
    void onFinishButtonClicked();
    void onFatigueTimerTimeOut();
    void onTaskButtonClicked();
    void onBlinkDectected();
    void onCvFrameReady(QImage img);

protected:
    void resizeEvent(QResizeEvent *);

private:
    // -- member variable -- //
    const int FATIGUE_LIMIT = 5000;
    const int NUM_TASKS = 1;

    QGridLayout *gridLayout;
    QStackedWidget *contentStack;

    MaskView *maskView;
    QLabel *cameraView;
    QWebView *webView;

    EyeTracker *eyeTracker;
    QThread *eyeTrackerThread;
    QTimer *fatigueTimer;
    PopupView *popupView;

    QDateTime timestamp;
    QDateTime timestart;

    enum StimuliMode { None = 0, Flash = 1, Blur = 2, Edge = 3, Popup = 4 };
    QVector<StimuliMode> stimulus;

    int blinkCounter = 0;
    int stimuliMode = 0;
    bool toFlash = false;
    bool toBlur  = true;
    bool stimulusEnabled = true;
    bool cameraViewEnabled = false;

    QVector<QPushButton*> taskButtons;
    QPushButton *buttonStart;
    QPushButton *buttonPause;
    QPushButton *buttonFinish;
    QPushButton *buttonCurrentTask;

    // -- member function -- //
    void setupEyeTracker();
    void setupTimers();
    void setupViews();
    void setupStimulus();
    void outputLog(const QString &msg);
};

#endif // USERSTUDY2_H
