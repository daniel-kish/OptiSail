#-------------------------------------------------
#
# Project created by QtCreator 2014-06-12T11:16:04
#
#-------------------------------------------------

QT       += core gui
CONFIG += qwt

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = optiSail
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    environment.cpp \
    pondview.cpp \
    sailboat.cpp \
    boatmodel.cpp

HEADERS  += mainwindow.h \
    environment.h \
    pondview.h \
    sailboat.h \
    boatmodel.h

FORMS    += mainwindow.ui
