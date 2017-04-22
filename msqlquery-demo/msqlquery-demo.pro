#-------------------------------------------------
#
# MSqlQuery demo application:
#----------------------------
# an example application that uses MSqlQuery.
#-------------------------------------------------

QT       += core gui widgets


# you need to add the following line in your 
# application's .pro file
include(msqlquery/msqlquery.pri)

TARGET = msqlquery-demo

TEMPLATE = app


SOURCES += main.cpp\
    querydemowidget.cpp \
    modeldemowidget.cpp \
    loadingdialog.cpp

HEADERS  += \    
    querydemowidget.h \
    modeldemowidget.h \
    loadingdialog.h

FORMS    += \
    querydemowidget.ui \
    modeldemowidget.ui
