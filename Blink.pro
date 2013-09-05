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

QT += core gui opengl widgets webkitwidgets multimedia multimediawidgets

TARGET = Blink
TEMPLATE = app

CONFIG += debug

HEADERS += \
    EyeTracker.h \
    MaskView.h \
    MemTest.h \
    UserStudy1.h \
    UserStudy2.h

SOURCES += \
    main.cpp \
    EyeTracker.cpp \
    MaskView.cpp \
    MemTest.cpp \
    UserStudy1.cpp \
    UserStudy2.cpp

OTHER_FILES += \
    blur.frag \
    blur.vert

RESOURCES += \
    resources.qrc
