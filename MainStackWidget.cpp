#include "MainStackWidget.h"

MainStackWidget::MainStackWidget(QWidget *parent) : QStackedWidget(parent) {
    setupUI();
    setupConnects();
}

MainStackWidget::~MainStackWidget() {
    delete taskImage1_;
    delete taskImage2_;
    delete taskVideo_;
}

void MainStackWidget::setupUI() {
    this->resize(1440, 800);

    taskImage1_ = new TaskImage1;
    taskImage2_ = new TaskImage2;
    taskVideo_  = new TaskVideo;

    this->addWidget(taskImage1_);
    this->addWidget(taskImage2_);
    this->addWidget(taskVideo_);

    this->setCurrentWidget(taskVideo_);
}

void MainStackWidget::setupConnects() {
    connect(taskImage1_, SIGNAL(taskFinished()), this, SLOT(onTaskImage1Finished()));
}

void MainStackWidget::onTaskImage1Finished() {
    this->setCurrentWidget(taskImage2_);
}
