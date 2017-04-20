#include "msqldatabase.h"
#include "mdbworker.h"
#include <QSqlDatabase>
#include <QStringList>
#include "qthreadutils.h"
#include "msqlthread.h"
#include <QSqlDriver>


const QString MSqlDatabase::defaultConnectionName(QSqlDatabase::defaultConnection+"msqlquery_default");

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
    PostToWorker(workerForConnection(connectionName), [=]{
        QSqlDatabase::addDatabase(type, connectionName);
    });
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
    QString connectionName = m_connectionName;
    PostToWorker(workerForConnection(connectionName), [=]{
        QSqlDatabase::database(connectionName, false)
                .setHostName(host);
    });
}

void MSqlDatabase::setDatabaseName(const QString &name)
{
    QString connectionName = m_connectionName;
    PostToWorker(workerForConnection(connectionName), [=]{
        QSqlDatabase::database(connectionName, false)
                .setDatabaseName(name);
    });
}

void MSqlDatabase::setUserName(const QString &name)
{
    QString connectionName = m_connectionName;
    PostToWorker(workerForConnection(connectionName), [=]{
        QSqlDatabase::database(connectionName, false)
                .setUserName(name);
    });
}

void MSqlDatabase::setPassword(const QString& password)
{
    QString connectionName = m_connectionName;
    PostToWorker(workerForConnection(connectionName), [=]{
        QSqlDatabase::database(connectionName, false)
                .setPassword(password);
    });
}

void MSqlDatabase::setConnectionOptions(const QString &options)
{
    QString connectionName = m_connectionName;
    PostToWorker(workerForConnection(connectionName), [=]{
        QSqlDatabase::database(connectionName, false)
                .setConnectOptions(options);
    });
}

void MSqlDatabase::setPort(int port)
{
    QString connectionName = m_connectionName;
    PostToWorker(workerForConnection(connectionName), [=]{
        QSqlDatabase::database(connectionName, false)
                .setPort(port);
    });
}

QString MSqlDatabase::hostName()
{
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).hostName();
    });
}

QString MSqlDatabase::databaseName()
{
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).databaseName();
    });
}

QString MSqlDatabase::userName()
{
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).userName();
    });
}

QString MSqlDatabase::password()
{
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).password();
    });
}

QString MSqlDatabase::connectionOptions()
{
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).connectOptions();
    });
}

int MSqlDatabase::port()
{
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).port();
    });
}

bool MSqlDatabase::transaction()
{
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).transaction();
    });
}

bool  MSqlDatabase::commit()
{
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).commit();
    });
}

bool MSqlDatabase::rollback()
{
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).rollback();
    });
}

QSqlError MSqlDatabase::lastError()const
{
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).lastError();
    });
}

bool MSqlDatabase::open()
{
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).open();
    });
}

void MSqlDatabase::close()
{
    QString connectionName = m_connectionName;
    CallByWorker(workerForConnection(connectionName), [=]{
        QSqlDatabase::database(connectionName, false).close();
    });
}

bool MSqlDatabase::isOpen()const
{
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).isOpen();
    });
}

bool MSqlDatabase::isOpenError()const
{
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).isOpenError();
    });
}

bool MSqlDatabase::isValid()const
{
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).isValid();
    });
}

bool MSqlDatabase::subscribeToNotification(const QString &name)
{
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).driver()->subscribeToNotification(name);
    });
}

QStringList MSqlDatabase::subscribedToNotifications()const
{
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).driver()->subscribedToNotifications();
    });
}

bool MSqlDatabase::unsubscribeFromNotification(const QString &name)
{
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).driver()->unsubscribeFromNotification(name);
    });
}

const QSqlDriver *MSqlDatabase::driver()
{
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).driver();
    });
}

QThread* MSqlDatabase::threadForConnection(QString connectionName) const {
    Q_UNUSED(connectionName) //using currently a singleton thread for all connections
    //this should be replaced with a single QThread per db connection
    return MSqlThread::instance();
}

QObject* MSqlDatabase::workerForConnection(QString connectionName) const {
    return threadForConnection(connectionName)->eventDispatcher();
}
