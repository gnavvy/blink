#include "MainWidget.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent) {
    setupEyeTracker();
    setupTimers();
    setupViews();
}

MainWidget::~MainWidget() {
    if (maskView)       delete maskView;
    if (webView)        delete webView;
    if (fatigueTimer)   delete fatigueTimer;
}

void MainWidget::setupEyeTracker() {
    eyeTrackerThread = new QThread;
    eyeTracker       = new EyeTracker;
    eyeTracker->moveToThread(eyeTrackerThread);
    connect(eyeTrackerThread, SIGNAL(started()), eyeTracker, SLOT(Start()));
    connect(eyeTracker, SIGNAL(blinkDetected()), this, SLOT(onBlinkDectected()));
    connect(eyeTracker, SIGNAL(finished()), eyeTrackerThread, SLOT(quit()));
    connect(eyeTrackerThread, SIGNAL(finished()), eyeTracker, SLOT(deleteLater()));
    connect(eyeTrackerThread, SIGNAL(finished()), eyeTrackerThread, SLOT(deleteLater()));
}

void MainWidget::setupViews() {
    maskView = new MaskView(this);

    webView = new QWebView(this);
    webView->load(QUrl("http://en.wikipedia.org/wiki/Principal_component_analysis"));

    baseLayout = new QGridLayout;
    baseLayout->addWidget(maskView, 0, 0);
    baseLayout->addWidget(webView, 0, 0);

    this->setLayout(baseLayout);
}

void MainWidget::setupTimers() {
    fatigueTimer = new QTimer(this);
    fatigueTimer->setInterval(FATIGUE_LIMIT);
    connect(fatigueTimer, SIGNAL(timeout()), this, SLOT(onFatigueTimerTimeOut()));
}

void MainWidget::start() {
    blinkCounter = 0;
    timestamp = QDateTime::currentDateTime();
    fatigueTimer->start(FATIGUE_LIMIT);
    eyeTrackerThread->start();
    outputLog(" |----------------------| ");
}

void MainWidget::stop() {
    eyeTracker->StopTracking();
    fatigueTimer->stop();

    float blinkRate = static_cast<float>(blinkCounter) * 60 / timestamp.secsTo(QDateTime::currentDateTime());
    outputLog(QString(" Eye blink rate: ").append(QString::number(blinkRate)));
    outputLog(" |----------------------| ");
}

// -------- slots -------- //
void MainWidget::onFatigueTimerTimeOut() {
    if (toFlash) {
        maskView->flash();
        fatigueTimer->start();
    }

    if (toBlur) {
        maskView->blur();
        fatigueTimer->stop();
    }

    outputLog(" stimulated ");
    update();   // todo glwidget.update()
}

void MainWidget::onBlinkDectected() {
    blinkCounter++;
    fatigueTimer->start();
    maskView->reset();
    outputLog(" blink detected ");
}

// -------- utils -------- //
void MainWidget::outputLog(const QString &msg) {
    QString fileName = QString("./log/").append(timestamp.toString()).append(".txt");
    QFile logFile(fileName);
    if (logFile.open(QFile::ReadWrite|QFile::Append|QFile::Text)) {
        QTextStream outStream(&logFile);
        outStream << QDateTime::currentDateTime().toString() << msg << "\n";
    }
}
