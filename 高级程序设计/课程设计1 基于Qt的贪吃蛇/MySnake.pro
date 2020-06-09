#-------------------------------------------------
#
# Project created by QtCreator 2012-12-11T19:56:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MySnake
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    food.cpp \
    gamecontroller.cpp \
    MySnake.cpp \
    startwindow.cpp \
    end_window.cpp

HEADERS  += mainwindow.h \
    food.h \
    gamecontroller.h \
    constants.h \
    MySnake.h \
    startwindow.h \
    end_window.h

FORMS += \
    startwindow.ui \
    end_window.ui
