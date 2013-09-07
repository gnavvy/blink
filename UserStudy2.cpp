#include "UserStudy2.h"

UserStudy2::UserStudy2(QWidget *parent) : QWidget(parent) {
    setupEyeTracker();
    setupTimers();
    setupViews();
    setupTasks();
}

void UserStudy2::setupEyeTracker() {
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

void UserStudy2::setupTimers() {
    fatigueTimer = new QTimer(this);
    fatigueTimer->setInterval(FATIGUE_LIMIT);
    connect(fatigueTimer, SIGNAL(timeout()), this, SLOT(onFatigueTimerTimeOut()));
}

void UserStudy2::setupViews() {
    gridLayout = new QGridLayout(this);  // 9*9 grid

    cameraView = new QLabel(this);
    cameraView->setAlignment(Qt::AlignCenter);

    maskView = new MaskView(this);
    maskView->setContext(cameraView);
    maskView->setAttribute(Qt::WA_TransparentForMouseEvents);

    gridLayout->addWidget(maskView, 1, 0, 9, 9);
    gridLayout->addWidget(cameraView, 1, 0, 9, 9);

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
    maskView->updateFboSize();
}

void UserStudy2::setupTasks() {}

void UserStudy2::onStartButtonClicked() {
    cameraViewEnabled = true;

    buttonStart->setDisabled(true);
    blinkCounter = 0;
    timestart = QDateTime::currentDateTime();

    QString taskPath = QString("./log/").append(timestart.toLocalTime().toString());
    if (!QDir(taskPath).exists())
        QDir().mkdir(taskPath);

    timestamp = timestart;
    eyeTrackerThread->start();
    fatigueTimer->start();
    outputLog(" |----------------------| ");
}

void UserStudy2::onTaskButtonClicked() {
    cameraViewEnabled = false;
}

void UserStudy2::onPauseButtonClicked() {
    cameraViewEnabled = true;

    QDateTime now = QDateTime::currentDateTime();
    float blinkRate = static_cast<float>(blinkCounter) * 60 / timestamp.secsTo(now);
    outputLog(QString(" Eye blink rate: ").append(QString::number(blinkRate)));
    outputLog(" |----------------------| ");

    blinkCounter = 0;
    timestamp = now;
}

void UserStudy2::onFinishButtonClicked() {
    eyeTracker->stop();
    fatigueTimer->stop();
    cameraView->clear();
}

void UserStudy2::onFatigueTimerTimeOut() {
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

void UserStudy2::onBlinkDectected() {
    blinkCounter++;
    if (stimulusEnabled) {
        fatigueTimer->start();
        maskView->reset();
    }
    if (!cameraViewEnabled)
        outputLog(" blink detected ");
}

void UserStudy2::onCvFrameReady(QImage img) {
    if (cameraViewEnabled) {
        cameraView->setPixmap(QPixmap::fromImage(img));
    } else {
        QString fp = QString("./log/").append(timestart.toLocalTime().toString());      // task path
        fp.append("/").append(timestamp.toLocalTime().toString()).append("/");          // subtask path
        fp.append(QString::number(timestamp.msecsTo(QDateTime::currentDateTime())));    // file name
        img.scaled(320, 240, Qt::KeepAspectRatio).save(fp.append(".png"), "PNG");
    }
}

void UserStudy2::resizeEvent(QResizeEvent *) {
    maskView->updateFboSize();
}

void UserStudy2::outputLog(const QString &msg) {
    QString fn = QString("./log/").append(timestart.toLocalTime().toString());
    fn.append("/").append(timestart.toLocalTime().toString()).append(".txt");
    QFile logFile(fn);
    if (logFile.open(QFile::ReadWrite|QFile::Append|QFile::Text)) {
        QTextStream outStream(&logFile);
        outStream << QDateTime::currentDateTime().toString() << msg << "\n";
    }
}
