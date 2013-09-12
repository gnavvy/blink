#include "MaskView.h"

MaskView::MaskView(QWidget *parent) : QGLWidget(parent) {
    setupTimers();
}

MaskView::~MaskView() {
    if (fboScene)       delete fboScene;
    if (fboBlur)        delete fboBlur;
}

void MaskView::setupTimers() {
    blurTimer = new QTimer(this);
    connect(blurTimer, SIGNAL(timeout()), this, SLOT(onBlurTimerTimeOut()));

    flashTimer = new QTimer(this);
    flashTimer->setInterval(1000/FPS);
    connect(flashTimer, SIGNAL(timeout()), this, SLOT(onFlashTimerTimeOut()));

    edgeTimer = new QTimer(this);
    edgeTimer->setInterval(1000/FPS*5);
    connect(edgeTimer, SIGNAL(timeout()), this, SLOT(onEdgeTimerTimeOut()));

    renderTimer = new QTimer(this);
    renderTimer->setInterval(1000/FPS);
    connect(renderTimer, SIGNAL(timeout()), this, SLOT(onRenderTimerTimeOut()));
    renderTimer->start();
}

void MaskView::setupShader(const QString &vshader, const QString &fshader) {
    blurShader = new QGLShaderProgram(this);

    QFileInfo vsh(vshader);
    if (vsh.exists()) {
        vertShader = new QGLShader(QGLShader::Vertex);
        if (vertShader->compileSourceFile(vshader))
            blurShader->addShader(vertShader);
    }

    QFileInfo fsh(fshader);
    if (fsh.exists()) {
        fragShader = new QGLShader(QGLShader::Fragment);
        if (fragShader->compileSourceFile(fshader))
            blurShader->addShader(fragShader);
    }

    blurShader->link();
}

void MaskView::initializeGL() {
    qDebug() << "initializeGL";

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    setupShader(":/blur.vert", ":/blur.frag");
}

void MaskView::paintGL() {
    if (flashEnabled) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return;
    } else {
        glClearColor(0.2f, 0.2f, 0.2f, 0.0f);   // transparent
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    if (!context) return;

    QPixmap pixmap = context->grab();

    if (edgeHighlightEnabled) {
        QPainter painter(&pixmap);
        QPen pen(Qt::white);
        pen.setWidth(2);
        painter.setPen(pen);
        painter.drawRect(204, 61, context->width()-408, context->height()-62);
    }

    GLuint tex = bindTexture(pixmap);

    if (!blurEnabled) {    // render glview using content from the contentWidget
        glViewport(0, 0, fboWidth, fboHeight);
        renderFromTexture(tex);
    } else {            // off screen blurring first, then on screen
        if (!fboScene)
            fboScene = new QGLFramebufferObject(fboWidth, fboHeight);

        if (!fboBlur)
            fboBlur  = new QGLFramebufferObject(fboWidth, fboHeight);

        // render to fbo
        glViewport(0, 0, fboScene->width(), fboScene->height());
        fboScene->bind(); {
            renderFromTexture(tex);
        } fboScene->release();

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

        // render to screen
        glViewport(0, 0, fboWidth, fboHeight);
        renderFromTexture(fboScene->texture());
    }

    deleteTexture(tex);
}

void MaskView::resizeGL(int w, int h) {
    qDebug() << "resizeGL";

    glViewport(0, 0, w, h);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    updateFboSize();

    delete fboScene;
    delete fboBlur;
    fboScene = new QGLFramebufferObject(fboWidth, fboHeight);
    fboBlur  = new QGLFramebufferObject(fboWidth, fboHeight);

    update();
}

void MaskView::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
        case Qt::Key_S: debug(); break;
        default: event->ignore(); break;
    }
}

void MaskView::renderFromTexture(GLuint tex) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, tex);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,-1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,-1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, 1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
    glEnd();
    glFlush();
}

// -------- ctrls -------- //
void MaskView::updateFboSize() {
    qDebug() << "updateLayout";
    fboWidth = context ? context->size().width() : 0;
    fboHeight = context ? context->size().height() : 0;
}

void MaskView::flash() {
    qDebug() << "flash";
    flashEnabled = true;
    flashTimer->start();
}

void MaskView::blur() {
    qDebug() << "blur";
    blurEnabled = true;
    blurTimer->start();
}

void MaskView::highlight() {
    qDebug() << "highlight";
    edgeHighlightEnabled = true;
    edgeTimer->start();
}

void MaskView::reset() {
    qDebug() << "reset";
    blurRadius = 0.0f;
    blurTimer->stop();
    edgeHighlightEnabled = false;
    edgeTimer->stop();
}

void MaskView::debug() {
    fboScene->toImage().save("fboScene.png", "PNG");
    fboBlur->toImage().save("fboBlur.png", "PNG");
}

// -------- slots -------- //
void MaskView::onRenderTimerTimeOut() {
    update();
}

void MaskView::onFlashTimerTimeOut() {
    flashEnabled = false;
    flashTimer->stop();  // flash once
}

void MaskView::onBlurTimerTimeOut() {
    blurRadius += 0.01f;
    update();
}

void MaskView::onEdgeTimerTimeOut() {
    edgeHighlightEnabled = !edgeHighlightEnabled;
    edgeTimer->start();  // flash continuously
}
