#include "UserStudy1.h"
#include "UserStudy2.h"
#include <QApplication>
#include "PopupView.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QRect res = QApplication::desktop()->screenGeometry(1);

//    UserStudy1 us;
    UserStudy2 us;
    us.move(res.x(), res.y());
    us.resize(res.width(), res.height());
    us.show();

//    PopupView pv;
//    pv.show();

    return app.exec();
}
