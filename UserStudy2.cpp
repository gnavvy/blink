#include "UserStudy2.h"

UserStudy2::UserStudy2(QWidget *parent) : QWidget(parent) {
    setupEyeTracker();
    setupTimers();
    setupViews();
    setupStimulus();
}

void UserStudy2::setupEyeTracker() {
    eyeTrackerThread = new QThread();
    eyeTracker = new EyeTracker();
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
    connect(fatigueTimer, SIGNAL(timeout()), this, SLOT(onFatigueTimerTimeOut()));

    stimuliTimer = new QTimer(this);
    connect(stimuliTimer, SIGNAL(timeout()), this, SLOT(onStimuliTimerTimeOut()));
}

void UserStudy2::setupViews() {
    gridLayout = new QGridLayout(this);  // 8*8 grid
    debugLabel = new QLabel(this);
    gridLayout->addWidget(debugLabel, 0, 7, 1, 1);

    contentStack = new QStackedWidget(this);

    cameraView = new QLabel(contentStack);
//    cameraView->setFixedSize(800, 600);
    cameraView->setAlignment(Qt::AlignTop|Qt::AlignHCenter);

    webView = new QWebView(contentStack);
//    webView->setFixedSize(800, 600);
//    webView->move(200, 200);
//    webView->load(QUrl("http://www.hdpuzzles.com/?puzzle=302"));
//    webView->load(QUrl("http://www.hdpuzzles.com/?puzzle=170"));
    webView->load(QUrl("http://www.spotthedifference.com/photogame.asp"));

    contentStack->addWidget(webView);
    contentStack->addWidget(cameraView);
    contentStack->setCurrentWidget(cameraView);
    gridLayout->addWidget(contentStack, 1, 0, 8, 8);

    maskView = new MaskView(contentStack);
    maskView->setContext(contentStack);
    maskView->setAttribute(Qt::WA_TransparentForMouseEvents);
    gridLayout->addWidget(maskView, 1, 0, 8, 8);

    buttonStart = new QPushButton("Camera On");
    buttonDemo = new QPushButton("Demo");
    buttonTask = new QPushButton("Start");
    buttonStop = new QPushButton("Stop");
    buttonFinish = new QPushButton("Camera Off");

    connect(buttonStart, SIGNAL(clicked()), this, SLOT(onStartButtonClicked()));
    connect(buttonDemo, SIGNAL(clicked()), this, SLOT(onDemoButtonClicked()));
    connect(buttonTask, SIGNAL(clicked()), this, SLOT(onTaskButtonClicked()));
    connect(buttonStop, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));
    connect(buttonFinish, SIGNAL(clicked()), this, SLOT(onFinishButtonClicked()));

    gridLayout->addWidget(buttonStart, 0, 0, 1, 1);
    gridLayout->addWidget(buttonDemo, 0, 1, 1, 1);
    gridLayout->addWidget(buttonTask, 0, 2, 1, 1);
    gridLayout->addWidget(buttonStop, 0, 3, 1, 1);
    gridLayout->addWidget(buttonFinish, 0, 4, 1, 1);

    this->setLayout(gridLayout);

    cameraView->resize(contentStack->size());
    webView->resize(contentStack->size());
    maskView->resize(contentStack->size());

    maskView->updateFboSize();

    popupView = new PopupView(this);
    QRect screenSpace = QApplication::desktop()->screenGeometry(1);
    popupView->move(screenSpace.x() + screenSpace.width()  - popupView->width()  - 30,
                    screenSpace.y() + screenSpace.height() - popupView->height() - 24);
}

void UserStudy2::setupStimulus() {
    stimuliModes.push_back(StimuliMode::None);
    stimuliModes.push_back(StimuliMode::Flash);
    stimuliModes.push_back(StimuliMode::Blur);
    stimuliModes.push_back(StimuliMode::Edge);
    stimuliModes.push_back(StimuliMode::Popup);

    std::srand(QTime::currentTime().msec());
    std::random_shuffle(stimuliModes.begin(), stimuliModes.end());

    stimuliModes.push_front(StimuliMode::Popup);
    stimuliModes.push_front(StimuliMode::Edge);
    stimuliModes.push_front(StimuliMode::Blur);
    stimuliModes.push_front(StimuliMode::Flash);
    stimuliModes.push_front(StimuliMode::None);

    stimuliNames.push_back("Control");
    stimuliNames.push_back("Flash");
    stimuliNames.push_back("Blur");
    stimuliNames.push_back("Edge Highlight");
    stimuliNames.push_back("Popup");
}

void UserStudy2::onStartButtonClicked() {
    cameraViewEnabled = true;
    contentStack->setCurrentWidget(cameraView);

    maskView->resize(contentStack->size());

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

void UserStudy2::onDemoButtonClicked() {
    buttonDemo->setEnabled(false);

    modeIndex = 0;
    currentMode = stimuliModes[modeIndex];

    stimuliTimer->start(kDemoTime*1000);
    debugLabel->setText(QString::number(modeIndex).append(": ").append(stimuliNames[currentMode]));

    fatigueTimer->start(randomStimulateInterval(4000, 8000));
}

void UserStudy2::onTaskButtonClicked() {
    buttonTask->setEnabled(false);

    modeIndex = 5;
    currentMode = stimuliModes[modeIndex];

    cameraViewEnabled = false;
    contentStack->setCurrentWidget(webView);

    timestamp = QDateTime::currentDateTime();
    QString subPath = QString("./log/%1/%2").arg(timestart.toString(), timestamp.toString());
    if (!QDir(subPath).exists())
        QDir().mkdir(subPath);

    stimuliTimer->start(kTestTime*1000);
    fatigueTimer->start(randomStimulateInterval(4000, 8000));
}

void UserStudy2::onStopButtonClicked() {
    cameraViewEnabled = true;
    contentStack->setCurrentWidget(cameraView);
//    contentStack->setStyleSheet("background-image: ");
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
        case StimuliMode::None:  return;
        case StimuliMode::Flash: maskView->flash(); break;
        case StimuliMode::Blur:  maskView->blur(); break;
        case StimuliMode::Edge:  maskView->highlight(); break;
        case StimuliMode::Popup: popupView->show(); break;
        default: break;
    }
    outputLog(" triggered ");
}

void UserStudy2::onStimuliTimerTimeOut() {
    if (++modeIndex >= stimuliModes.size()) {
        onStopButtonClicked();
        stimuliTimer->stop();
        fatigueTimer->stop();
        eyeTracker->stop();
        return;
    }

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
    debugLabel->setText(QString::number(modeIndex).append(": ").append(stimuliNames[currentMode]));

    // 0: none_test, 1: flash_test, 2: blur_test, 3: edge_test, 4: popup_test, each 15s
    // 5-9 random stimuli, 120s per stimuli
    if (modeIndex == 5) {   // start real task
        onTaskButtonClicked();
    } else if (modeIndex < 5) {
        stimuliTimer->start(kDemoTime*1000);
    } else {
        stimuliTimer->start(kTestTime*1000);
        outputLog(QString(" %1 %2s").arg(stimuliNames[currentMode], QString::number(kTestTime)));
    }
}

void UserStudy2::onBlinkDectected() {
    blinkCounter++;
    if (stimulusEnabled) {
        fatigueTimer->start(randomStimulateInterval(4000, 8000));
        maskView->reset();
        popupView->hide();
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
