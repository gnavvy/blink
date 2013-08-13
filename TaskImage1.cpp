#include "TaskImage1.h"
#include "ui_TaskImage1.h"

TaskImage1::TaskImage1(QWidget *parent) : QWidget(parent), ui(new Ui::TaskImage1) {
    for (int i = 0; i < 12; i++) {
        diffFound_.push_back(0);
    }

    ui->setupUi(this);

    connect(ui->b1a, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(ui->b1b, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(ui->b2a, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(ui->b2b, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(ui->b3a, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(ui->b3b, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(ui->b4a, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(ui->b4b, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(ui->b5a, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(ui->b5b, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(ui->b6a, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(ui->b6b, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(ui->b7a, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(ui->b7b, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(ui->b8a, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(ui->b8b, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(ui->b9a, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(ui->b9b, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(ui->b10a, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(ui->b10b, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(ui->b11a, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(ui->b11b, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(ui->b12a, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(ui->b12b, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
}

TaskImage1::~TaskImage1() {
    delete ui;
}

void TaskImage1::onButtonClicked() {
    QObject *theButton = sender();

    if      (ui->b1a == theButton || ui->b1b == theButton) { diffFound_[0] = 1; }
    else if (ui->b2a == theButton || ui->b2b == theButton) { diffFound_[1] = 1; }
    else if (ui->b3a == theButton || ui->b3b == theButton) { diffFound_[2] = 1; }
    else if (ui->b4a == theButton || ui->b4b == theButton) { diffFound_[3] = 1; }
    else if (ui->b5a == theButton || ui->b5b == theButton) { diffFound_[4] = 1; }
    else if (ui->b6a == theButton || ui->b6b == theButton) { diffFound_[5] = 1; }
    else if (ui->b7a == theButton || ui->b7b == theButton) { diffFound_[6] = 1; }
    else if (ui->b8a == theButton || ui->b8b == theButton) { diffFound_[7] = 1; }
    else if (ui->b9a == theButton || ui->b9b == theButton) { diffFound_[8] = 1; }
    else if (ui->b10a == theButton || ui->b10b == theButton) { diffFound_[9] = 1; }
    else if (ui->b11a == theButton || ui->b11b == theButton) { diffFound_[10] = 1; }
    else if (ui->b12a == theButton || ui->b12b == theButton) { diffFound_[11] = 1; }

    int sum = std::accumulate(diffFound_.begin(), diffFound_.end(), 0);
    ui->stats->setText("Found: " + QString::number(sum) + " / 12");

    if (sum == 12) {
        emit taskFinished();
    }
}
