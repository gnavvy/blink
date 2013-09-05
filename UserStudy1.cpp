#include "UserStudy1.h"

UserStudy1::UserStudy1(QWidget *parent) : QWidget(parent) {
    setupEyeTracker();
    setupViews();
    setupTasks();
}

void UserStudy1::setupEyeTracker() {
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

void UserStudy1::setupViews() {
    gridLayout = new QGridLayout(this);  // 9*9 grid
    contentStack = new QStackedWidget(this);

    cameraView = new QLabel(this);
    cameraView->setAlignment(Qt::AlignCenter);

    webView = new QWebView(this);
    memtestView = new MemTest(this);

    contentStack->addWidget(cameraView);
    contentStack->addWidget(webView);
    contentStack->addWidget(memtestView);

    gridLayout->addWidget(contentStack, 1, 0, 9, 9);

    for (int i = 0; i < NUM_TASKS; i++) {
        QPushButton *button = new QPushButton("Task"+QString::number(i+1));
        connect(button, SIGNAL(clicked()), this, SLOT(onTaskButtonClicked()));
        gridLayout->addWidget(button, 0, i+1, 1, 1);
        taskButtons.push_back(button);
    }

    buttonStart = new QPushButton("Camera On");
    buttonPause = new QPushButton("Stop");
    buttonFinish = new QPushButton("Camera Off");

    connect(buttonStart, SIGNAL(clicked()), this, SLOT(onStartButtonClicked()));
    connect(buttonPause, SIGNAL(clicked()), this, SLOT(onPauseButtonClicked()));
    connect(buttonFinish, SIGNAL(clicked()), this, SLOT(onFinishButtonClicked()));

    gridLayout->addWidget(buttonStart, 0, 0, 1, 1);
    gridLayout->addWidget(buttonPause, 0, 7, 1, 1);
    gridLayout->addWidget(buttonFinish, 0, 8, 1, 1);

    this->setLayout(gridLayout);
}

void UserStudy1::setupTasks() {
    taskUrls.push_back(QUrl("http://mrnussbaum.com/readingcomp/doughnuts/"));
    taskUrls.push_back(QUrl("http://en.wikipedia.org/wiki/Principal_component_analysis"));
    taskUrls.push_back(QUrl("http://www.spotthedifference.com/photogame.asp"));
    taskUrls.push_back(QUrl("task://memory.test"));
    taskUrls.push_back(QUrl("file:///Users/Yang/Develop/blink/video/SonaarLuthra_2011G-480p.mp4"));
    taskUrls.push_back(QUrl("file:///Users/Yang/Develop/blink/video/Trailer.mp4"));
    std::srand(QTime::currentTime().msec());
    std::random_shuffle(taskUrls.begin(), taskUrls.end());
}

// -------- slots -------- //
void UserStudy1::onStartButtonClicked() {
    cameraViewEnabled = true;
    contentStack->setCurrentWidget(cameraView);

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

void UserStudy1::onTaskButtonClicked() {
    cameraViewEnabled = false;

    buttonCurrentTask = (QPushButton*)sender();
    buttonCurrentTask->setDisabled(true);
    int taskId = buttonCurrentTask->text().right(1).toInt()-1;
    QUrl taskUrl = taskUrls[taskId];
    if (taskUrl.scheme() == SCHEME_HTTP) {
        contentStack->setCurrentWidget(webView);
        webView->load(taskUrl);
    } else if (taskUrl.scheme() == SCHEME_TASK) {
        contentStack->setCurrentWidget(memtestView);
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

void UserStudy1::onPauseButtonClicked() {
    cameraViewEnabled = true;
    contentStack->setCurrentWidget(cameraView);

    QDateTime now = QDateTime::currentDateTime();
    float blinkRate = static_cast<float>(blinkCounter) * 60 / timestamp.secsTo(now);
    outputLog(QString(" Eye blink rate: ").append(QString::number(blinkRate)));
    outputLog(" |----------------------| ");

    blinkCounter = 0;
    timestamp = now;
}

void UserStudy1::onFinishButtonClicked() {
    eyeTracker->stop();
    cameraView->clear();
}

void UserStudy1::onBlinkDectected() {
    blinkCounter++;
    if (!cameraViewEnabled)
        outputLog(" blink detected ");
}

void UserStudy1::onCvFrameReady(QImage img) {
    if (cameraViewEnabled) {
        cameraView->setPixmap(QPixmap::fromImage(img));
    } else {
        QString fp = QString("./log/").append(timestart.toLocalTime().toString());      // task path
        fp.append("/").append(timestamp.toLocalTime().toString()).append("/");          // subtask path
        fp.append(QString::number(timestamp.msecsTo(QDateTime::currentDateTime())));    // file name
        img.scaled(320, 240, Qt::KeepAspectRatio).save(fp.append(".png"), "PNG");
    }
}

// -------- utils -------- //
void UserStudy1::outputLog(const QString &msg) {
    QString fn = QString("./log/").append(timestart.toLocalTime().toString());
    fn.append("/").append(timestart.toLocalTime().toString()).append(".txt");
    QFile logFile(fn);
    if (logFile.open(QFile::ReadWrite|QFile::Append|QFile::Text)) {
        QTextStream outStream(&logFile);
        outStream << QDateTime::currentDateTime().toString() << msg << "\n";
    }
}
