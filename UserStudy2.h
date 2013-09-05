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
#include "MaskView.h"
#include "EyeTracker.h"

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
    const int FATIGUE_LIMIT = 3000;
    const int NUM_TASKS = 3;

    QGridLayout *gridLayout;
    MaskView *maskView;
    QLabel *cameraView;

    EyeTracker *eyeTracker;
    QThread *eyeTrackerThread;
    QTimer *fatigueTimer;
    QDateTime timestamp;
    QDateTime timestart;

    int blinkCounter = 0;
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
    void setupTasks();
    void outputLog(const QString &msg);
};

#endif // USERSTUDY2_H
