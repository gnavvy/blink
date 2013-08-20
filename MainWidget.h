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
    void onFatigueTimerTimeOut();
    void onBlinkDectected();

private:
    // -- member -- //
    const int   FATIGUE_LIMIT = 3000;  // 5s

    EyeTracker *eyeTracker;
    QThread    *eyeTrackerThread;

    QGridLayout *baseLayout;
    QWebView    *webView;
    MaskView    *maskView;

    QTimer     *fatigueTimer;
    QDateTime   timestamp;

    int         blinkCounter = 0;   // -> eye tracker?
    bool        toFlash = false;
    bool        toBlur  = true;

    // -- functions -- //
    void setupEyeTracker();
    void setupViews();
    void setupTimers();

    void start();
    void stop();

    void outputLog(const QString &msg);
    
};

#endif // MAINWIDGET_H
