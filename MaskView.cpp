#include "MaskView.h"

MaskView::MaskView(QWidget *contentWidget) {
    contentView = contentWidget;
    contentWidth = contentView->size().width();
    contentHeight = contentView->size().height();

    setupTimers();
}

MaskView::~MaskView() {
    if (blurTimer)      delete blurTimer;
    if (flashTimer)     delete flashTimer;
    if (renderTimer)    delete renderTimer;

    if (fboScene)       delete fboScene;
    if (fboBlur)        delete fboBlur;
    if (blurShader)     delete blurShader;
    if (vertShader)     delete vertShader;
    if (fragShader)     delete fragShader;
}

void MaskView::setupTimers() {
    blurTimer = new QTimer(this);
    connect(blurTimer, SIGNAL(timeout()), this, SLOT(onBlurTimerTimeOut()));

    flashTimer = new QTimer(this);
    flashTimer->setInterval(1000/60);
    connect(flashTimer, SIGNAL(timeout()), this, SLOT(onFlashTimerTimeOut()));

    renderTimer = new QTimer(this);
    renderTimer->setInterval(1000/FPS);
    connect(renderTimer, SIGNAL(timeout()), this, SLOT(onRenderTimerTimeOut()));
    renderTimer->start();
}

void MaskView::setupShader(const QString &vshader, const QString &fshader) {
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

void MaskView::initializeGL() {
    setupShader(":/blur.vert", ":/blur.frag");

    fboScene = new QGLFramebufferObject(contentWidth, contentHeight);
    fboBlur  = new QGLFramebufferObject(contentWidth, contentHeight);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void MaskView::paintGL() {
    if (flashing) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return;
    } else {
        glClearColor(0.2f, 0.2f, 0.2f, 0.0f);   // transparent
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    GLuint tex = bindTexture(contentView->grab());

    if (!blurring) {    // render glview using content from the contentWidget
        glViewport(0, 0, contentWidth, contentHeight);
        renderFromTexture(tex);
    } else {            // off screen blurring first, then on screen
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
        glViewport(0, 0, contentWidth, contentHeight);
        renderFromTexture(fboScene->texture());
    }
}

void MaskView::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    contentWidth = contentView->size().width();
    contentHeight = contentView->size().height();

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

void MaskView::debug() {
    fboScene->toImage().save("fboScene.png", "PNG");
    fboBlur->toImage().save("fboBlur.png", "PNG");
}

void MaskView::flash() {
    flashing = true;
    flashTimer->start();
}

void MaskView::blur() {
    blurring = true;
    blurTimer->start();
}

void MaskView::reset() {
    blurRadius = 0.0f;
    blurTimer->stop();
}

// -------- slots -------- //
void MaskView::onRenderTimerTimeOut() {
    update();
}

void MaskView::onFlashTimerTimeOut() {
    flashing = false;
    flashTimer->stop();
}

void MaskView::onBlurTimerTimeOut() {
    blurRadius += 0.01f;
    update();
}