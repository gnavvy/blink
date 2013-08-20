#ifndef MAINGLWIDGET_H
#define MAINGLWIDGET_H

#include <QTimer>
#include <QtOpenGL>
#include <QGLWidget>
#include <QGLShader>
#include <QGLFunctions>
#include <QtWebKitWidgets>

class MaskView : public QGLWidget {
    Q_OBJECT
public:
    explicit MaskView(QWidget *parent = 0);
    virtual ~MaskView();

    // control
    void reset();
    void flash();
    void blur();
    
signals:
    
public slots:
    void onRenderTimerTimeOut();
    void onFlashTimerTimeOut();
    void onBlurTimerTimeOut();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void keyPressEvent(QKeyEvent *event);

private:
    // ---- member ---- //
    const int   FATIGUE_LIMIT = 3000;  // 5s
    const int   BLUR_SIZE = 512;
    const int   FPS = 30;
    const int   WEB_VIEW_WIDTH = 1280;
    const int   WEB_VIEW_HEIGHT = 800;

    bool        blurring;
    bool        flashing;
    QImage      img;
    QTimer     *renderTimer;
    QTimer     *flashTimer;
    QTimer     *blurTimer;

    // opengl
    GLuint *texture = new GLuint[2];
    QGLFramebufferObject *fboScene;
    QGLFramebufferObject *fboBlurV;
    QGLFramebufferObject *fboBlur;
    QGLShaderProgram *blurShader;
    QGLShader *vertShader;
    QGLShader *fragShader;

    // blink detection
    int         blinkCounter = 0;
    float       blurRadius = 0.0f;

    // ---- function ---- //
    void setupTimers();
    void setupShader(const QString &vshader, const QString &fshader);

    void debug();

    void updateTexture();
    void renderFromTexture(GLuint tex);

    void outputLog(const QString &msg);
};

#endif // MAINGLWIDGET_H
