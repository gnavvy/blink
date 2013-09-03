#include "MainWidget.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent) {
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

    connect(eyeTrackerThread, SIGNAL(started()), eyeTracker, SLOT(start()));
    connect(eyeTracker, SIGNAL(frameReady(QImage)), this, SLOT(onCvFrameReady(QImage)));
    connect(eyeTracker, SIGNAL(blinkDetected()), this, SLOT(onBlinkDectected()));
    connect(eyeTracker, SIGNAL(finished()), eyeTrackerThread, SLOT(quit()));
    connect(eyeTrackerThread, SIGNAL(finished()), eyeTracker, SLOT(deleteLater()));
    connect(eyeTrackerThread, SIGNAL(finished()), eyeTrackerThread, SLOT(deleteLater()));
}

void MainWidget::setupViews() {
    gridLayout = new QGridLayout(this);  // 9*9 grid

    cameraView = new QLabel(this);
    cameraView->setAlignment(Qt::AlignCenter);

    webView = new QWebView(this);
    memView = new MemTest(this);

    maskView = new MaskView(this);
    maskView->setContext(cameraView);  // temp
    maskView->setAttribute(Qt::WA_TransparentForMouseEvents);

    gridLayout->addWidget(cameraView, 1, 0, 9, 9);
    gridLayout->addWidget(memView, 1, 0, 9, 9);
    gridLayout->addWidget(webView, 1, 0, 9, 9);
    gridLayout->addWidget(maskView, 1, 0, 9, 9);

    for (int i = 0; i < NUM_TASKS; i++) {
        QPushButton *button = new QPushButton("Task"+QString::number(i+1));
        connect(button, SIGNAL(clicked()), this, SLOT(onTaskButtonClicked()));
        gridLayout->addWidget(button, 0, i+1, 1, 1);
        taskButtons.push_back(button);
    }

    buttonStart = new QPushButton("Start");
    buttonPause = new QPushButton("Pause");
    buttonFinish = new QPushButton("Finish");

    connect(buttonStart, SIGNAL(clicked()), this, SLOT(onStartButtonClicked()));
    connect(buttonPause, SIGNAL(clicked()), this, SLOT(onPauseButtonClicked()));
    connect(buttonFinish, SIGNAL(clicked()), this, SLOT(onFinishButtonClicked()));

    gridLayout->addWidget(buttonStart, 0, 0, 1, 1);
    gridLayout->addWidget(buttonPause, 0, 7, 1, 1);
    gridLayout->addWidget(buttonFinish, 0, 8, 1, 1);

    this->setLayout(gridLayout);
    maskView->updateFboSize();
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
    taskUrls.push_back(QUrl("task://memory.test"));
    taskUrls.push_back(QUrl("file:///Users/Yang/Develop/blink/video/Ted.mp4"));
    taskUrls.push_back(QUrl("file:///Users/Yang/Develop/blink/video/Trailer.mp4"));
    std::srand(QTime::currentTime().msec());
    std::random_shuffle(taskUrls.begin(), taskUrls.end());
}

// -------- slots -------- //
void MainWidget::onStartButtonClicked() {   
    cameraViewEnabled = true;
    maskView->setContext(cameraView);
    maskView->updateFboSize();

    buttonStart->setDisabled(true);
    blinkCounter = 0;
    timestart = QDateTime::currentDateTime();

    QString taskPath = QString("./log/").append(timestart.toLocalTime().toString());
    if (!QDir(taskPath).exists())
        QDir().mkdir(taskPath);

    timestamp = timestart;
    eyeTrackerThread->start();
    outputLog(" |----------------------| ");
}

void MainWidget::onTaskButtonClicked() {
    cameraViewEnabled = false;

    buttonCurrentTask = (QPushButton*)sender();
    buttonCurrentTask->setDisabled(true);
    int taskId = buttonCurrentTask->text().right(1).toInt()-1;
    QUrl taskUrl = taskUrls[taskId];
    if (taskUrl.scheme() == SCHEME_HTTP) {
        memView->hide();
        maskView->setContext(webView);
        maskView->updateFboSize();
        webView->load(taskUrl);
    } else if (taskUrl.scheme() == SCHEME_TASK) {
        memView->show();
        maskView->setContext(memView);
        maskView->updateFboSize();
    } else if (taskUrl.scheme() == SCHEME_FILE) {
        QDesktopServices::openUrl(taskUrl);
    }

    timestamp = QDateTime::currentDateTime();
    QString subTaskPath = QString("./log/").append(timestart.toLocalTime().toString());
    subTaskPath.append("/").append(timestamp.toLocalTime().toString());
    if (!QDir(subTaskPath).exists())
        QDir().mkdir(subTaskPath);

    outputLog(QString(" Task: ").append(QUrl(taskUrls[taskId]).toString()));
}

void MainWidget::onPauseButtonClicked() {
    cameraViewEnabled = true;
    maskView->setContext(cameraView);
    maskView->updateFboSize();

    QDateTime now = QDateTime::currentDateTime();
    float blinkRate = static_cast<float>(blinkCounter) * 60 / timestamp.secsTo(now);
    outputLog(QString(" Eye blink rate: ").append(QString::number(blinkRate)));
    outputLog(" |----------------------| ");

    blinkCounter = 0;
    timestamp = now;
}

void MainWidget::onFinishButtonClicked() {   
    eyeTracker->stopTracking();
    fatigueTimer->stop();

    QDateTime now = QDateTime::currentDateTime();
    float blinkRate = static_cast<float>(blinkCounter) * 60 / timestamp.secsTo(now);
    outputLog(QString(" Eye blink rate: ").append(QString::number(blinkRate)));
    outputLog(" |----------------------| ");

    cameraView->clear();
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

void MainWidget::onBlinkDectected() {
    blinkCounter++;
    if (stimulusEnabled) {
        fatigueTimer->start();
        maskView->reset();
    }
    if (!cameraViewEnabled)
        outputLog(" blink detected ");
}

void MainWidget::onCvFrameReady(QImage img) {
    if (cameraViewEnabled) {
        cameraView->setPixmap(QPixmap::fromImage(img));

    } else {
        QString fp = QString("./log/").append(timestart.toLocalTime().toString());      // task path
        fp.append("/").append(timestamp.toLocalTime().toString()).append("/");          // subtask path
        fp.append(QString::number(timestamp.msecsTo(QDateTime::currentDateTime())));    // file name
        img.scaled(320, 240, Qt::KeepAspectRatio).save(fp.append(".png"), "PNG");
    }
}

void MainWidget::resizeEvent(QResizeEvent *event) {
    maskView->updateFboSize();
}

// -------- utils -------- //
void MainWidget::outputLog(const QString &msg) {
    QString fn = QString("./log/").append(timestart.toLocalTime().toString());
    fn.append("/").append(timestart.toLocalTime().toString()).append(".txt");
    QFile logFile(fn);
    if (logFile.open(QFile::ReadWrite|QFile::Append|QFile::Text)) {
        QTextStream outStream(&logFile);
        outStream << QDateTime::currentDateTime().toString() << msg << "\n";
    }
}
