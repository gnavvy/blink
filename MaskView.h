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
    explicit MaskView(QWidget *cw);
    virtual ~MaskView();

    // control
//    void setContentWidget(QWidget *cw) {
//        this->contentView = cw;
//    }
    void updateLayout();
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
    const int   FPS = 60;
    bool        blurring;
    bool        flashing;

    QTimer     *renderTimer = nullptr;
    QTimer     *flashTimer  = nullptr;
    QTimer     *blurTimer   = nullptr;
    QWidget    *contentView = nullptr;
    int         contentWidth;
    int         contentHeight;

    // opengl
    QGLFramebufferObject *fboScene   = nullptr;
    QGLFramebufferObject *fboBlur    = nullptr;
    QGLShaderProgram     *blurShader = nullptr;
    QGLShader            *vertShader = nullptr;
    QGLShader            *fragShader = nullptr;

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
