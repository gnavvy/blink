#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QStackedWidget(parent), ui_(new Ui::MainWindow) {
    ui_->setupUi(this);
    pFlashWidget_ = new FlashWidget();

    pTimer_ = new QTimer(this);
    connect(pTimer_, SIGNAL(timeout()), this, SLOT(updateCounter()));
    pTimer_->start(1000);

    setupWorkInThread();
}

MainWindow::~MainWindow() {
    delete ui_;
    delete pTimer_;
    delete pTracker_;
    delete pTrackerThread_;
}

void MainWindow::setupWorkInThread() {
    pTrackerThread_ = new QThread;
    pTracker_ = new EyeTracker();
    pTracker_->moveToThread(pTrackerThread_);

    connect(pTrackerThread_, SIGNAL(started()), pTracker_, SLOT(Start()));  // start tracker when thread stats
    connect(pTracker_, SIGNAL(log(QString)), this, SLOT(log(QString)));
    connect(pTracker_, SIGNAL(blinkDetected()), this, SLOT(resetCounter()));
    pTrackerThread_->start();
}

void MainWindow::updateCounter() {
    counter_++;
    qDebug() << counter_;
    if (counter_ > interval()) {
        qDebug() << "stimulate!";
        stimulate();
    }
}

void MainWindow::stimulate() {
    pFlashWidget_->Flash();
    resetCounter();
}

