#ifndef TASKTEXT_H
#define TASKTEXT_H

#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QGridLayout>
#include <QPushButton>

class TaskText : public QWidget {
    Q_OBJECT
public:
    explicit TaskText(QWidget *parent = 0);
    virtual ~TaskText();
signals:
    
public slots:
    
private:
    QGridLayout *gridLayout_;
    QLabel      *textLabel_;
    QPushButton *doneButton_;

};

#endif // TASKTEXT_H
