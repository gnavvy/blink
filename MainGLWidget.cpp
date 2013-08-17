#include "MainGLWidget.h"

MainGLWidget::MainGLWidget(QWidget *parent) : QGLWidget(parent) {
    setupEyeTracker();
    setupTimers();
    start();
}

MainGLWidget::~MainGLWidget() {
    if (fatigueTimer)   delete fatigueTimer;
    if (blurTimer)      delete blurTimer;
    if (texture)        delete texture;
    if (fboScene)       delete fboScene;
    if (fboBlur)        delete fboBlur;
    if (blurShader)     delete blurShader;
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

void MainGLWidget::setupTimers() {
    fatigueTimer = new QTimer(this);
    connect(fatigueTimer, SIGNAL(timeout()), this, SLOT(onFatigueTimerTimeOut()));

    blurTimer = new QTimer(this);
    connect(blurTimer, SIGNAL(timeout()), this, SLOT(onBlurTimerTimeOut()));
}

void MainGLWidget::setupShader(const QString &vshader, const QString &fshader) {
    blurShader = new QGLShaderProgram;

    QFileInfo vsh(vshader);
    if (vsh.exists()) {
        vertShader = new QGLShader(QGLShader::Vertex);
        if (vertShader->compileSourceFile(vshader)) {
            blurShader->addShader(vertShader);
        }
    }

    QFileInfo fsh(fshader);
    if (fsh.exists()) {
        fragShader = new QGLShader(QGLShader::Fragment);
        if (fragShader->compileSourceFile(fshader)) {
            blurShader->addShader(fragShader);
        }
    }

    blurShader->link();
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
    fboBlur = new QGLFramebufferObject(img.width(), img.height());
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
    // render to fbo
    glViewport(0, 0, fboScene->width(), fboScene->height());
    fboScene->bind(); {
        renderFromTexture(texture[0]);
    } fboScene->release();

    if (toBlur) {
        blurShader->bind();
        blurShader->setUniformValue("tex0", fboScene->texture());
        blurShader->setUniformValue("radius", blurRadius);

        // blur horizontally
        blurShader->setUniformValue("offset", 1.0f/fboScene->width(), 0.0f);
        fboBlur->bind(); {
            renderFromTexture(fboScene->texture());
        } fboBlur->release();

        // blur vertically
        blurShader->setUniformValue("offset", 0.0f, 1.0f/fboScene->height());
        fboScene->bind(); {
            renderFromTexture(fboBlur->texture());
        } fboScene->release();

        blurShader->release();
    }

//    if (flashing) { return; }

    // render to screen
    glViewport(0, 0, width(), height());
    renderFromTexture(fboScene->texture());
}

void MainGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void MainGLWidget::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
        case Qt::Key_Escape: stop(); break;
        case Qt::Key_S: debug(); break;
        default: event->ignore(); break;
    }
}

void MainGLWidget::renderFromTexture(GLuint tex) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glBindTexture(GL_TEXTURE_2D, tex);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,-1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,-1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, 1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
    glEnd();
    glFlush();
}

void MainGLWidget::start() {
    blinkCounter = 0;
    timestamp = QDateTime::currentDateTime();
    fatigueTimer->start(FATIGUE_LIMIT);
    pTrackerThread->start();
    outputLog(" |----------------------| ");
}

void MainGLWidget::stop() {
    pTracker->StopTracking();
    fatigueTimer->stop();

    float blinkRate = static_cast<float>(blinkCounter) * 60 / timestamp.secsTo(QDateTime::currentDateTime());
    outputLog(QString(" Eye blink rate: ").append(QString::number(blinkRate)));
    outputLog(" |----------------------| ");
}

void MainGLWidget::debug() {
    fboScene->toImage().save("fboScene.png", "PNG");
    fboBlur->toImage().save("fboBlur.png", "PNG");
}

// -------- slots -------- //
void MainGLWidget::onFatigueTimerTimeOut() {
    if (toFlash) {
        flashing = !flashing;
        fatigueTimer->start(flashing ? 1000/60 : FATIGUE_LIMIT);
    }

    if (toBlur) {
        blurTimer->start(1000/60);
        fatigueTimer->stop();
    }

    outputLog(" stimulated ");
    update();
}

void MainGLWidget::onBlurTimerTimeOut() {
    blurRadius += 0.01f;
    update();
}

void MainGLWidget::onBlinkDectected() {
    blinkCounter++;

    blurRadius = 0.0f;
    blurTimer->stop();

    fatigueTimer->start(FATIGUE_LIMIT);
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
