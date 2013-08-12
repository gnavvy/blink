#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QStackedWidget(parent), ui_(new Ui::MainWindow) {
    setupUI();
    setupTimer();
    setupWorker();
    start();
}

MainWindow::~MainWindow() {
    end();
    delete ui_;
    delete pFlashWidget_;
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
    pTimer_->setInterval(1000);
    connect(pTimer_, SIGNAL(timeout()), this, SLOT(updateTimer()));
    pTimer_->start();
}

void MainWindow::setupWorker() {
    pTrackerThread_ = new QThread;
    pTracker_ = new EyeTracker;
    pTracker_->moveToThread(pTrackerThread_);

    connect(pTracker_, SIGNAL(blinkDetected()), this, SLOT(onBlinkDetected()));
    connect(pTrackerThread_, SIGNAL(started()), pTracker_, SLOT(Start()));
}

void MainWindow::start() {
    blinkCounter_ = 0;
    startTime_ = QDateTime::currentDateTime();

    pTrackerThread_->start();
    outputLog(" |----------------------| ");
}

void MainWindow::end() {
    pTracker_->StopTracking();

    int timeSpan = startTime_.secsTo(QDateTime::currentDateTime());
    float blinkRate = static_cast<float>(blinkCounter_) * 60 / timeSpan;
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
