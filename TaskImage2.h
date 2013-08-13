#ifndef TASKIMAGE2_H
#define TASKIMAGE2_H

#include <QTimer>
#include <QImage>
#include <QLabel>
#include <QWidget>

namespace Ui {
    class TaskImage2;
}

class TaskImage2 : public QWidget {
    Q_OBJECT
    
public:
    explicit TaskImage2(QWidget *parent = 0);
    virtual ~TaskImage2();
    
signals:
    void taskFinished();

public slots:
    void onTimerTick();

private:
    Ui::TaskImage2 *ui;
    QTimer *timer_;
    QImage img1, img2;
    int timerCounter_;
};

#endif // TASKIMAGE2_H
