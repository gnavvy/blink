#include "MainWindow.h"
#include <QApplication>

#include "EyeTracker.h"

int main(int argc, char *argv[]) {
    EyeTracker et;
    et.Start();

//    QApplication a(argc, argv);
//    MainWindow w;

//    w.show();
    
//    return a.exec();
}
