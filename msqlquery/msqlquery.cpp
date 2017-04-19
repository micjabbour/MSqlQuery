#include "msqlquery.h"
#include "mdbworker.h"
#include <QEventLoop>
#include <QCoreApplication>
#include "qthreadutils.h"
#include "msqlthread.h"
#include <QMutexLocker>

MSqlQuery::MSqlQuery(QObject *parent, MSqlDatabase db)
    : QObject(parent), db(db), m_isBusy(false), currentItem(-1), lastSuccess(true),
      shouldEmitGotResult(true) {
    w= new MSqlQueryWorker();
    //connect func from worker to this instance's signal
    //this will make the signal get emitted from the MSqlQuery thread (instead of the worker thread)
    connect(w, &MSqlQueryWorker::resultsReady, this, &MSqlQuery::resultsReady);
    w->moveToThread(MSqlThread::instance());
    auto w= this->w; //in order to capture w by value
    //   ^^^^^^^^^^ is there a better way to do this without using c++14 initializing capture??
    PostToWorker(w, [=]{
        w->q = new QSqlQuery(db.connectionName());
    });
}

MSqlQuery::~MSqlQuery() {
    w->deleteLater();
}

void MSqlQuery::prepare(const QString &query) {
    w->prepare(query);
}

void MSqlQuery::addBindValue(const QVariant &val, QSql::ParamType paramType)
{
    w->addBindValue(val, paramType);
}

void MSqlQuery::bindValue(const QString &placeholder, const QVariant &val, QSql::ParamType paramType)
{
    w->bindValue(placeholder, val, paramType);
}

void MSqlQuery::execAsync(const QString &query)
{
    w->prepare(query);
    w->execAsync();
}

void MSqlQuery::execAsync()
{
    w->execAsync();
}

bool MSqlQuery::exec(const QString &query)
{
    w->prepare(query);
    exec();
}

bool MSqlQuery::exec()
{
    w->setNextQueryReady(true);
    auto w= this->w; //in order to capture w by value
    //TODO: find a way to block ready signal when using sync API
    CallByWorker(w, [=]{
        w->execNextQuery();
    });
    bool success = w->lastError().type()==QSqlError::NoError;
    return success;
}

bool MSqlQuery::next()
{
    return w->next();
}

QSqlRecord MSqlQuery::record() const
{
    return w->record();
}

bool MSqlQuery::seek(int index)
{
    return w->seek(index);
}

QVariant MSqlQuery::lastInsertId()const
{
    return w->lastInsertId();
}

void MSqlQueryWorker::prepare(const QString &query) {
    QMutexLocker locker(&mutex);
    Q_UNUSED(locker)
    m_nextQuery.isReady = false;
    m_nextQuery.placeHolderBinds.clear();
    m_nextQuery.positionalBinds.clear();
    m_nextQuery.prepareStr = query;
}

void MSqlQueryWorker::bindValue(const QString &placeholder, const QVariant &val, QSql::ParamType paramType) {
    QMutexLocker locker(&mutex);
    Q_UNUSED(locker)
    m_nextQuery.isReady = false;
    m_nextQuery.placeHolderBinds.append(std::make_tuple(placeholder, val, paramType));
}

void MSqlQueryWorker::addBindValue(const QVariant &val, QSql::ParamType paramType) {
    QMutexLocker locker(&mutex);
    Q_UNUSED(locker)
    m_nextQuery.isReady = false;
    m_nextQuery.positionalBinds.append(std::make_tuple(val, paramType));
}

void MSqlQueryWorker::execAsync() {
    QMutexLocker locker(&mutex);
    Q_UNUSED(locker)
    m_nextQuery.isReady = true;
    InvokeLater(this, &MSqlQueryWorker::execNextQuery);
}

bool MSqlQueryWorker::next() {
    QMutexLocker locker(&mutex);
    if(m_nextQuery.isReady)
        return false; //another query is to be executed soon
    if(m_records.isEmpty()) return false;
    if(++m_currentItem >= m_records.size()) return false;
    return true;
}

bool MSqlQueryWorker::seek(int index) {
    QMutexLocker locker(&mutex);
    if(m_nextQuery.isReady)
        return false; //another query is to be executed soon
    if(index >= m_records.size()) {
        return false;
    } else {
        m_currentItem = index;
        return true;
    }
}

QSqlRecord MSqlQueryWorker::record() const {
    QMutexLocker locker(&mutex);
    QSqlRecord record = m_records.at(m_currentItem);
    return record;
}

QVariant MSqlQueryWorker::lastInsertId() const {
    QMutexLocker locker(&mutex);
    QVariant lastId= m_lastInsertId;
    return lastId;
}

QSqlError MSqlQueryWorker::lastError() const {
    QMutexLocker locker(&mutex);
    QSqlError lastE = m_lastError;
    return lastE;
}

void MSqlQueryWorker::execNextQuery() {
    QMutexLocker locker(&mutex);
    //take out next query
    SqlQueryExec currentQuery = m_nextQuery;
    m_nextQuery.isReady = false;
    locker.unlock(); //unlock mutex
    if(!currentQuery.isReady) return;
    q->clear();
    q->prepare(currentQuery.prepareStr);
    for(const auto& bind : currentQuery.placeHolderBinds)
        q->bindValue(std::get<0>(bind), std::get<1>(bind), std::get<2>(bind));
    for(const auto& bind : currentQuery.positionalBinds)
        q->addBindValue(std::get<0>(bind), std::get<1>(bind));
    if(q->exec()) { //execute statement
        locker.relock(); //lock mutex to store new records
        while(q->next()) m_records.append(q->record());
        m_currentItem = 0;
        m_lastInsertId = q->lastInsertId();
        m_lastError = QSqlError();
        locker.unlock();
        emit resultsReady(true);
    } else {
        locker.relock(); //lock mutex to store new error
        m_records.clear();
        m_currentItem = 0;
        m_lastInsertId = QVariant();
        m_lastError= q->lastError();
        locker.unlock();
        emit resultsReady(false);
    }
}

void MSqlQueryWorker::setNextQueryReady(bool isReady) {
    QMutexLocker locker(&mutex);
    m_nextQuery.isReady = isReady;
}
