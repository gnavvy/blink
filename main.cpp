#include "MainWindow.h"
#include "MainStackWidget.h"
#include "MainGLWidget.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainGLWidget w;
    w.showFullScreen();

//    MainWindow w;
//    w.show();

//    MainStackWidget w;
//    w.show();

    return a.exec();
}
