#include "MainWindow.h"
#include "MainStackWidget.h"
#include "TaskImage1.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

//    MainWindow w;
//    w.show();

    MainStackWidget w;
    w.show();

    return a.exec();
}
