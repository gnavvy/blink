#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background:transparent;");
    setWindowFlags(Qt::FramelessWindowHint);

    view = new QDeclarativeView;
    view->setSource(QUrl("qrc:/ui.qml"));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(stimulate()));
    timer->start(3000);

    return;
}

MainWindow::~MainWindow() {
    delete view;
}

void MainWindow::stimulate() {
//    if (qrand() % 2) {
        flash();
//    } else {
//        blur();
//    }
}

void MainWindow::flash() {
    view->showFullScreen();
    view->raise();
    Sleeper::usleep(10); // cannot control this, quite disturbing
    view->hide();
}

void MainWindow::blur() {
    QGraphicsBlurEffect blur;
    blur.setBlurHints(QGraphicsBlurEffect::QualityHint);
    blur.setBlurRadius(10);
    view->setGraphicsEffect(&blur);
    Sleeper::usleep(1000);
    view->show();
    view->graphicsEffect()->setEnabled(false);
}
