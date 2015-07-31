#ifndef MDBWORKER_H
#define MDBWORKER_H

#include <QObject>
#include <QThread>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>
#include <QSqlRecord>

class MSqlQuery;
class QSqlDriver;

class MDbThread : public QThread
{
public:
    explicit MDbThread(QObject* parent = 0);
    ~MDbThread();
    
    static MDbThread* Instance();
private:
};

class MDbWorker : public QObject
{
    Q_OBJECT
public:
    explicit MDbWorker(QObject *parent = 0);
    ~MDbWorker();
    
    static MDbWorker* Instance();
    
    static const QString defaultConnectionName;
signals:
    void results(QList<QSqlRecord> records, bool success);
public slots:
    void CallQSqlDatabaseAddDatabase(const QString& type, const QString& connectionName = defaultConnectionName);
    void CallQSqlDatabaseSetHostName(const QString& hostName, const QString& connectionName=defaultConnectionName);
    void CallQSqlDatabaseSetDatabaseName(const QString& name, const QString& connectionName=defaultConnectionName);
    void CallQSqlDatabaseSetUserName(const QString& name, const QString& connectionName=defaultConnectionName);
    void CallQSqlDatabaseSetPassword(const QString& password, const QString& connectionName=defaultConnectionName);
    void CallQSqlDatabaseSetConnectionOptions(const QString& options = QString(), const QString& connectionName=defaultConnectionName);
    void CallQSqlDatabaseSetPort(int port, const QString& connectionName=defaultConnectionName);
    QString CallQSqlDatabaseHostName(const QString& connectionName=defaultConnectionName);
    QString CallQSqlDatabaseDatabaseName(const QString& connectionName=defaultConnectionName);
    QString CallQSqlDatabaseUserName(const QString& connectionName=defaultConnectionName);
    QString CallQSqlDatabasePassword(const QString& connectionName=defaultConnectionName);
    QString CallQSqlDatabaseConnectionOptions(const QString& connectionName=defaultConnectionName);
    int CallQSqlDatabasePort(const QString& connectionName=defaultConnectionName);
    bool CallQSqlDatabaseTransaction(const QString& connectionName=defaultConnectionName);
    bool CallQSqlDatabaseCommit(const QString& connectionName=defaultConnectionName);
    bool CallQSqlDatabaseRollback(const QString& connectionName=defaultConnectionName);
    bool CallQSqlDatabaseOpen(const QString& connectionName=defaultConnectionName);
    void CallQSqlDatabaseClose(const QString& connectionName=defaultConnectionName);
    bool CallQSqlDatabaseIsOpen(const QString& connectionName=defaultConnectionName);
    bool CallQSqlDatabaseIsOpenError(const QString& connectionName=defaultConnectionName);
    bool CallQSqlDatabaseIsValid(const QString& connectionName=defaultConnectionName);
    QSqlError CallQSqlDatabaseLastError(const QString& connectionName=defaultConnectionName);
    QSqlQuery* CreateNewQSqlQuery(const QString& connectionName=defaultConnectionName); //called from MSqlQuery constructor, deleted in its destructor
    void DeleteQSqlQuery(QSqlQuery* query);
    bool CallQSqlQueryPrepare(QSqlQuery* query, const QString& querystr);
    void CallQSqlQueryAddBindValue(QSqlQuery* query, const QVariant& val, QSql::ParamType paramType = QSql::In);
    void CallQSqlQueryBindValue(QSqlQuery* query, const QString& placeholder, const QVariant& val, QSql::ParamType paramType = QSql::In);
    void CallQSqlQueryExec(MSqlQuery* mquery, QSqlQuery* query, const QString& querystr);
    void CallQSqlQueryExec(MSqlQuery* mquery, QSqlQuery* query);
    QVariant CallQSqlQueryLastInsertId(QSqlQuery* query);
    QSqlDriver* CallQSqlDatabaseDriver(const QString& connectionName);
    bool CallQSqlDriverSubscribeToNotification(const QString& notificationName, const QString& connectionName=defaultConnectionName);
    QStringList CallQSqlDriverSubscribedToNotifications(const QString& connectionName=defaultConnectionName);
    bool CallQSqlDriverUnsubscribeFromNotification(const QString& notificationName, const QString& connectionName=defaultConnectionName);
    //bool CallQSqlQueryExecSync(MSqlQuery* mquery, QSqlQuery* query, const QString& querystr);
    //bool CallQSqlQueryExecSync(MSqlQuery* mquery, QSqlQuery* query);
    
private:
};

#endif // MDBWORKER_H
