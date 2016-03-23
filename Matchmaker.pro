#-------------------------------------------------
#
# Project created by QtCreator 2014-07-22T12:42:46
#
#-------------------------------------------------

LIBS += -lopengl32

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets opengl


TARGET = Matchmaker
TEMPLATE = app
CONFIG += windows


SOURCES += main.cpp\
        mainwindow.cpp \
    glmeshselectwidget.cpp \
    gltextureselectwidget.cpp \
    mathalgorithms.cpp \
    glfinalwidget.cpp \
    glprogresswidget.cpp

HEADERS  += mainwindow.h \
    glmeshselectwidget.h \
    gltextureselectwidget.h \
    mathalgorithms.h \
    glfinalwidget.h \
    glprogresswidget.h

FORMS    +=

RESOURCES += \
    defaultImages.qrc
