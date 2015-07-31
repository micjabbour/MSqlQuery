#-------------------------------------------------
#
# Project created by QtCreator 2015-07-04T18:44:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT       += sql

TARGET = msqlquery
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    mdbworker.cpp \
    msqldatabase.cpp \
    msqlquery.cpp \
    msqlquerymodel.cpp

HEADERS  += widget.h \
    mdbworker.h \
    msqldatabase.h \
    msqlquery.h \
    msqlquerymodel.h

FORMS    += widget.ui
