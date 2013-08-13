#include "TaskImage2.h"
#include "ui_TaskImage2.h"

TaskImage2::TaskImage2(QWidget *parent) : QWidget(parent), ui(new Ui::TaskImage2) {
    timerCounter_ = 10;
    ui->setupUi(this);

    img1.load(":/imgs/task_image_3.jpg");
    img2.load(":/imgs/task_image_4.jpg");

    ui->image->setPixmap(QPixmap::fromImage(img1));

    timer_ = new QTimer(this);
    connect(timer_, SIGNAL(timeout()), this, SLOT(onTimerTick()));
    timer_->start(1000);
}

TaskImage2::~TaskImage2() {
    delete ui;
    delete timer_;
}

void TaskImage2::onTimerTick() {
    if (timerCounter_-- > 0) {
        ui->stats->setText(QString::number(timerCounter_));
    } else {
        ui->image->setPixmap(QPixmap::fromImage(img2));
        ui->title->setText("Please write down what is missing from the 1st picture.");
    }
}
