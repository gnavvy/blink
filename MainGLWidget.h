#ifndef MAINGLWIDGET_H
#define MAINGLWIDGET_H

#include <QTimer>
#include <QtOpenGL>
#include <QDateTime>
#include <QGLWidget>
#include <QGLShader>
#include <QGLFunctions>
#include "EyeTracker.h"

class MainGLWidget : public QGLWidget {
    Q_OBJECT
public:
    explicit MainGLWidget(QWidget *parent = 0);
    virtual ~MainGLWidget();
    
signals:
    
public slots:
    void onFatigueTimerTimeOut();
    void onBlinkDectected();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void keyPressEvent(QKeyEvent *event);

private:
    // ---- member ---- //
    const int   FATIGUE_LIMIT = 5000;  // 5s
    const int   BLUR_SIZE = 512;

    bool        toFlash = false;
    bool        toBlur  = true;
    QImage      img;
    QTimer     *pFatigueTimer;
    QDateTime   timestamp;

    // opengl
    GLuint *texture = new GLuint[2];
    QGLFramebufferObject *fboScene;
    QGLFramebufferObject *fboBlurV;
    QGLFramebufferObject *fboBlur;
    QGLShaderProgram *blurShader;
    QGLShader *vertShader;
    QGLShader *fragShader;

    // blink detection
    EyeTracker *pTracker;
    QThread    *pTrackerThread;
    int         blinkCounter = 0;

    // ---- function ---- //
    void setupFatigueTimer();
    void setupEyeTracker();
    void setupGLTextures();
    void setupShader(const QString &vshader, const QString &fshader);

    void start();
    void stop();
    void debug();

    void renderFromTexture(GLuint tex);

    void outputLog(const QString &msg);
};

#endif // MAINGLWIDGET_H
