#ifndef MSQLQUERY_H
#define MSQLQUERY_H

#include <QObject>
#include <QSqlRecord>
#include <QVariant>
#include "msqldatabase.h"

class QSqlQuery;

class MSqlQuery : public QObject
{
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
    bool asyncExec(const QString& query); //the async version (returns immediately, sets the object to busy, returns false and fails when busy)
    bool asyncExec();
    QString getDbConnectionName()const{return db.connectionName();}
    QVariant lastInsertId()const;
signals:
    void gotResults(bool success);
    void gotResponseFromOtherThread(); //for internal class usage
public slots:
    void setResults(const QList<QSqlRecord>& res, bool success); //set isBusy to false, and emits gotResults signal
private:
    MSqlDatabase db;
    QSqlQuery* qquery;
    bool m_isBusy;
    QList<QSqlRecord> records;
    int currentItem;
    bool lastSuccess;
    bool shouldEmitGotResult; //false when the call is synchronuous so that it does not emit the signal
};

#endif // MSQLQUERY_H
