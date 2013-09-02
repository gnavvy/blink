#include "MemTest.h"

#define TIME 30

MemTest::MemTest(QWidget *parent) : QWidget(parent) {
    numObjects = 4;
    numRound = 2;
    correct = incorrect = 0;
    timeGiven = TIME;
    loadImages("./res/memtest/");

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()), this, SLOT(updateClock()));

    stackLayout = new QStackedLayout();
    this->setLayout(stackLayout);

    initView();
}

int MemTest::randInt(int low, int hight) {
    return qrand() % ((hight + 1) - low) + low;
}

void MemTest::clearVariables(){
    buttons.clear();
    imagesSelected.clear();
    correct = incorrect = 0;
    timeGiven = TIME;
}

void MemTest::selectImages() {
    imagesSelected.clear();
    qsrand(QTime::currentTime().msec());
    while (imagesSelected.size() < numObjects) {
        int value = randInt(0, images.size()-1);
        if (!imagesSelected.contains(value))
            imagesSelected.push_back(value);
    }
}

void MemTest::initView() {
    while (stackLayout->count() != 0) {
        stackLayout->setCurrentIndex(0);
        stackLayout->removeWidget(stackLayout->currentWidget());
    }
    clearVariables();

    QWidget *startWidget = new QWidget();
    QGridLayout *startlayout = new QGridLayout();

    QLabel *instruction = new QLabel("In this task, you're given "+ QString::number(timeGiven) +" seconds to memories a set of "+ QString::number(numObjects) +" images.");
    QFont font = instruction->font();
    font.setPointSize(18);
    font.setBold(true);
    instruction->setFont(font);
    QPushButton *startButton = new QPushButton("Start");
    startButton->setFixedWidth(120);
    connect(startButton,SIGNAL(pressed()),this,SLOT(started()));

    startlayout->addWidget(instruction, 0, 0, Qt::AlignCenter);
    startlayout->addWidget(startButton, 1, 0, Qt::AlignTop|Qt::AlignHCenter);

    startWidget->setLayout(startlayout);
    stackLayout->addWidget(startWidget);

    // --------------------- //

    QWidget *memoryWidget = new QWidget();
    QGridLayout *memoryLayout = new QGridLayout();

    selectImages();
    int gridSize = static_cast<int>(ceil(sqrt(imagesSelected.size())));
    for (int i = 0; i < imagesSelected.size(); i++){
        QLabel *label = new QLabel();
        label->setPixmap(images[imagesSelected[i]]->pixmap(144,144));
        memoryLayout->addWidget(label, i/gridSize, i%gridSize, Qt::AlignCenter);
    }
    timerLabel = new QLabel(QString::number(timeGiven/60)+":"+QString("%1").arg(timeGiven%60,2,'f',0,'0'));
    timerLabel->setFont(font);

    memoryLayout->addWidget(timerLabel, gridSize, gridSize/2, Qt::AlignCenter);
    memoryWidget->setLayout(memoryLayout);
    stackLayout->addWidget(memoryWidget);

    // --------------------- //

    QWidget *selectionWidget = new QWidget();
    QGridLayout *selectionLayout = new QGridLayout();
    gridSize = static_cast<int>(ceil(sqrt(images.size())));

    for (int i = 0; i < images.size(); i++) {
        QPushButton * button = new QPushButton(*(images[i]),"");
        button->setIconSize(QSize(144,144));
        selectionLayout->addWidget(button, i/gridSize, i%gridSize);
        connect(button, SIGNAL(pressed()), this, SLOT(buttonPressed()));
        buttons.push_back(button);
    }
    resultsLabel = new QLabel(" 0 out "+QString::number(numObjects) + " objects found.");

    giveupnext = new QPushButton("Give up");
    connect(giveupnext,SIGNAL(pressed()), this, SLOT(nextGiveUp()));

    selectionLayout->addWidget(resultsLabel, gridSize, 0, 1, 2);
    selectionLayout->addWidget(giveupnext, gridSize, 2);

    selectionWidget->setLayout(selectionLayout);
    stackLayout->addWidget(selectionWidget);

    this->setLayout(stackLayout);
}

void MemTest::loadImages(QString directory) {
    QDir dir(directory);
    int count = 0;
    foreach (QFileInfo mitm, dir.entryInfoList()) {
        if (mitm.isDir()) { continue; }
        QIcon *image= new QIcon(mitm.absoluteFilePath());
        images.push_back(image);
        count++;
    }
}

void MemTest::updateClock() {
    timeGiven--;
    timerLabel->setText(QString::number(timeGiven/60)+":"+QString("%1").arg(timeGiven%60,2,'f',0,'0'));
    if (timeGiven <= 0)
        stackLayout->setCurrentIndex(2);
    else
        timer->start(1000);
}

void MemTest::buttonPressed() {
    for (int i = 0; i < buttons.size(); i++) {
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
    resultsLabel->setText(QString::number(correct) + " out "+QString::number(numObjects) + " objects found.");
    if (correct == imagesSelected.size()) {
        resultsLabel->setText(resultsLabel->text() + " You incorrectly indentified "+ QString::number(incorrect) +" images.");
        timer->stop();
        if (numRound > 0) {
            numRound--;
            giveupnext->setText("Next");
        } else {
            giveupnext->setText("Done");
        }
    }
}

void MemTest::nextGiveUp() {
    if(correct == imagesSelected.size()){
        if (numRound > 0) {
            numRound--;
            numObjects = 6;
            initView();
            stackLayout->setCurrentIndex(0);
        } else {
            stackLayout->setCurrentIndex(3);
        }
    }

}


void MemTest::started() {

    timerLabel->setText(QString::number(timeGiven/60)+":"+QString("%1").arg(timeGiven%60,2,'f',0,'0'));
    stackLayout->setCurrentIndex(1);
    timer->start(1000);
}
