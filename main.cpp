#include "UserStudy1.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QRect res = QApplication::desktop()->screenGeometry(1);
    UserStudy1 us1;
    us1.move(res.x(), res.y());
    us1.resize(res.width(), res.height());
    us1.show();

    return app.exec();
}
