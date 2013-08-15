#include "MainGLWidget.h"

MainGLWidget::MainGLWidget(QWidget *parent) : QGLWidget(parent) {
    setupEyeTracker();
    setupFatigueTimer();
    start();
}

MainGLWidget::~MainGLWidget() {
    delete pFatigueTimer;
    delete texture;
}

void MainGLWidget::setupEyeTracker() {
    pTrackerThread = new QThread;
    pTracker       = new EyeTracker;
    pTracker->moveToThread(pTrackerThread);
    connect(pTrackerThread, SIGNAL(started()), pTracker, SLOT(Start()));
    connect(pTracker, SIGNAL(blinkDetected()), this, SLOT(onBlinkDectected()));
    connect(pTracker, SIGNAL(finished()), pTrackerThread, SLOT(quit()));
    connect(pTrackerThread, SIGNAL(finished()), pTracker, SLOT(deleteLater()));
    connect(pTrackerThread, SIGNAL(finished()), pTrackerThread, SLOT(deleteLater()));
}

void MainGLWidget::setupFatigueTimer() {
    pFatigueTimer = new QTimer(this);
    connect(pFatigueTimer, SIGNAL(timeout()), this, SLOT(onFatigueTimerTimeOut()));
}

void MainGLWidget::initializeGL() {
    setupGLTextures();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void MainGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    if (toStimulate) { return; }

    glEnable(GL_TEXTURE_2D);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img.width(), img.height(), GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,-1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,-1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
    glEnd();
    glFlush();
    glFinish();
    glDisable(GL_TEXTURE_2D);
}

void MainGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void MainGLWidget::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
        case Qt::Key_Escape: stop(); break;
        default: event->ignore(); break;
    }
}

void MainGLWidget::setupGLTextures() {
    if (!img.load(":/imgs/bg.png")) {
        qDebug() << "cannot load bg.png";
        exit(EXIT_FAILURE);
    }

    img = QGLWidget::convertToGLFormat(img);
    if (img.isNull()) {
        qDebug() << "cannot convert to gl format";
        exit(EXIT_FAILURE);
    }

    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void MainGLWidget::start() {
    blinkCounter = 0;
    timestamp = QDateTime::currentDateTime();
    pFatigueTimer->start(FATIGUE_LIMIT);
    pTrackerThread->start();
    outputLog(" |----------------------| ");
}

void MainGLWidget::stop() {
    pTracker->StopTracking();
    pFatigueTimer->stop();

    float blinkRate = static_cast<float>(blinkCounter) * 60 / timestamp.secsTo(QDateTime::currentDateTime());
    outputLog(QString(" Eye blink rate: ").append(QString::number(blinkRate)));
    outputLog(" |----------------------| ");
}

// -------- slots -------- //
void MainGLWidget::onFatigueTimerTimeOut() {
    toStimulate = !toStimulate;
    pFatigueTimer->start(toStimulate ? 1000/60 : FATIGUE_LIMIT);
    outputLog(" stimulated ");
    update();
}

void MainGLWidget::onBlinkDectected() {
    blinkCounter++;
    pFatigueTimer->start(FATIGUE_LIMIT);
    outputLog(" blink detected ");
}

void MainGLWidget::outputLog(const QString &msg) {
    QString fileName = QString("../../../log/").append(timestamp.toString()).append(".txt");
    QFile logFile(fileName);
    if (logFile.open(QFile::ReadWrite|QFile::Append|QFile::Text)) {
        QTextStream outStream(&logFile);
        outStream << QDateTime::currentDateTime().toString() << msg << "\n";
    }
}
