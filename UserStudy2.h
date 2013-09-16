#ifndef USERSTUDY2_H
#define USERSTUDY2_H

#include <QHash>
#include <QLabel>
#include <QTime>
#include <QTimer>
#include <QLabel>
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
    void onDemoButtonClicked();
    void onStopButtonClicked();
    void onFinishButtonClicked();
    void onFatigueTimerTimeOut();
    void onStimuliTimerTimeOut();
    void onTaskButtonClicked();
    void onBlinkDectected();
    void onCvFrameReady(QImage img);

protected:
    void resizeEvent(QResizeEvent *);

private:
    // -- member variable -- //
    const int kDemoTime = 15;
    const int kTestTime = 120;

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
    QVector<StimuliMode> stimuliModes;
    QVector<QString> stimuliNames;
    QTimer *stimuliTimer;
    StimuliMode currentMode;
    int modeIndex;

    int blinkCounter = 0;
    bool toFlash = false;
    bool toBlur  = true;
    bool stimulusEnabled = true;
    bool cameraViewEnabled = false;

    QPushButton *buttonStart;
    QPushButton *buttonDemo;
    QPushButton *buttonTask;
    QPushButton *buttonStop;
    QPushButton *buttonFinish;
    QLabel *debugLabel;


    // -- member function -- //
    void setupEyeTracker();
    void setupTimers();
    void setupViews();
    void setupStimulus();
    void outputLog(const QString &msg);
    int randomStimulateInterval(int min, int max);
};

#endif // USERSTUDY2_H
