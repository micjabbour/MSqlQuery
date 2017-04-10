#ifndef MSQLQUERY_H
#define MSQLQUERY_H

#include <QObject>
#include <QSqlRecord>
#include <QVariant>
#include "msqldatabase.h"

class QSqlQuery;

class MSqlQuery : public QObject {
    Q_OBJECT
public:
    explicit MSqlQuery(QObject *parent = 0, MSqlDatabase db = MSqlDatabase::database());
    ~MSqlQuery();
    
    bool prepare(const QString& query);
    void addBindValue(const QVariant& val, QSql::ParamType paramType = QSql::In);
    void bindValue(const QString& placeholder, const QVariant& val, QSql::ParamType paramType = QSql::In);
    
    bool exec(const QString& query);
    bool exec();
    
    bool next();
    bool seek(int index);
    QSqlRecord record();
    bool isSuccess()const{return lastSuccess;}
    bool isBusy()const{return m_isBusy;}
    bool execAsync(const QString& query); //the async version (returns immediately, sets the object to busy, returns false and fails when busy)
    bool execAsync();
    QString getDbConnectionName()const{return db.connectionName();}
    QVariant lastInsertId()const;
signals:
    void gotResults(bool success);
    void gotResponseFromOtherThread(); //for internal class usage
public slots:
    void setResults(const QList<QSqlRecord>& res, bool success); //set isBusy to false, and emits gotResults signal
private:
    //the worker object that lives in the database connection's thread and owns the QSqlQuery instance
    class MSqlQueryWorker : public QObject {
    public:
        explicit MSqlQueryWorker(QObject* parent= nullptr);
        ~MSqlQueryWorker() { delete q; }
        QSqlQuery* q; //accessed only from worker threads
    };

    //pointer accessed only from the client thread
    //passed to worker threads through lambdas capturing it by value, lives in database connection thread
    MSqlQueryWorker* w;
    MSqlDatabase db;
    QSqlQuery* qquery;
    bool m_isBusy;
    QList<QSqlRecord> records;
    int currentItem;
    bool lastSuccess;
    bool shouldEmitGotResult; //false when the call is synchronuous so that it does not emit the signal
};

#endif // MSQLQUERY_H
