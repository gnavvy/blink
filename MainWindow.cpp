#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QStackedWidget(parent), pMainWindow_(new Ui::MainWindow) {
    setupUI();
    setupTimer();
    setupWorker();
    setupSignalSlots();
}

MainWindow::~MainWindow() {
    delete pMainWindow_;
    delete pFlashWidget_;
    delete pTimer_;
    delete pTracker_;
    delete pTrackerThread_;
}

void MainWindow::setupUI() {
    pMainWindow_->setupUi(this);
    pFlashWidget_ = new FlashWidget();
}

void MainWindow::setupTimer() {
    pTimer_ = new QTimer(this);
    pTimer_->setInterval(1000);
}

void MainWindow::setupWorker() {
    pTrackerThread_ = new QThread;
    pTracker_ = new EyeTracker;
    pTracker_->moveToThread(pTrackerThread_);
}

void MainWindow::setupSignalSlots() {
    connect(pTimer_, SIGNAL(timeout()), this, SLOT(updateTimer()));
    connect(pTracker_, SIGNAL(blinkDetected()), this, SLOT(onBlinkDetected()));
    connect(pTrackerThread_, SIGNAL(started()), pTracker_, SLOT(Start()));
    connect(pMainWindow_->buttonStart, SIGNAL(clicked()), this, SLOT(onButtonStartClicked()));
    connect(pMainWindow_->buttonEnd, SIGNAL(clicked()), this, SLOT(onButtonEndClicked()));
}

void MainWindow::start() {
    blinkCounter_ = 0;
    startTime_ = QDateTime::currentDateTime();

    pTimer_->start();
    pTrackerThread_->start();
    outputLog(" |----------------------| ");
}

void MainWindow::end() {
    pTimer_->stop();
    pTracker_->StopTracking();

    float blinkRate = static_cast<float>(blinkCounter_) * 60 / startTime_.secsTo(QDateTime::currentDateTime());
    outputLog(QString(" Eye blink rate: ").append(QString::number(blinkRate)));
    outputLog(" |----------------------| ");
}

void MainWindow::updateTimer() {
    timerCounter_++;
    qDebug() << timerCounter_;
    if (timerCounter_ > getInterval()) {
        qDebug() << "stimulate!";
        stimulate();
    }
}

void MainWindow::stimulate() {
    pFlashWidget_->Flash();
    resetTimer();
}

void MainWindow::onBlinkDetected() {
    blinkCounter_++;
    resetTimer();
    outputLog(" blink detected");
}

void inline MainWindow::outputLog(const QString &msg) {
    QString fileName = QString("../../../log/").append(startTime_.toString()).append(".txt");
    QFile logFile(fileName);
    if (logFile.open(QFile::ReadWrite|QFile::Append|QFile::Text)) {
        QTextStream outStream(&logFile);
        outStream << QDateTime::currentDateTime().toString() << msg << "\n";
    }
}
