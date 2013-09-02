#ifndef MEMTEST_H
#define MEMTEST_H

#include <QWidget>
#include <math.h>
#include <QFileDialog>
#include <QDebug>
#include <QLabel>
#include <QTimer>
#include <QTime>
#include <QGridLayout>
#include <QStackedLayout>
#include <QPushButton>
#include <QDateTime>

class MemTest : public QWidget {
    Q_OBJECT
public:
    explicit MemTest(QWidget *parent = 0);
    
    int randInt(int low, int hight);
    void selectImages();
    void loadImages(QString directory);
    void initView();
    void clearVariables();

signals:
    
public slots:
    void started();
    void updateClock();
    void buttonPressed();
    void nextGiveUp();

private:
    QVector<QIcon*> images;
    QVector<int> imagesSelected;
    QVector<QPushButton*> buttons;

    int numRound;
    int numObjects, timeGiven;
    int correct, incorrect;

    QPushButton *giveupnext;
    QStackedLayout *stackLayout;
    QLabel *timerLabel, *resultsLabel;
    QTimer *timer;

    void newRound();
};

#endif // MEMTEST_H
