#include "msqlquery.h"
#include "mdbworker.h"
#include <QEventLoop>
#include <QCoreApplication>
#include "qthreadutils.h"
#include "msqlthread.h"

MSqlQuery::MSqlQuery(QObject *parent, MSqlDatabase db)
    : QObject(parent), db(db), m_isBusy(false), currentItem(-1), lastSuccess(true),
      shouldEmitGotResult(true) {
    w= new MSqlQueryWorker();
    w->moveToThread(MSqlThread::instance());
    auto w= this->w; //in order to capture w by value
    //   ^^^^^^^^^^ is there a better way to do this without using c++14 initializing capture??
    //TODO: make sure slots invocation order in the target thread matches signal emitting order
    PostToWorker(w, [=]{
        w->q = new QSqlQuery(db.connectionName());
    });
}

MSqlQuery::~MSqlQuery() {
    w->deleteLater();
}

bool MSqlQuery::prepare(const QString &query) {
    auto w= this->w; //in order to capture w by value
    return CallByWorker(w, [=]{
        return w->q->prepare(query);
    });
}

void MSqlQuery::addBindValue(const QVariant &val, QSql::ParamType paramType)
{
    auto w= this->w; //in order to capture w by value
    PostToWorker(w, [=]{
        w->q->addBindValue(val, paramType);
    });
}

void MSqlQuery::bindValue(const QString &placeholder, const QVariant &val, QSql::ParamType paramType)
{
    auto w= this->w;
    PostToWorker(w, [=]{
        w->q->bindValue(placeholder, val, paramType);
    });
}

bool MSqlQuery::execAsync(const QString &query)
{
    if(m_isBusy) return false;
    m_isBusy = true;
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlQueryExec",
                              Qt::QueuedConnection,
                              Q_ARG(MSqlQuery*, this),
                              Q_ARG(QSqlQuery*, qquery),
                              Q_ARG(QString, query));
    return true;
}

bool MSqlQuery::execAsync()
{
    if(m_isBusy) return false;
    m_isBusy = true;
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlQueryExec",
                              Qt::QueuedConnection,
                              Q_ARG(MSqlQuery*, this),
                              Q_ARG(QSqlQuery*, qquery));
    return true;
}

bool MSqlQuery::exec(const QString &query)
{
    //TODO: make exec fully sync using BlockingQueuedConnection call
    while(m_isBusy) //wait until this object is not busy executing other queries
    {
        QEventLoop loop;
        connect(this, SIGNAL(gotResults(bool)), &loop, SLOT(quit()));
        loop.exec(QEventLoop::ExcludeUserInputEvents);
    }
    //call the equivalent async function
    QEventLoop loop;
    connect(this, SIGNAL(gotResponseFromOtherThread()), &loop, SLOT(quit()));
    execAsync(query);
    shouldEmitGotResult = false;
    loop.exec(QEventLoop::ExcludeUserInputEvents); //wait for the query to finish
    shouldEmitGotResult = true;
    return lastSuccess;
    /*QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlQueryExec",
                              Qt::BlockingQueuedConnection,
                              Q_ARG(MSqlQuery*, this),
                              Q_ARG(QSqlQuery*, qquery),
                              Q_ARG(QString, query));
    QCoreApplication::processEvents();
    return lastSuccess;*/
}

bool MSqlQuery::exec()
{
    while(m_isBusy) //wait until this object is not busy executing other queries
    {
        QEventLoop loop;
        connect(this, SIGNAL(gotResults(bool)), &loop, SLOT(quit()));
        loop.exec(QEventLoop::ExcludeUserInputEvents);
    }
    //call the equivalent async function
    QEventLoop loop;
    connect(this, SIGNAL(gotResponseFromOtherThread()), &loop, SLOT(quit()));
    execAsync();
    shouldEmitGotResult = false;
    loop.exec(QEventLoop::ExcludeUserInputEvents); //wait for the query to finish
    shouldEmitGotResult = true;
    return lastSuccess;
    /*QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlQueryExec",
                              Qt::BlockingQueuedConnection,
                              Q_ARG(MSqlQuery*, this),
                              Q_ARG(QSqlQuery*, qquery));
    QCoreApplication::processEvents();
    return lastSuccess;*/
}

void MSqlQuery::setResults(const QList<QSqlRecord> &res, bool success)
{
    m_isBusy = false;
    if(!success)
    {
        lastSuccess = false;
        records.clear();
        currentItem = -1;
    }
    else
    {
        lastSuccess = true;
        records = res;
        currentItem = -1;
    }
    emit gotResponseFromOtherThread();
    if(shouldEmitGotResult)
        emit gotResults(success);
}

bool MSqlQuery::next()
{
    if(records.isEmpty()) return false;
    if(++currentItem >= records.size()) return false;
    return true;
}

QSqlRecord MSqlQuery::record()
{
    return records.at(currentItem);
}

bool MSqlQuery::seek(int index)
{
    if(index >= records.size()) return false;
    else
    {
        currentItem = index;
        return true;
    }
}

QVariant MSqlQuery::lastInsertId()const
{
    QVariant result;
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlQueryLastInsertId",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(QVariant, result),
                              Q_ARG(QSqlQuery*, qquery));
    return result;
}
