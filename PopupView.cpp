#include "PopupView.h"

PopupView::PopupView(QWidget *parent) : QWidget(parent) {
    resize(160, 120);
    setWindowFlags(Qt::Popup);

    timer = new QTimer(this);
    timer->setInterval(200);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));
    switchCount = 6;
}

void PopupView::flash() {
    qDebug() << "start";
    switchCount = 6;
    timer->start();
}

void PopupView::onTimerTimeout() {
    qDebug() << "timeout";
    isHidden() ? show() : hide();
    --switchCount > 0 ? timer->start() : timer->stop();
}
