QT       += core sql


INCLUDEPATH += $$PWD

DEFINES += M_SQL_QUERY_DEBUG

SOURCES += $$PWD/mdbworker.cpp \
    $$PWD/msqldatabase.cpp \
    $$PWD/msqlquery.cpp \
    $$PWD/msqlquerymodel.cpp

HEADERS  += $$PWD/mdbworker.h \
    $$PWD/msqldatabase.h \
    $$PWD/msqlquery.h \
    $$PWD/msqlquerymodel.h
