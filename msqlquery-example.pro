#-------------------------------------------------
#
# Project created by QtCreator 2015-07-04T18:44:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
include(msqlquery/msqlquery.pri)

TARGET = msqlquery-example
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h  

FORMS    += widget.ui
