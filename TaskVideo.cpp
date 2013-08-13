#include "TaskVideo.h"

TaskVideo::TaskVideo(QWidget *parent) : QWidget(parent) {
    gridLayout_ = new QGridLayout(this);

    videoPlayer_ = new Phonon::VideoPlayer(Phonon::VideoCategory, this);

    connect(videoPlayer_, SIGNAL(finished()), videoPlayer_, SLOT(deleteLater()));
    gridLayout_->addWidget(videoPlayer_, 0, 0, 1, 2);

    startButton1_ = new QPushButton("TED Talk", this);
    startButton2_ = new QPushButton("Movie Trailer", this);
    connect(startButton1_, SIGNAL(clicked()), this, SLOT(onStartButton1Clicked()));
    connect(startButton2_, SIGNAL(clicked()), this, SLOT(onStartButton2Clicked()));
    gridLayout_->addWidget(startButton1_, 1, 0, 1, 1);
    gridLayout_->addWidget(startButton2_, 1, 1, 1, 1);

    setLayout(gridLayout_);
}

TaskVideo::~TaskVideo() {
    delete gridLayout_;
    delete startButton1_;
    delete startButton2_;
    delete videoPlayer_;
}

void TaskVideo::onStartButton1Clicked() {
    videoPlayer_->load(Phonon::MediaSource("../../../imgs/Ted.mp4"));
    videoPlayer_->play();
}

void TaskVideo::onStartButton2Clicked() {
    videoPlayer_->load(Phonon::MediaSource("../../../imgs/Trailer.mp4"));
    videoPlayer_->play();
}
