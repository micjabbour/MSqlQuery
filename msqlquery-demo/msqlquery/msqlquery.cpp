#include "msqlquery.h"
#include "qthreadutils.h"
#include "msqlthread.h"
#include "msqldatabase.h"
#include <QMutexLocker>
#include <QSqlQuery>

MSqlQuery::MSqlQuery(QObject *parent, MSqlDatabase db)
    : QObject(parent), db(db) {
    w= new MSqlQueryWorker();
    //connect signal from worker to the MSqlQuery's private slot
    connect(w, &MSqlQueryWorker::resultsReady, this, &MSqlQuery::workerFinished);
    w->moveToThread(MSqlDatabase::threadForConnection(db.connectionName()));
    auto w= this->w; //in order to capture w by value
    //   ^^^^^^^^^^ is there a better way to do this without using c++14 initializing capture??
    PostToWorker(w, [=]{
        QSqlDatabase qdb = QSqlDatabase::database(db.connectionName());
        w->q = new QSqlQuery(qdb);
    });
}

MSqlQuery::~MSqlQuery() {
    w->setNextQueryReady(false); //cancel next query if any
    InvokeLater(w, &QObject::deleteLater);
}

void MSqlQuery::prepare(const QString &query) {
    w->prepare(query);
}

void MSqlQuery::addBindValue(const QVariant &val, QSql::ParamType paramType) {
    w->addBindValue(val, paramType);
}

void MSqlQuery::bindValue(const QString &placeholder, const QVariant &val, QSql::ParamType paramType) {
    w->bindValue(placeholder, val, paramType);
}

void MSqlQuery::execAsync(const QString &query) {
    w->prepare(query);
    execAsync();
}

void MSqlQuery::execAsync()
{
    w->execAsync(++currentQueryId);
    if(!m_isBusy) {
        m_isBusy = true;
        emit busyToggled(true);
    }
}

void MSqlQuery::execBatchAsync(QSqlQuery::BatchExecutionMode mode) {
    w->execAsync(++currentQueryId, true, mode);
    if(!m_isBusy) {
        m_isBusy = true;
        emit busyToggled(true);
    }
}

bool MSqlQuery::exec(const QString &query) {
    w->prepare(query);
    return exec();
}

bool MSqlQuery::exec() {
    w->setNextQueryReady(true);
    return execNextBlocking();
}

bool MSqlQuery::execBatch(QSqlQuery::BatchExecutionMode mode) {
    w->setNextQueryReady(true, true, mode);
    return execNextBlocking();
}

bool MSqlQuery::next() {
    return w->next();
}

QSqlRecord MSqlQuery::record() const {
    return w->record();
}

QVariant MSqlQuery::value(int index) const {
    return w->record().value(index);
}

QVariant MSqlQuery::value(const QString &name) const {
    return w->record().value(name);
}

QSqlError MSqlQuery::lastError() const {
    return w->lastError();
}

bool MSqlQuery::seek(int index)
{
    return w->seek(index);
}

QVariant MSqlQuery::lastInsertId() const {
    return w->lastInsertId();
}

bool MSqlQuery::isBusy() const {
    return m_isBusy;
}

QList<QSqlRecord> MSqlQuery::getAllRecords() const {
    return w->getAllRecords();
}

void MSqlQuery::workerFinished(int queryId, bool success) {
    if(queryId == currentQueryId) { //if this signal does not belong to an overwritten query
        m_isBusy = false;
        emit resultsReady(success);
        emit busyToggled(false);
    }
}

bool MSqlQuery::execNextBlocking() {
    currentQueryId++; //previous queries are not interesting anymore
    //block signals when using sync API ( blockSignals(true) is not thread-safe )
    disconnect(w, &MSqlQueryWorker::resultsReady, this, &MSqlQuery::workerFinished);
    auto w= this->w; //in order to capture w by value
    CallByWorker(w, [=]{
        w->execNextQuery();
    });
    connect(w, &MSqlQueryWorker::resultsReady, this, &MSqlQuery::workerFinished);
    bool success = w->lastError().type()==QSqlError::NoError;
    return success;
}

MSqlQueryWorker::MSqlQueryWorker():QObject(nullptr) {
    
}

MSqlQueryWorker::~MSqlQueryWorker() {
    delete q;
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

void MSqlQueryWorker::execAsync(int queryId, bool isBatch, QSqlQuery::BatchExecutionMode batchMode) {
    QMutexLocker locker(&mutex);
    Q_UNUSED(locker)
    m_nextQuery.isReady = true;
    m_nextQuery.isBatch = isBatch;
    m_nextQuery.batchMode = batchMode;
    m_nextQuery.queryId = queryId;
    m_records.clear();
    m_currentItem = -1; //before first item
    m_lastInsertId = QVariant();
    m_lastError = QSqlError();
    InvokeLater(this, &MSqlQueryWorker::execNextQuery);
}

bool MSqlQueryWorker::next() {
    QMutexLocker locker(&mutex);
    if(m_records.isEmpty()) return false;
    if(m_currentItem+1 >= m_records.size()) return false;
    m_currentItem++;
    return true;
}

bool MSqlQueryWorker::seek(int index) {
    QMutexLocker locker(&mutex);
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
    return m_lastInsertId;
}

QSqlError MSqlQueryWorker::lastError() const {
    QMutexLocker locker(&mutex);
    return m_lastError;
}

void MSqlQueryWorker::execNextQuery() {
    QMutexLocker locker(&mutex);
    if(!m_nextQuery.isReady) return; //if there is no query to execute
    //take out next query
    SqlQueryExec currentQuery = m_nextQuery;
    m_nextQuery.isReady = false;
    m_isBusy = true;
    locker.unlock(); //unlock mutex
    q->clear();
    q->prepare(currentQuery.prepareStr);
    for(const auto& bind : currentQuery.placeHolderBinds)
        q->bindValue(std::get<0>(bind), std::get<1>(bind), std::get<2>(bind));
    for(const auto& bind : currentQuery.positionalBinds)
        q->addBindValue(std::get<0>(bind), std::get<1>(bind));
    bool result; //query execution result
    if(currentQuery.isBatch)
        //do exec batch if it is a batch query
        result = q->execBatch(currentQuery.batchMode);
    else
        //otherwise call normal exec
        result = q->exec();
    locker.relock(); //lock mutex to store new records
    //clear any previous results (if any)
    m_records.clear();
    m_currentItem = -1; //before first item
    m_lastInsertId = QVariant();
    m_lastError = QSqlError();
    if(m_nextQuery.isReady) //if another query has been scheduled
        return; //cancel current query (no need to store its results)
    if(result) { //execute statement
        while(q->next()) m_records.append(q->record());
        m_currentItem = -1; //before first item
        m_lastInsertId = q->lastInsertId();
        m_lastError = QSqlError();
        m_isBusy = false;
    } else {
        locker.relock(); //lock mutex to store new error
        m_records.clear();
        m_currentItem = -1; //before first item
        m_lastInsertId = QVariant();
        m_lastError= q->lastError();
        m_isBusy = false;
    }
    locker.unlock();
    q->clear();
    emit resultsReady(currentQuery.queryId, result);
}

void MSqlQueryWorker::setNextQueryReady(bool isReady, bool isBatch, QSqlQuery::BatchExecutionMode batchMode) {
    QMutexLocker locker(&mutex);
    Q_UNUSED(locker)
    m_nextQuery.isReady = isReady;
    m_nextQuery.isBatch = isBatch;
    m_nextQuery.batchMode = batchMode;
}

bool MSqlQueryWorker::hasNextQuery() const {
    QMutexLocker locker(&mutex);
    Q_UNUSED(locker)
    return m_nextQuery.isReady;
}

QList<QSqlRecord> MSqlQueryWorker::getAllRecords() const {
    QMutexLocker locker(&mutex);
    Q_UNUSED(locker)
    return m_records;
}

bool MSqlQueryWorker::isBusy() const {
    QMutexLocker locker(&mutex);
    Q_UNUSED(locker)
    return m_isBusy;
}
