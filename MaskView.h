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
    explicit MaskView(QWidget *contentWidget);
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
    bool        blurring;
    bool        flashing;

    QTimer     *renderTimer;
    QTimer     *flashTimer;
    QTimer     *blurTimer;

    QWidget    *contentView;
    int         contentWidth;
    int         contentHeight;

    // opengl
    QGLFramebufferObject *fboScene;
    QGLFramebufferObject *fboBlurV;
    QGLFramebufferObject *fboBlur;
    QGLShaderProgram     *blurShader;
    QGLShader            *vertShader;
    QGLShader            *fragShader;

    // blink detection
    int         blinkCounter = 0;
    float       blurRadius = 0.0f;

    // ---- function ---- //
    void setupTimers();
    void setupShader(const QString &vshader, const QString &fshader);
    void debug();
    void renderFromTexture(GLuint tex);
};

#endif // MAINGLWIDGET_H
