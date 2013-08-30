#include "MainWidget.h"
#include <QApplication>

#include "MemTest.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MemTest mt;
    mt.show();

//    QRect res = QApplication::desktop()->screenGeometry(1);
//    MainWidget w;
//    w.move(res.x(), res.y());
//    w.resize(res.width(), res.height());
//    w.show();

    return app.exec();
}
