#include "msqldatabase.h"
#include "mdbworker.h"
#include <QSqlDatabase>
#include <QStringList>


const QString MSqlDatabase::defaultConnectionName(QSqlDatabase::defaultConnection);

MSqlDatabase::MSqlDatabase()
{
    
}

MSqlDatabase::~MSqlDatabase()
{
    
}

MSqlDatabase MSqlDatabase::addDatabase(const QString &type, const QString &connectionName)
{
    MSqlDatabase db;
    db.m_connectionName = connectionName;
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDatabaseAddDatabase",
                              Qt::QueuedConnection, 
                              Q_ARG(QString, type),
                              Q_ARG(QString, connectionName));
    return db;
}

MSqlDatabase MSqlDatabase::database(const QString &connectionName)
{
    MSqlDatabase db;
    db.m_connectionName = connectionName;
    return db;
}

void MSqlDatabase::setHostName(const QString &host)
{
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDatabaseSetHostName",
                              Qt::QueuedConnection,
                              Q_ARG(QString, host),
                              Q_ARG(QString, m_connectionName));
}

void MSqlDatabase::setDatabaseName(const QString &name)
{
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDatabaseSetDatabaseName",
                              Qt::QueuedConnection,
                              Q_ARG(QString, name),
                              Q_ARG(QString, m_connectionName));
}

void MSqlDatabase::setUserName(const QString &name)
{
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDatabaseSetUserName",
                              Qt::QueuedConnection, 
                              Q_ARG(QString, name),
                              Q_ARG(QString, m_connectionName));
}

void MSqlDatabase::setPassword(const QString& password)
{
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDatabaseSetPassword",
                              Qt::QueuedConnection, 
                              Q_ARG(QString, password),
                              Q_ARG(QString, m_connectionName));
}

void MSqlDatabase::setConnectionOptions(const QString &options)
{
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDatabaseSetConnectionOptions",
                              Qt::QueuedConnection,
                              Q_ARG(QString, options),
                              Q_ARG(QString, m_connectionName));
}

void MSqlDatabase::setPort(int port)
{
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDatabaseSetPort",
                              Qt::QueuedConnection,
                              Q_ARG(int, port),
                              Q_ARG(QString, m_connectionName));
}

QString MSqlDatabase::hostName()
{
    QString hostName;
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDatabaseHostName",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(QString, hostName),
                              Q_ARG(QString, m_connectionName));
    return hostName;
}

QString MSqlDatabase::databaseName()
{
    QString dbName;
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDatabaseDatabaseName",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(QString, dbName),
                              Q_ARG(QString, m_connectionName));
    return dbName;
}

QString MSqlDatabase::userName()
{
    QString name;
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDatabaseUserName",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(QString, name),
                              Q_ARG(QString, m_connectionName));
    return name; 
}

QString MSqlDatabase::password()
{
    QString pass;
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDatabasePassword",
                              Qt::BlockingQueuedConnection, 
                              Q_RETURN_ARG(QString, pass),
                              Q_ARG(QString, m_connectionName));
    return pass;
}

QString MSqlDatabase::connectionOptions()
{
    QString options;
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDatabaseConnectionOptions",
                              Qt::BlockingQueuedConnection, 
                              Q_RETURN_ARG(QString, options),
                              Q_ARG(QString, m_connectionName));
    return options;
}

int MSqlDatabase::port()
{
    int port;
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDatabasePort",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(int, port),
                              Q_ARG(QString, m_connectionName));
    return port;
}

bool MSqlDatabase::transaction()
{
    bool result;
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDatabaseTransaction",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(bool, result),
                              Q_ARG(QString, m_connectionName));
    return result;
}

bool  MSqlDatabase::commit()
{
    bool result;
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDatabaseCommit",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(bool, result),
                              Q_ARG(QString, m_connectionName));
    return result;
}

bool MSqlDatabase::rollback()
{
    bool result;
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDatabaseRollback",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(bool, result),
                              Q_ARG(QString, m_connectionName));
    return result;
}

QSqlError MSqlDatabase::lastError()const
{
    QSqlError result;
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDatabaseLastError",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(QSqlError, result),
                              Q_ARG(QString, m_connectionName));
    return result;
}

bool MSqlDatabase::open()
{
    bool result;
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDatabaseOpen",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(bool, result),
                              Q_ARG(QString, m_connectionName));
    return result;
}

void MSqlDatabase::close()
{
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDatabaseClose",
                              Qt::QueuedConnection,
                              Q_ARG(QString, m_connectionName));
}

bool MSqlDatabase::isOpen()const
{
    bool result;
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDatabaseIsOpen",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(bool, result),
                              Q_ARG(QString, m_connectionName));
    return result;
}

bool MSqlDatabase::isOpenError()const
{
    bool result;
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDatabaseIsOpenError",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(bool, result),
                              Q_ARG(QString, m_connectionName));
    return result;
}

bool MSqlDatabase::isValid()const
{
    bool result;
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDatabaseIsValid",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(bool, result),
                              Q_ARG(QString, m_connectionName));
    return result;
}

bool MSqlDatabase::subscribeToNotification(const QString &name)
{
    bool result;
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDriverSubscribeToNotification",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(bool, result),
                              Q_ARG(QString, name),
                              Q_ARG(QString, m_connectionName));
    return result;
}

QStringList MSqlDatabase::subscribedToNotifications()const
{
    QStringList result;
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDriverSubscribedToNotifications",
                              Qt::BlockingQueuedConnection, 
                              Q_RETURN_ARG(QStringList, result),
                              Q_ARG(QString, m_connectionName));
    return result;
}

bool MSqlDatabase::unsubscribeFromNotification(const QString &name)
{
    bool result;
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDriverUnsubscribeFromNotification",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(bool, result),
                              Q_ARG(QString, name),
                              Q_ARG(QString, m_connectionName));
    return result;
}

QSqlDriver* MSqlDatabase::driver()
{
    QSqlDriver* result;
    QMetaObject::invokeMethod(MDbWorker::Instance(), "CallQSqlDatabaseDriver",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(QSqlDriver*, result),
                              Q_ARG(QString, m_connectionName));
    return result;
}
