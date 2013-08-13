#ifndef TASKIMAGE1_H
#define TASKIMAGE1_H

#include <vector>
#include <numeric>
#include <QDebug>
#include <QWidget>
#include <QPushButton>

namespace Ui {
    class TaskImage1;
}

class TaskImage1 : public QWidget {
    Q_OBJECT
    
public:
    explicit TaskImage1(QWidget *parent = 0);
    virtual ~TaskImage1();
    
signals:
    void taskFinished();

public slots:
    void onButtonClicked();

private:
    Ui::TaskImage1 *ui;
    int numDifferenceFound_ = 0;
    std::vector<int> diffFound_;
};

#endif // TASKIMAGE1_H
