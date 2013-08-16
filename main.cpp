#include "MainGLWidget.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainGLWidget w;
    w.showMaximized();

    return a.exec();
}
