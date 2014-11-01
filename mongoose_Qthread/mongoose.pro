#-------------------------------------------------
#
# Project created by QtCreator 2014-10-31T11:02:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

TARGET = mongoose
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mongoose.c \
    RestfulApi.cpp

HEADERS  += mainwindow.h \
    mongoose.h \
    RestfulApi.h

FORMS    += mainwindow.ui
