#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QStackedWidget(parent), ui_(new Ui::MainWindow) {
    setupUI();
    setupTimer();
    setupWorker();
}

MainWindow::~MainWindow() {
    outputLog(" |----------------------|");

    delete ui_;
    delete pTimer_;
    delete pTracker_;
    delete pTrackerThread_;
}

void MainWindow::setupUI() {
    ui_->setupUi(this);
    pFlashWidget_ = new FlashWidget();
}

void MainWindow::setupTimer() {
    pTimer_ = new QTimer(this);
    connect(pTimer_, SIGNAL(timeout()), this, SLOT(updateCounter()));
    pTimer_->start(1000);
}

void MainWindow::setupWorker() {
    pTrackerThread_ = new QThread; {
        pTracker_ = new EyeTracker(); {
            pTracker_->moveToThread(pTrackerThread_);
            connect(pTracker_, SIGNAL(log(QString)), this, SLOT(outputLog(QString)));
            connect(pTracker_, SIGNAL(blinkDetected()), this, SLOT(onBlinkDetected()));
        }
        connect(pTrackerThread_, SIGNAL(started()), pTracker_, SLOT(Start()));
    }
    pTrackerThread_->start();

    outputLog(" |----------------------|");
}

void MainWindow::updateCounter() {
    counter_++;
    qDebug() << counter_;
    if (counter_ > getInterval()) {
        qDebug() << "stimulate!";
        stimulate();
    }
}

void MainWindow::stimulate() {
    pFlashWidget_->Flash();
    resetCounter();
}

void MainWindow::onBlinkDetected() {
    resetCounter();
    outputLog(" blink detected");
}

void inline MainWindow::outputLog(const QString &msg) {
    QFile logFile("../../../log.txt");
    if (logFile.open(QFile::ReadWrite|QFile::Append|QFile::Text)) {
        QTextStream outStream(&logFile);
        outStream << QDateTime::currentDateTime().toString() << msg << "\n";
    }
}
