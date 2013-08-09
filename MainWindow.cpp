#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    pView_ = new QDeclarativeView; {
        pView_->setSource(QUrl("qrc:/ui.qml"));
        pView_->setAttribute(Qt::WA_TranslucentBackground);
        pView_->setStyleSheet("background:transparent;");
        pView_->setWindowFlags(Qt::FramelessWindowHint);
    }

    pTimer_ = new QTimer(this);
    connect(pTimer_, SIGNAL(timeout()), this, SLOT(update()));
    pTimer_->start(1000);

    pTracker_ = new EyeTracker;
    connect(pTracker_, SIGNAL(blinked()), this, SLOT(blinked()));
    pTracker_->Start();
}

MainWindow::~MainWindow() {
    delete pView_;
    delete pTracker_;
    delete pTimer_;
}

void MainWindow::update() {
    counter_++;
    qDebug() << counter_;
    if (counter_ > interval()) {
        stimulate();
    }
}

void MainWindow::stimulate() {
    flash();
    resetCounter();
}

void MainWindow::flash() {
    pView_->showFullScreen();
    pView_->raise();
    Sleeper::usleep(10); // cannot control this, quite disturbing
    pView_->hide();
}

void MainWindow::blur() {
    QGraphicsBlurEffect blur;
    blur.setBlurHints(QGraphicsBlurEffect::QualityHint);
    blur.setBlurRadius(10);
    pView_->setGraphicsEffect(&blur);
    Sleeper::usleep(1000);
    pView_->show();
    pView_->graphicsEffect()->setEnabled(false);
}
