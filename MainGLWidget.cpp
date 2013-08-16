#include "MainGLWidget.h"

MainGLWidget::MainGLWidget(QWidget *parent) : QGLWidget(parent) {
    setupEyeTracker();
    setupFatigueTimer();
//    start();
}

MainGLWidget::~MainGLWidget() {
    if (pFatigueTimer)  delete pFatigueTimer;
    if (texture)        delete texture;
    if (fboScene)       delete fboScene;
    if (fboVBlur)       delete fboVBlur;
    if (fboHBlur)       delete fboHBlur;
    if (shaderProgram)  delete shaderProgram;
    if (vertShader)     delete vertShader;
    if (fragShader)     delete fragShader;
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

void MainGLWidget::setupShader(const QString &vshader, const QString &fshader) {
    shaderProgram = new QGLShaderProgram;

    QFileInfo vsh(vshader);
    if (vsh.exists()) {
        vertShader = new QGLShader(QGLShader::Vertex);
        if (vertShader->compileSourceFile(vshader)) {
            shaderProgram->addShader(vertShader);
        }
    }

    QFileInfo fsh(fshader);
    if (fsh.exists()) {
        fragShader = new QGLShader(QGLShader::Fragment);
        if (fragShader->compileSourceFile(fshader)) {
            shaderProgram->addShader(fragShader);
        }
    }

    shaderProgram->link();
//    shaderProgram->bind();
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

    glGenTextures(2, texture);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    fboScene = new QGLFramebufferObject(img.width(), img.height());
    fboHBlur = new QGLFramebufferObject(128, 128);
    fboVBlur = new QGLFramebufferObject(128, 128);
}

void MainGLWidget::initializeGL() {
    setupGLTextures();
    setupShader(":/blur.vert", ":/blur.frag");

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void MainGLWidget::paintGL() {
    fboScene->bind(); {
        glViewport(0, 0, fboScene->width(), fboScene->height());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        if (toFlash) { return; }

        renderWithTexture(texture[0]);

        glViewport(0, 0, width(), height());
    } fboScene->release();

//    shaderProgram->setUniformValue("tex0", fboScene->texture());
//    shaderProgram->setUniformValue("offset", 1.0f/fboHBlur->width(), 0);
//    shaderProgram->setUniformValue("attenuation", 2.5f);

//    glViewport(0, 0, fboHBlur->width(), fboHBlur->height());
//    fboHBlur->bind(); {
//        glEnable(GL_TEXTURE_2D);
//        glBindTexture(GL_TEXTURE_2D, texture[0]);
//        glBegin(GL_QUADS);
//            glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,-1.0f);
//            glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,-1.0f);
//            glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, 1.0f);
//            glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
//        glEnd();
//        glFlush();
//        glFinish();
//        glDisable(GL_TEXTURE_2D);
//    } fboHBlur->release();

//    shaderProgram->setUniformValue("offset", 1.0f/fboVBlur->height(), 0);
//    shaderProgram->setUniformValue("attenuation", 2.5f);

//    glViewport(0, 0, fboVBlur->width(), fboVBlur->height());
//    fboHBlur->bind(); {
//        glEnable(GL_TEXTURE_2D);
//        glBindTexture(GL_TEXTURE_2D, texture[0]);
//        glBegin(GL_QUADS);
//            glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,-1.0f);
//            glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,-1.0f);
//            glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, 1.0f);
//            glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
//        glEnd();
//        glFlush();
//        glFinish();
//        glDisable(GL_TEXTURE_2D);
//    } fboHBlur->release();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    renderWithTexture(fboScene->texture());
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

void MainGLWidget::renderWithTexture(GLuint tex) {
//    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,-1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,-1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, 1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
    glEnd();
    glFlush();
//    glDisable(GL_TEXTURE_2D);
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
    toFlash = !toFlash;
    pFatigueTimer->start(toFlash ? 1000/60 : FATIGUE_LIMIT);
    outputLog(" stimulated ");
    update();
}

void MainGLWidget::onBlinkDectected() {
    blinkCounter++;
    pFatigueTimer->start(FATIGUE_LIMIT);
    outputLog(" blink detected ");
}

void MainGLWidget::outputLog(const QString &msg) {
    QString fileName = QString("./log/").append(timestamp.toString()).append(".txt");
    QFile logFile(fileName);
    if (logFile.open(QFile::ReadWrite|QFile::Append|QFile::Text)) {
        QTextStream outStream(&logFile);
        outStream << QDateTime::currentDateTime().toString() << msg << "\n";
    }
}
