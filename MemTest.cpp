#include "MemTest.h"

MemTest::MemTest(QWidget *parent) : QWidget(parent) {
    timeGiven = 10;
    numObjects = 8;
    correct = incorrect = 0;
    loadImages("./res/memtest/");
    initview();
}

int MemTest::randInt(int low, int hight) {
    return qrand() % ((hight + 1) - low) + low;
}

void MemTest::selectImages() {
    qsrand(QDateTime::currentDateTime().toTime_t());
    while (imagesSelected.size() < numObjects) {
        int value = randInt(0, images.size()-1);
        if (!imagesSelected.contains(value))
            imagesSelected.push_back(value);
    }
}


void MemTest::initview() {
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()), this, SLOT(updateClock()));

    main = new QStackedLayout();

    QWidget *startwidget = new QWidget();
    QGridLayout *startlayout = new QGridLayout();

    QLabel *instruction = new QLabel("Your given 2 minutes to memories a set of images.");
    QPushButton *startbutton = new QPushButton("start");

    connect(startbutton,SIGNAL(pressed()),this,SLOT(started()));

    startlayout->addWidget(instruction,0,0);
    startlayout->addWidget(startbutton,1,0);

    startwidget->setLayout(startlayout);
    main->addWidget(startwidget);


    QWidget *memorywidget = new QWidget();
    QGridLayout *memorylayout = new QGridLayout();

    selectImages();
    int gridsize = sqrt(imagesSelected.size()) + 1;
    for(unsigned int i = 0; i < imagesSelected.size(); i++){
        QLabel * label = new QLabel();
        label->setPixmap(images[imagesSelected[i]]->pixmap(100,100));
        memorylayout->addWidget(label,i / gridsize,i % gridsize);
    }
    QLabel *labeltimer = new QLabel("Timer: ");
    timerLabel = new QLabel();
    timerLabel->setText(QString::number(timeGiven/60)+":"+QString("%1").arg(timeGiven%60,2,'f',0,'0'));

    memorylayout->addWidget(labeltimer ,gridsize, 0);
    memorylayout->addWidget(timerLabel,gridsize, 1);
    memorywidget->setLayout(memorylayout);
    main->addWidget(memorywidget);

    QWidget *selectionwidget = new QWidget();
    QGridLayout *selectionlayout = new QGridLayout();
    gridsize = sqrt(images.size());

    for(unsigned int i = 0; i < images.size(); i++){
        QPushButton * button = new QPushButton(*(images[i]),"");
        button->setIconSize(QSize(100,100));
        selectionlayout->addWidget(button,i / gridsize,i % gridsize);
        connect(button, SIGNAL(pressed()), this, SLOT(buttonPressed()));
        buttons.push_back(button);
    }
    selectionwidget->setLayout(selectionlayout);
    main->addWidget(selectionwidget);

    QWidget *resultswidget = new QWidget();
    QGridLayout *resultslayout = new QGridLayout();

    resultsLabel = new QLabel();
    resultslayout->addWidget(resultsLabel,0,0);

    resultswidget->setLayout(resultslayout);
    main->addWidget(resultswidget);

    this->setLayout(main);

}

void MemTest::loadImages(QString directory) {
    QDir dir(directory);
    int count = 0;
    foreach (QFileInfo mitm, dir.entryInfoList()) {
        if (mitm.isDir()) { continue; }
//        qDebug() << mitm.absoluteFilePath();
        QIcon *image= new QIcon(mitm.absoluteFilePath());
        images.push_back(image);
        mapIconToNum[image] = count;
        count++;
    }
}

void MemTest::updateClock() {
    timeGiven--;
    timerLabel->setText(QString::number(timeGiven/60)+":"+QString("%1").arg(timeGiven%60,2,'f',0,'0'));
    if (timeGiven <= 0)
        main->setCurrentIndex(2);
    else
        timer->start(1000);
}

void MemTest::buttonPressed() {
    for (unsigned int i = 0; i < buttons.size(); i++) {
        if (buttons[i]->isDown()) {
            if (imagesSelected.contains(i)) {
                qDebug() << "Correct";
                buttons[i]->setIcon(QIcon("./res/right.png"));
                correct++;
            } else {
                qDebug() << "Wrong";
                buttons[i]->setIcon(QIcon("./res/wrong.png"));
                incorrect++;
            }
        }
    }
    if (correct == imagesSelected.size()) {
        resultsLabel->setText("You incorrectly indentified "+ QString::number(incorrect) +" images.");
        timer->stop();
        main->setCurrentIndex(3);
    }
}

void MemTest::started() {
    main->setCurrentIndex(1);
    timer->start(1000);
}
