#ifndef MAINSTACKWIDGET_H
#define MAINSTACKWIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include "TaskText.h"
#include "TaskImage1.h"
#include "TaskImage2.h"
#include "TaskVideo.h"

class MainStackWidget : public QStackedWidget {
    Q_OBJECT
public:
    explicit MainStackWidget(QWidget *parent = 0);
    virtual ~MainStackWidget();
    
signals:
    
public slots:
    void onTaskImage1Finished();


private:
    // ---- member ----/
    TaskText   *taskText_;
    TaskImage1 *taskImage1_;
    TaskImage2 *taskImage2_;
    TaskVideo  *taskVideo_;

    // ---- function ----/
    void setupUI();
    void setupConnects();
};

#endif // MAINSTACKWIDGET_H
