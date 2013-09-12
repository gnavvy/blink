#include "PopupView.h"

PopupView::PopupView(QWidget *parent) : QWidget(parent) {
    resize(160, 120);
    setWindowFlags(Qt::Popup);
    QLabel *label = new QLabel("Blink", this);
    QFont font = label->font();
    font.setPointSize(48);
    font.setBold(true);
    label->setFont(font);
    label->setAlignment(Qt::AlignCenter);
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->addWidget(label);
    setLayout(gridLayout);
}
