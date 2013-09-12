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
    fatigueTimer->setInterval(FATIGUE_LIMIT);
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

    maskView = new MaskView(contentStack);
    maskView->setContext(contentStack);
    maskView->setAttribute(Qt::WA_TransparentForMouseEvents);

    gridLayout->addWidget(contentStack, 1, 0, 8, 8);
    gridLayout->addWidget(maskView, 1, 0, 8, 8);

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
    gridLayout->addWidget(buttonPause, 0, NUM_TASKS+1, 1, 1);
    gridLayout->addWidget(buttonFinish, 0, NUM_TASKS+2, 1, 1);

    this->setLayout(gridLayout);
    maskView->updateFboSize();

    popupView = new PopupView(this);
    QRect screenSpace = QApplication::desktop()->screenGeometry(1);
    popupView->move(screenSpace.x() + screenSpace.width() - popupView->width() - 24,
                    screenSpace.y() + screenSpace.height() - popupView->height() - 24);
}

void UserStudy2::setupStimulus() {
    stimuliDuration[StimuliMode::None]  = 5 * 1000;
    stimuliDuration[StimuliMode::Flash] = 20 * 1000;
    stimuliDuration[StimuliMode::Blur]  = 20 * 1000;
    stimuliDuration[StimuliMode::Edge]  = 20 * 1000;
    stimuliDuration[StimuliMode::Popup] = 20 * 1000;

    // start and end with no-stimuli, random stimulus in between
    stimuliModes.push_back(StimuliMode::Flash);
    stimuliModes.push_back(StimuliMode::Blur);
    stimuliModes.push_back(StimuliMode::Edge);
    stimuliModes.push_back(StimuliMode::Popup);
    std::srand(QTime::currentTime().msec());
    std::random_shuffle(stimuliModes.begin(), stimuliModes.end());
    stimuliModes.push_front(StimuliMode::None);
    stimuliModes.push_back(StimuliMode::None);

    modeIndex = 0;
    currentMode = stimuliModes[modeIndex];
}

void UserStudy2::onStartButtonClicked() {
    cameraViewEnabled = true;
    contentStack->setCurrentWidget(cameraView);

    buttonStart->setDisabled(true);
    blinkCounter = 0;
    timestart = QDateTime::currentDateTime();

    QString taskPath = QString("./log/").append(timestart.toLocalTime().toString());
    if (!QDir(taskPath).exists())
        QDir().mkdir(taskPath);

    int currentModeDuration = stimuliDuration[currentMode];
    stimuliTimer->start(currentModeDuration);
    qDebug() << "mode: " << currentMode << ", t: " << currentModeDuration;

    timestamp = timestart;
    eyeTrackerThread->start();
    fatigueTimer->start();
    outputLog(" |----------------------| ");
}

void UserStudy2::onTaskButtonClicked() {
    cameraViewEnabled = false;
    contentStack->setCurrentWidget(webView);

    webView->resize(contentStack->size());
    ((QPushButton*)sender())->setDisabled(true);

    //    int currentModeDuration = stimuliDuration[currentMode];
    //    stimuliTimer->start(currentModeDuration);
    //    qDebug() << "mode: " << currentMode << ", t: " << currentModeDuration;
}

void UserStudy2::onPauseButtonClicked() {
    cameraViewEnabled = true;
    contentStack->setCurrentWidget(cameraView);

    ((QPushButton*)sender())->setDisabled(true);
    QDateTime now = QDateTime::currentDateTime();
    float blinkRate = static_cast<float>(blinkCounter) * 60 / timestamp.secsTo(now);
    outputLog(QString(" Eye blink rate: ").append(QString::number(blinkRate)));
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

    maskView->update();
    outputLog(QString(" stimulated (").append(QString::number(currentMode)).append(")"));
}

void UserStudy2::onStimuliTimerTimeOut() {
    if (modeIndex >= stimuliModes.size())
        return;

    QDateTime now = QDateTime::currentDateTime();
    float blinkRate = static_cast<float>(blinkCounter) * 60 / timestamp.secsTo(now);
    outputLog(QString(" Eye blink rate: ").append(QString::number(blinkRate)));
    outputLog(" |----------------------| ");
    blinkCounter = 0;
    timestamp = now;

    currentMode = stimuliModes[modeIndex];
    int currentModeDuration = stimuliDuration[currentMode];
    stimuliTimer->start(currentModeDuration);
    outputLog(" mode: " + QString::number(currentMode) + ", t: " + QString::number(currentModeDuration));

    modeIndex++;
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
