#include "mdbworker.h"
#include "msqlquery.h"
#include <QGlobalStatic>
#include <QSqlDatabase>
#include <QApplication>
#include <QSqlDriver>

Q_GLOBAL_STATIC(MDbThread, mDbThread)
Q_GLOBAL_STATIC(MDbWorker,  mDbWorker)

const QString MDbWorker::defaultConnectionName(QSqlDatabase::defaultConnection);

MDbThread::MDbThread(QObject *parent) : QThread(parent)
{
    start();
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(quit()));
}
MDbThread::~MDbThread()
{
    
}
MDbThread* MDbThread::Instance()
{
    return mDbThread();
}
MDbWorker::MDbWorker(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<MSqlQuery*>("MSqlQuery*");
    qRegisterMetaType<QSqlQuery*>("QSqlQuery*");
    qRegisterMetaType< QList<QSqlRecord> >("QList<QSqlRecord>");
    qRegisterMetaType<QSqlDriver*>("QSqlDriver*");
    qRegisterMetaType<QSql::ParamType>("QSql::ParamType");
    moveToThread(MDbThread::Instance());
}
MDbWorker::~MDbWorker()
{
    
}


MDbWorker* MDbWorker::Instance()
{
    return mDbWorker();
}

void MDbWorker::CallQSqlDatabaseAddDatabase(const QString &type, const QString &connectionName)
{
    QSqlDatabase::addDatabase(type, connectionName);
}
void MDbWorker::CallQSqlDatabaseSetHostName(const QString &hostName, const QString &connectionName)
{
    QSqlDatabase::database(connectionName, false).setHostName(hostName);
}
void MDbWorker::CallQSqlDatabaseSetDatabaseName(const QString &name, const QString &connectionName)
{
    QSqlDatabase::database(connectionName, false).setDatabaseName(name);
}
void MDbWorker::CallQSqlDatabaseSetUserName(const QString &name, const QString &connectionName)
{
    QSqlDatabase::database(connectionName, false).setUserName(name);
}
void MDbWorker::CallQSqlDatabaseSetPassword(const QString &password, const QString &connectionName)
{
    QSqlDatabase::database(connectionName, false).setPassword(password);
}
void MDbWorker::CallQSqlDatabaseSetPort(int port, const QString &connectionName)
{
    QSqlDatabase::database(connectionName, false).setPort(port);
}
void MDbWorker::CallQSqlDatabaseSetConnectionOptions(const QString &options, const QString &connectionName)
{
    QSqlDatabase::database(connectionName, false).setConnectOptions(options);
}
QString MDbWorker::CallQSqlDatabaseHostName(const QString &connectionName)
{
    return QSqlDatabase::database(connectionName, false).hostName();
}
QString MDbWorker::CallQSqlDatabaseDatabaseName(const QString &connectionName)
{
    return QSqlDatabase::database(connectionName, false).databaseName();
}
QString MDbWorker::CallQSqlDatabaseUserName(const QString &connectionName)
{
    return QSqlDatabase::database(connectionName, false).userName();
}
QString MDbWorker::CallQSqlDatabasePassword(const QString &connectionName)
{
    return QSqlDatabase::database(connectionName, false).password();
}
QString MDbWorker::CallQSqlDatabaseConnectionOptions(const QString &connectionName)
{
    return QSqlDatabase::database(connectionName, false).connectOptions();
}
int MDbWorker::CallQSqlDatabasePort(const QString &connectionName)
{
    return QSqlDatabase::database(connectionName, false).port();
}
bool MDbWorker::CallQSqlDatabaseTransaction(const QString &connectionName)
{
    return QSqlDatabase::database(connectionName, false).transaction();
}
bool MDbWorker::CallQSqlDatabaseCommit(const QString &connectionName)
{
    return QSqlDatabase::database(connectionName, false).commit();
}
bool MDbWorker::CallQSqlDatabaseRollback(const QString &connectionName)
{
    return QSqlDatabase::database(connectionName, false).rollback();
}
QSqlError MDbWorker::CallQSqlDatabaseLastError(const QString &connectionName)
{
    return QSqlDatabase::database(connectionName, false).lastError();
}
bool MDbWorker::CallQSqlDatabaseOpen(const QString &connectionName)
{
    return QSqlDatabase::database(connectionName, false).open();
}
void MDbWorker::CallQSqlDatabaseClose(const QString &connectionName)
{
    QSqlDatabase::database(connectionName, false).close();
}
bool MDbWorker::CallQSqlDatabaseIsOpen(const QString &connectionName)
{
    return QSqlDatabase::database(connectionName, false).isOpen();
}
bool MDbWorker::CallQSqlDatabaseIsOpenError(const QString &connectionName)
{
    return QSqlDatabase::database(connectionName, false).isOpenError();
}
bool MDbWorker::CallQSqlDatabaseIsValid(const QString &connectionName)
{
    return QSqlDatabase::database(connectionName, false).isValid();
}

QSqlQuery* MDbWorker::CreateNewQSqlQuery(const QString &connectionName)
{
    return new QSqlQuery(QSqlDatabase::database(connectionName, false));
}
void MDbWorker::DeleteQSqlQuery(QSqlQuery *query)
{
    delete query;
}

bool MDbWorker::CallQSqlQueryPrepare(QSqlQuery *query, const QString &querystr)
{
    return query->prepare(querystr);
}
void MDbWorker::CallQSqlQueryAddBindValue(QSqlQuery *query, const QVariant &val, QSql::ParamType paramType)
{
    query->addBindValue(val, paramType);
}
void MDbWorker::CallQSqlQueryBindValue(QSqlQuery *query, const QString &placeholder, const QVariant &val, QSql::ParamType paramType)
{
    query->bindValue(placeholder, val, paramType);
}
void MDbWorker::CallQSqlQueryExec(MSqlQuery* mquery, QSqlQuery *query, const QString &querystr)
{
    connect(this, SIGNAL(results(QList<QSqlRecord>,bool)), mquery, SLOT(setResults(QList<QSqlRecord>,bool)));
    QList<QSqlRecord> res;
    if(!query->exec(querystr))
    {
        /*QMetaObject::invokeMethod(mquery, "setResults",
                                  Qt::QueuedConnection,
                                  Q_ARG(QList<QSqlRecord>, res),
                                  Q_ARG(bool, false));*/
        emit results(res, false);
        disconnect(this, SIGNAL(results(QList<QSqlRecord>,bool)), 0, 0);
    }
    else
    {
        while(query->next())
            res.append(query->record());
        /*QMetaObject::invokeMethod(mquery, "setResults",
                                  Qt::QueuedConnection,
                                  Q_ARG(QList<QSqlRecord>, res),
                                  Q_ARG(bool, true));*/
        emit results(res, true);
        disconnect(this, SIGNAL(results(QList<QSqlRecord>,bool)), 0, 0);
    }
}
void MDbWorker::CallQSqlQueryExec(MSqlQuery *mquery, QSqlQuery *query)
{
    connect(this, SIGNAL(results(QList<QSqlRecord>,bool)), mquery, SLOT(setResults(QList<QSqlRecord>,bool)));
    QList<QSqlRecord> res;
    if(!query->exec())
    {
        /*QMetaObject::invokeMethod(mquery, "setResults",
                                  Qt::QueuedConnection,
                                  Q_ARG(QList<QSqlRecord>, res),
                                  Q_ARG(bool, false));*/
        emit results(res, false);
        disconnect(this, SIGNAL(results(QList<QSqlRecord>,bool)), 0, 0);
    }
    else
    {
        while(query->next())
            res.append(query->record());
        /*QMetaObject::invokeMethod(mquery, "setResults",
                                  Qt::QueuedConnection,
                                  Q_ARG(QList<QSqlRecord>, res),
                                  Q_ARG(bool, true));*/
        emit results(res, true);
        disconnect(this, SIGNAL(results(QList<QSqlRecord>,bool)), 0, 0);
    }
}
QVariant MDbWorker::CallQSqlQueryLastInsertId(QSqlQuery *query)
{
    return query->lastInsertId();
}

QSqlDriver* MDbWorker::CallQSqlDatabaseDriver(const QString &connectionName)
{
    return QSqlDatabase::database(connectionName, false).driver();
}

bool MDbWorker::CallQSqlDriverSubscribeToNotification(const QString& notificationName, const QString &connectionName)
{
    return QSqlDatabase::database(connectionName, false).driver()->subscribeToNotification(notificationName);
}

QStringList MDbWorker::CallQSqlDriverSubscribedToNotifications(const QString &connectionName)
{
    return QSqlDatabase::database(connectionName, false).driver()->subscribedToNotifications();
}

bool MDbWorker::CallQSqlDriverUnsubscribeFromNotification(const QString &notificationName, const QString &connectionName)
{
    return QSqlDatabase::database(connectionName, false).driver()->unsubscribeFromNotification(notificationName);
}

