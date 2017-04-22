#ifndef MSQLQUERY_H
#define MSQLQUERY_H

#include <QObject>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QVariant>
#include "msqldatabase.h"
#include <QMutex>

class MSqlQueryWorker;

//all functions in this class do NOT block EXCEPT the exec() function
//use execAsync() and connect to resultsReady() signal instead
class MSqlQuery : public QObject {
    Q_OBJECT
public:
    explicit MSqlQuery(QObject *parent = 0, MSqlDatabase db = MSqlDatabase::database());
    ~MSqlQuery();
    //an interface similar to QSqlQuery
    void prepare(const QString& query);
    void addBindValue(const QVariant& val, QSql::ParamType paramType = QSql::In);
    void bindValue(const QString& placeholder, const QVariant& val, QSql::ParamType paramType = QSql::In);
    bool exec(const QString& query);
    bool exec();
    bool execBatch(QSqlQuery::BatchExecutionMode mode = QSqlQuery::ValuesAsRows);
    bool next();
    bool seek(int index);
    QSqlRecord record() const;
    QSqlError lastError() const;
    void execAsync(const QString& query);
    void execAsync();
    void execBatchAsync(QSqlQuery::BatchExecutionMode mode = QSqlQuery::ValuesAsRows);
    QString getDbConnectionName()const{return db.connectionName();}
    QVariant lastInsertId()const;

    //additional functions
    bool isBusy()const;
    QList<QSqlRecord> getAllRecords() const;
signals:
    void resultsReady(bool success);
    void busyToggled(bool isBusy);
private:
    Q_INVOKABLE void workerFinished(int queryId, bool success);
    
    bool execNextBlocking();
    //pointer accessed only from the client thread
    //passed to worker threads through lambdas capturing it by value, lives in database connection thread
    MSqlQueryWorker* w;
    MSqlDatabase db;
    bool m_isBusy = false; //accessed only from client thread
    //when a query is finished, its id is checked to make sure that it matches currentQueryId
    //(in order to emit resultsReady signal only for the last query set on this object)
    int currentQueryId = -1;
};

//the worker object lives in the database connection's thread and owns the QSqlQuery instance
//this class is internal to the library
//all functions in the worker object are accessed from the worker thread only, except
//functions marked as thread-safe
class MSqlQueryWorker : public QObject {
    Q_OBJECT
public:
    using PlaceHolderBind = std::tuple<QString, QVariant, QSql::ParamType>;
    using PositionalBind = std::tuple<QVariant, QSql::ParamType>;

    explicit MSqlQueryWorker(); //worker does not have a parent
    ~MSqlQueryWorker();
    QSqlQuery* q; //accessed only from worker threads
    //the following functions are thread-safe
    void prepare(const QString &query);
    void bindValue(const QString &placeholder, const QVariant &val, QSql::ParamType paramType);
    void addBindValue(const QVariant& val, QSql::ParamType paramType = QSql::In);
    void execAsync(int queryId, bool isBatch = false, QSqlQuery::BatchExecutionMode batchMode = QSqlQuery::ValuesAsRows);
    bool next();
    bool seek(int index);
    QSqlRecord record() const;
    QVariant lastInsertId() const;
    QSqlError lastError() const;
    void setNextQueryReady(bool isReady, bool isBatch = false, QSqlQuery::BatchExecutionMode batchMode = QSqlQuery::ValuesAsRows);
    bool isBusy() const;
    bool hasNextQuery() const;
    QList<QSqlRecord> getAllRecords() const;

    Q_SIGNAL void resultsReady(int queryId, bool success);
    Q_INVOKABLE void execNextQuery(); //always invoked in worker thread
private:
    mutable QMutex mutex;
    struct SqlQueryExec {
        //every query to be executed has an id, so that it can be overwritten later
        //when a query is finished, its id is checked to make sure that it has not been overwritten
        //(in order to emit resultsReady signal only for the last query set on this object)
        int queryId = -1; 
        QString prepareStr;
        QList<PlaceHolderBind> placeHolderBinds;
        QList<PositionalBind> positionalBinds;
        bool isBatch = false;
        QSqlQuery::BatchExecutionMode batchMode;
        bool isReady = false;
    } m_nextQuery;
    QList<QSqlRecord> m_records; //to store query result
    int m_currentItem = -1; //before first item
    bool m_isBusy = false;
    QSqlError m_lastError;
    QVariant m_lastInsertId; //to store query last insert id
};

#endif // MSQLQUERY_H
