#-------------------------------------------------
#
# Project created by QtCreator 2013-07-31T14:37:59
#
#-------------------------------------------------

QMAKE_CXX       =  g++-4.8
QMAKE_CXXFLAGS  = -std=c++11
INCLUDEPATH    += /usr/local/include
INCLUDEPATH    += /usr/local/opt/qt5/include
LIBS           += -L/usr/local/lib
LIBS           += -L/usr/local/opt/qt5/lib
LIBS           += -lm -lopencv_core -lopencv_highgui -lopencv_objdetect -lopencv_imgproc

QMAKE_CFLAGS_X86_64             -= -arch x86_64 -Xarch_x86_64 -mmacosx-version-min=10.5
QMAKE_OBJECTIVE_CFLAGS_X86_64   -= -arch x86_64 -Xarch_x86_64 -mmacosx-version-min=10.5
QMAKE_CXXFLAGS_X86_64           -= -arch x86_64 -Xarch_x86_64 -mmacosx-version-min=10.5
QMAKE_LFLAGS_X86_64             -= -arch x86_64 -Xarch_x86_64 -mmacosx-version-min=10.5

QMAKE_LINK       = $$QMAKE_CXX
QMAKE_LINK_SHLIB = $$QMAKE_CXX

QT += core gui opengl widgets

TARGET = Blink
TEMPLATE = app

FORMS += \
    TaskImage1.ui \
    TaskImage2.ui

HEADERS += \
    EyeTracker.h \
    Utilities.h \
    TaskImage1.h \
    MainStackWidget.h \
    TaskImage2.h \
    TaskVideo.h \
    TaskText.h \
    MainGLWidget.h

SOURCES += \
    main.cpp \
    EyeTracker.cpp \
    TaskImage1.cpp \
    MainStackWidget.cpp \
    TaskImage2.cpp \
    TaskVideo.cpp \
    TaskText.cpp \
    MainGLWidget.cpp

OTHER_FILES += \
    blur.frag \
    blur.vert

RESOURCES += \
    resources.qrc
