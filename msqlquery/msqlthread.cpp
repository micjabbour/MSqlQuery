#include "msqlthread.h"
#include <QGlobalStatic>

Q_GLOBAL_STATIC(MSqlThread, mSqlThread)

MSqlThread::MSqlThread(QObject *parent) {
    start();
}
MSqlThread* MSqlThread::instance() {
    return mSqlThread();
}
