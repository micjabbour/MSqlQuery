QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT       += sql


INCLUDEPATH += $$PWD


SOURCES += $$PWD/mdbworker.cpp \
    $$PWD/msqldatabase.cpp \
    $$PWD/msqlquery.cpp \
    $$PWD/msqlquerymodel.cpp

HEADERS  += $$PWD/mdbworker.h \
    $$PWD/msqldatabase.h \
    $$PWD/msqlquery.h \
    $$PWD/msqlquerymodel.h
