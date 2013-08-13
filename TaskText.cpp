#include "TaskText.h"

TaskText::TaskText(QWidget *parent) : QWidget(parent) {
    gridLayout_ = new QGridLayout(this);
    textLabel_ = new QLabel(this);

    QImage textSnapshot(":/imgs/task_text_1.png");
    QPixmap pixmap = QPixmap::fromImage(textSnapshot).scaledToWidth(1400, Qt::SmoothTransformation);
    textLabel_->setPixmap(pixmap);

    doneButton_ = new QPushButton("Done", this);

    gridLayout_->addWidget(textLabel_);
    gridLayout_->addWidget(doneButton_);

    setLayout(gridLayout_);
}

TaskText::~TaskText() {
    delete gridLayout_;
    delete textLabel_;
    delete doneButton_;
}
