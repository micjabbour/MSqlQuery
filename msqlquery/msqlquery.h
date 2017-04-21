#ifndef MSQLQUERY_H
#define MSQLQUERY_H

#include <QObject>
#include <QSqlRecord>
#include <QVariant>
#include "msqldatabase.h"
#include <QMutex>

class QSqlQuery;
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
    bool next();
    bool seek(int index);
    QSqlRecord record() const;
    QSqlError lastError() const;
    void execAsync(const QString& query);
    void execAsync();
    QString getDbConnectionName()const{return db.connectionName();}
    QVariant lastInsertId()const;

    //additional functions
    bool isBusy()const;
    QList<QSqlRecord> getAllRecords() const;
signals:
    void resultsReady(bool success);
    void busyToggled(bool isBusy);
private:
    Q_INVOKABLE void workerFinished(bool success);

    //pointer accessed only from the client thread
    //passed to worker threads through lambdas capturing it by value, lives in database connection thread
    MSqlQueryWorker* w;
    MSqlDatabase db;
    bool m_isBusy = false; //accessed only from client thread
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
    void execAsync();
    bool next();
    bool seek(int index);
    QSqlRecord record() const;
    QVariant lastInsertId() const;
    QSqlError lastError() const;
    void setNextQueryReady(bool isReady);
    bool isBusy() const;
    bool hasNextQuery() const;
    QList<QSqlRecord> getAllRecords() const;

    Q_SIGNAL void resultsReady(bool success);
    Q_INVOKABLE void execNextQuery(); //always invoked in worker thread
private:
    mutable QMutex mutex;
    struct SqlQueryExec {
        QString prepareStr;
        QList<PlaceHolderBind> placeHolderBinds;
        QList<PositionalBind> positionalBinds;
        bool isReady = false;
    } m_nextQuery;
    QList<QSqlRecord> m_records; //to store query result
    int m_currentItem = -1; //before first item
    bool m_isBusy = false;
    QSqlError m_lastError;
    QVariant m_lastInsertId; //to store query last insert id
};

#endif // MSQLQUERY_H
