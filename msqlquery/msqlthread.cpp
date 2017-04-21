#include "msqlthread.h"

MSqlThread::MSqlThread(QObject *parent):SafeThread(parent) {
    m_worker = new QObject;
    connect(this, &QThread::finished, m_worker, &QObject::deleteLater);
    m_worker->moveToThread(this);
    start();
}
