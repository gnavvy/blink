#include "MainWidget.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent) {
    this->resize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
    setupEyeTracker();
    setupTimers();
    setupViews();
    setupTasks();
}

MainWidget::~MainWidget() {
    if (maskView)       delete maskView;
    if (webView)        delete webView;
    if (fatigueTimer)   delete fatigueTimer;

    foreach (auto button, taskButtons)
        delete button;
}

void MainWidget::setupEyeTracker() {
    eyeTrackerThread = new QThread();
    eyeTracker       = new EyeTracker();
    eyeTracker->moveToThread(eyeTrackerThread);

    connect(eyeTrackerThread, SIGNAL(started()), eyeTracker, SLOT(Start()));
    connect(eyeTracker, SIGNAL(blinkDetected()), this, SLOT(onBlinkDectected()));
    connect(eyeTracker, SIGNAL(finished()), eyeTrackerThread, SLOT(quit()));
    connect(eyeTrackerThread, SIGNAL(finished()), eyeTracker, SLOT(deleteLater()));
    connect(eyeTrackerThread, SIGNAL(finished()), eyeTrackerThread, SLOT(deleteLater()));
}

void MainWidget::setupViews() {
    baseLayout = new QGridLayout();  // 9*9 grid

    webView = new QWebView();
    maskView = new MaskView(webView);
    maskView->setAttribute(Qt::WA_TransparentForMouseEvents);

    baseLayout->addWidget(maskView, 1, 0, 9, 9);
    baseLayout->addWidget(webView, 1, 0, 9, 9);

    for (int i = 0; i < NUM_TASKS; i++) {
        QPushButton *button = new QPushButton("Task"+QString::number(i+1));
        connect(button, SIGNAL(clicked()), this, SLOT(onTaskButtonClicked()));
        baseLayout->addWidget(button, 0, i+1, 1, 1);
        taskButtons.push_back(button);
    }

    buttonStart = new QPushButton("Start");
    buttonPause = new QPushButton("Pause");
    buttonFinish = new QPushButton("Finish");
    connect(buttonStart, SIGNAL(clicked()), this, SLOT(onStartButtonClicked()));
    connect(buttonPause, SIGNAL(clicked()), this, SLOT(onPauseButtonClicked()));
    connect(buttonFinish, SIGNAL(clicked()), this, SLOT(onFinishButtonClicked()));
    baseLayout->addWidget(buttonStart, 0, 0, 1, 1);
    baseLayout->addWidget(buttonPause, 0, 7, 1, 1);
    baseLayout->addWidget(buttonFinish, 0, 8, 1, 1);

    this->setLayout(baseLayout);
    // update maskView sizes for fbo
    maskView->updateLayout();
}

void MainWidget::setupTimers() {
    fatigueTimer = new QTimer(this);
    fatigueTimer->setInterval(FATIGUE_LIMIT);
    connect(fatigueTimer, SIGNAL(timeout()), this, SLOT(onFatigueTimerTimeOut()));
}

void MainWidget::setupTasks() {
    taskUrls.push_back(QUrl("http://mrnussbaum.com/readingcomp/doughnuts/"));
    taskUrls.push_back(QUrl("http://en.wikipedia.org/wiki/Principal_component_analysis"));
    taskUrls.push_back(QUrl("http://www.spotthedifference.com/photogame.asp"));
    taskUrls.push_back(QUrl("http://faculty.washington.edu/chudler/puzmatch.html"));
    taskUrls.push_back(QUrl("file:///Users/Yang/Develop/blink/video/Ted.mp4"));
    taskUrls.push_back(QUrl("file:///Users/Yang/Develop/blink/video/Trailer.mp4"));
    std::random_shuffle(taskUrls.begin(), taskUrls.end());
}

// -------- slots -------- //
void MainWidget::onStartButtonClicked() {
    blinkCounter = 0;
    timestart = QDateTime::currentDateTime();
    timestamp = timestart;
    eyeTrackerThread->start();
    outputLog(" |----------------------| ");
}

void MainWidget::onPauseButtonClicked() {
    paused = true;
    blinkCounter = 0;

    QDateTime current = QDateTime::currentDateTime();
    float blinkRate = static_cast<float>(blinkCounter) * 60 / timestamp.secsTo(current);
    timestamp = current;

    outputLog(QString(" Eye blink rate: ").append(QString::number(blinkRate)));
    outputLog(" |----------------------| ");
}

void MainWidget::onFinishButtonClicked() {
    eyeTracker->StopTracking();
    fatigueTimer->stop();

    float blinkRate = static_cast<float>(blinkCounter) * 60 / timestamp.secsTo(QDateTime::currentDateTime());
    outputLog(QString(" Eye blink rate: ").append(QString::number(blinkRate)));
    outputLog(" |----------------------| ");
}

void MainWidget::onFatigueTimerTimeOut() {
    if (toFlash) {
        maskView->flash();
        fatigueTimer->start();
    }

    if (toBlur) {
        maskView->blur();
        fatigueTimer->stop();
    }

    maskView->update();
    outputLog(" stimulated ");
}

void MainWidget::onTaskButtonClicked() {
    QPushButton *button = (QPushButton*)sender();
    int taskId = button->text().right(1).toInt()-1;
    QUrl taskUrl = taskUrls[taskId];
    if (taskUrl.scheme() == SCHEME_HTTP)
        webView->load(taskUrl);
    else if (taskUrl.scheme() == SCHEME_FILE)
        QDesktopServices::openUrl(taskUrl);

    paused = false;
    timestamp = QDateTime::currentDateTime();
    outputLog(QString(" Task: ").append(QUrl(taskUrls[taskId]).toString()));
}

void MainWidget::onBlinkDectected() {
    blinkCounter++;
    if (stimulusEnabled) {
        fatigueTimer->start();
        maskView->reset();
    }
    if (!paused)
        outputLog(" blink detected ");
}

// -------- utils -------- //
void MainWidget::outputLog(const QString &msg) {
    QString fileName = QString("./log/").append(timestart.toString()).append(".txt");
    QFile logFile(fileName);
    if (logFile.open(QFile::ReadWrite|QFile::Append|QFile::Text)) {
        QTextStream outStream(&logFile);
        outStream << QDateTime::currentDateTime().toString() << msg << "\n";
    }
}
