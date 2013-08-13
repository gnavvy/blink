#ifndef TASKVIDEO_H
#define TASKVIDEO_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <phonon/VideoPlayer>

class TaskVideo : public QWidget {
    Q_OBJECT
    
public:
    explicit TaskVideo(QWidget *parent = 0);
    virtual ~TaskVideo();
    
public slots:
    void onStartButton1Clicked();
    void onStartButton2Clicked();

private:
    QGridLayout *gridLayout_;
    QPushButton *startButton1_;
    QPushButton *startButton2_;

    Phonon::VideoPlayer *videoPlayer_;
};

#endif // TASKVIDEO_H
