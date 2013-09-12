#include "UserStudy2.h"

UserStudy2::UserStudy2(QWidget *parent) : QWidget(parent) {
    setupEyeTracker();
    setupTimers();
    setupViews();
    setupStimulus();
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
//    fatigueTimer->setInterval(FATIGUE_LIMIT);
    connect(fatigueTimer, SIGNAL(timeout()), this, SLOT(onFatigueTimerTimeOut()));

    stimuliTimer = new QTimer(this);
    connect(stimuliTimer, SIGNAL(timeout()), this, SLOT(onStimuliTimerTimeOut()));
}

void UserStudy2::setupViews() {
    gridLayout = new QGridLayout(this);  // 8*8 grid
    contentStack = new QStackedWidget(this);

    cameraView = new QLabel(contentStack);
    cameraView->setAlignment(Qt::AlignCenter);

    webView = new QWebView(contentStack);
    webView->load(QUrl("http://www.hdpuzzles.com/?puzzle=302"));

    contentStack->addWidget(webView);
    contentStack->addWidget(cameraView);
    contentStack->setCurrentWidget(cameraView);
    gridLayout->addWidget(contentStack, 1, 0, 8, 8);

    maskView = new MaskView(contentStack);
    maskView->setContext(contentStack);
    maskView->setAttribute(Qt::WA_TransparentForMouseEvents);
    gridLayout->addWidget(maskView, 1, 0, 8, 8);

    buttonStart = new QPushButton("Camera On");
    buttonTask = new QPushButton("Task");
    buttonStop = new QPushButton("Stop");
    buttonFinish = new QPushButton("Camera Off");

    connect(buttonStart, SIGNAL(clicked()), this, SLOT(onStartButtonClicked()));
    connect(buttonTask, SIGNAL(clicked()), this, SLOT(onTaskButtonClicked()));
    connect(buttonStop, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));
    connect(buttonFinish, SIGNAL(clicked()), this, SLOT(onFinishButtonClicked()));

    gridLayout->addWidget(buttonStart, 0, 0, 1, 1);
    gridLayout->addWidget(buttonTask, 0, 1, 1, 1);
    gridLayout->addWidget(buttonStop, 0, 2, 1, 1);
    gridLayout->addWidget(buttonFinish, 0, 3, 1, 1);

    this->setLayout(gridLayout);
    maskView->updateFboSize();

    popupView = new PopupView(this);
    QRect screenSpace = QApplication::desktop()->screenGeometry(1);
    popupView->move(screenSpace.x() + screenSpace.width() - popupView->width() - 24,
                    screenSpace.y() + screenSpace.height() - popupView->height() - 24);
}

void UserStudy2::setupStimulus() {
    stimuliDuration[StimuliMode::None]  = 10;
    stimuliDuration[StimuliMode::Flash] = 10;
    stimuliDuration[StimuliMode::Blur]  = 10;
    stimuliDuration[StimuliMode::Edge]  = 10;
    stimuliDuration[StimuliMode::Popup] = 10;

    // start and end with no-stimuli, random stimulus in between
    stimuliModes.push_back(StimuliMode::Flash);
    stimuliModes.push_back(StimuliMode::Blur);
    stimuliModes.push_back(StimuliMode::Edge);
    stimuliModes.push_back(StimuliMode::Popup);
    std::srand(QTime::currentTime().msec());
    std::random_shuffle(stimuliModes.begin(), stimuliModes.end());
    stimuliModes.push_front(StimuliMode::None);
    stimuliModes.push_back(StimuliMode::None);

    stimuliNames.push_back("None");
    stimuliNames.push_back("Flash");
    stimuliNames.push_back("Blur");
    stimuliNames.push_back("Edge Highlight");
    stimuliNames.push_back("Popup");

    modeIndex = 0;
    currentMode = stimuliModes[modeIndex];
}

void UserStudy2::onStartButtonClicked() {
    cameraViewEnabled = true;
    contentStack->setCurrentWidget(cameraView);

    buttonStart->setDisabled(true);
    blinkCounter = 0;
    timestart = QDateTime::currentDateTime();

    QString path = QString("./log/%1").arg(timestart.toString());
    if (!QDir(path).exists())
        QDir().mkdir(path);

    timestamp = timestart;
    eyeTrackerThread->start();

    outputLog(" |----------------------| ");
}

void UserStudy2::onTaskButtonClicked() {
    cameraViewEnabled = false;
    contentStack->setCurrentWidget(webView);

    webView->resize(contentStack->size());
    ((QPushButton*)sender())->setDisabled(true);

    timestamp = QDateTime::currentDateTime();

    QString subPath = QString("./log/%1/%2").arg(timestart.toString(), timestamp.toString());
    if (!QDir(subPath).exists())
        QDir().mkdir(subPath);

    int currentModeDuration = stimuliDuration[currentMode];
    stimuliTimer->start(currentModeDuration*1000);
    fatigueTimer->start(randomStimulateInterval(4000, 8000));
}

void UserStudy2::onStopButtonClicked() {
    cameraViewEnabled = true;
    contentStack->setCurrentWidget(cameraView);
    ((QPushButton*)sender())->setDisabled(true);

    QDateTime now = QDateTime::currentDateTime();
    float blinkRate = static_cast<float>(blinkCounter) * 60 / timestamp.secsTo(now);
    outputLog(QString(" blink rate for %1: %2").arg(stimuliNames[currentMode], QString::number(blinkRate)));
    outputLog(" |----------------------| ");
    blinkCounter = 0;
    timestamp = now;
}

void UserStudy2::onFinishButtonClicked() {
    cameraView->clear();
    eyeTracker->stop();
    fatigueTimer->stop();
}

void UserStudy2::onFatigueTimerTimeOut() {
    switch (currentMode) {
        case StimuliMode::None:
            return;
        case StimuliMode::Flash:
            maskView->flash(); break;
        case StimuliMode::Blur:
            maskView->blur(); break;
        case StimuliMode::Edge:
            maskView->highlight(); break;
        case StimuliMode::Popup:
            popupView->flash(); break;
        default: break;
    }

    outputLog(" triggered ");
}

void UserStudy2::onStimuliTimerTimeOut() {
    if (++modeIndex >= stimuliModes.size())
        return;

    QDateTime now = QDateTime::currentDateTime();
    float blinkRate = static_cast<float>(blinkCounter) * 60 / timestamp.secsTo(now);
    outputLog(QString(" blink rate for %1: %2").arg(stimuliNames[currentMode], QString::number(blinkRate)));
    outputLog(" |----------------------| ");
    blinkCounter = 0;
    timestamp = now;


    QString subPath = QString("./log/%1/%2").arg(timestart.toString(), timestamp.toString());
    if (!QDir(subPath).exists())
        QDir().mkdir(subPath);

    currentMode = stimuliModes[modeIndex];
    int currentModeDuration = stimuliDuration[currentMode];
    stimuliTimer->start(currentModeDuration*1000);
    outputLog(QString(" %1 %2s").arg(stimuliNames[currentMode], QString::number(currentModeDuration)));
}

void UserStudy2::onBlinkDectected() {
    blinkCounter++;
    if (stimulusEnabled) {
        fatigueTimer->start(randomStimulateInterval(4000, 8000));
        maskView->reset();
    }
    if (!cameraViewEnabled)
        outputLog(" blinked ");
}

void UserStudy2::onCvFrameReady(QImage img) {
    if (cameraViewEnabled) {
        cameraView->setPixmap(QPixmap::fromImage(img));
    } else {
        QString path = QString("./log/%1/%2/%3.png").arg(timestart.toString(), timestamp.toString(),
                            QString::number(timestamp.msecsTo(QDateTime::currentDateTime())));
        img.scaled(320, 240, Qt::KeepAspectRatio).save(path, "PNG");
    }
}

void UserStudy2::resizeEvent(QResizeEvent *) {
    maskView->updateFboSize();
}

void UserStudy2::outputLog(const QString &msg) {
    QString path = QString("./log/%1/%2.txt").arg(timestart.toString(), timestart.toString());
    QFile logFile(path);
    if (logFile.open(QFile::ReadWrite|QFile::Append|QFile::Text)) {
        QTextStream outStream(&logFile);
        outStream << QDateTime::currentDateTime().toString() << msg << "\n";
    }
}

int UserStudy2::randomStimulateInterval(int min, int max) {
    qsrand((uint)QTime::currentTime().msec());
    return qrand() % ((max + 1) - min) + min;
}
