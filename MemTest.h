#ifndef MEMTEST_H
#define MEMTEST_H

#include <QWidget>
#include <math.h>
#include <QFileDialog>
#include <QDebug>
#include <QLabel>
#include <QTimer>
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
    void initview();
    void check();

signals:
    
public slots:
    void started();
    void updateClock();
    void buttonPressed();

private:
    QVector<QIcon*> images;
    QMap<QIcon*, int> mapIconToNum;
    QVector<int> imagesSelected;
    QVector<QPushButton*> buttons;

    int numObjects, timeGiven;
    int correct, incorrect;

    QStackedLayout *main;
    QLabel *timerLabel, *resultsLabel;
    QTimer *timer;

};

#endif // MEMTEST_H
