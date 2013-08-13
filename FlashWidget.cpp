#include "FlashWidget.h"

FlashWidget::FlashWidget(QWidget *parent) : QWidget(parent) {
    pView_ = new QDeclarativeView; {
        pView_->setSource(QUrl("qrc:/ui.qml"));
        pView_->setAttribute(Qt::WA_TranslucentBackground);
        pView_->setStyleSheet("background:transparent;");
        pView_->setWindowFlags(Qt::FramelessWindowHint);
    }
}

FlashWidget::~FlashWidget() { }

void FlashWidget::Flash() {
    pView_->showFullScreen();
    pView_->raise();
    Sleeper::usleep(10);
    pView_->hide();
}
