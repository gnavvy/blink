#include "MainWidget.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QRect res = QApplication::desktop()->screenGeometry(1);
    MainWidget w;
    w.move(res.x(), res.y());
    w.resize(res.width(), res.height());
    w.show();

    return app.exec();
}
