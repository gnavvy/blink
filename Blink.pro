#-------------------------------------------------
#
# Project created by QtCreator 2013-07-31T14:37:59
#
#-------------------------------------------------

QMAKE_CXX       =  g++-4.8
QMAKE_CXXFLAGS  = -std=c++11
INCLUDEPATH     = -I/usr/local/include
LIBS            = -L/usr/local/lib -lm -lopencv_core -lopencv_highgui -lopencv_objdetect -lopencv_imgproc

QMAKE_CFLAGS_X86_64             -= -arch x86_64 -Xarch_x86_64 -mmacosx-version-min=10.5
QMAKE_OBJECTIVE_CFLAGS_X86_64   -= -arch x86_64 -Xarch_x86_64 -mmacosx-version-min=10.5
QMAKE_CXXFLAGS_X86_64           -= -arch x86_64 -Xarch_x86_64 -mmacosx-version-min=10.5
QMAKE_LFLAGS_X86_64             -= -arch x86_64 -Xarch_x86_64 -mmacosx-version-min=10.5

QMAKE_LINK       = $$QMAKE_CXX
QMAKE_LINK_SHLIB = $$QMAKE_CXX

QT += core gui declarative

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Blink
TEMPLATE = app

FORMS +=

HEADERS += \
    MainWindow.h \
    EyeTracker.h

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    EyeTracker.cpp

OTHER_FILES += \
    ui.qml

RESOURCES += \
    resources.qrc
