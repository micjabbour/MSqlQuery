#include "msqldatabase.h"
#include "qthreadutils.h"
#include "msqlthread.h"
#include <QSqlDatabase>
#include <QStringList>
#include <QSqlDriver>
#include <QDebug>
#include <QReadWriteLock>
#include <QWriteLocker>
#include <QReadLocker>
#include <QGlobalStatic>
#include <QCoreApplication>

const QString MSqlDatabase::defaultConnectionName(QString(QSqlDatabase::defaultConnection)+"_msqlquery_default");

struct MSqlConnections {
    QHash<QString, MSqlThread*> dict;
    bool isPostRoutineAdded = false;
    mutable QReadWriteLock lock;
};
Q_GLOBAL_STATIC(MSqlConnections, getMSqlConnections)

static void MSqlCleanup() {
    //must be called before QSqlDatabase cleanup routine
    //so, it must be added after QSqlDatabase
    MSqlConnections* connections = getMSqlConnections();
    QWriteLocker locker(&connections->lock);
    //destruct all connection's threads 
    //this causes calling thread to block until all threads are terminated
    for(auto i = connections->dict.begin(); i!=connections->dict.end(); ++i)
        delete i.value();
    //make sure any of the threads getting destructed do not attempt to access
    //MSqlConnections because this will cause a deadlock
    connections->dict.clear();
}


MSqlDatabase::MSqlDatabase() {
    
}

MSqlDatabase::~MSqlDatabase() {
    
}

MSqlDatabase MSqlDatabase::addDatabase(const QString &type, const QString &connectionName) {
    MSqlDatabase db;
    db.m_connectionName = connectionName;
    MSqlConnections* connections = getMSqlConnections();
    QWriteLocker locker(&connections->lock);
    Q_UNUSED(locker)
    if(connections->dict.contains(connectionName)){
        //destruct and remove old connection if one already exists
        delete connections->dict.value(connectionName);
        connections->dict.remove(connectionName);
    }
    //create new thread for connection
    MSqlThread* thread = new MSqlThread();
    connections->dict.insert(connectionName, thread);
    //create database connection in newly created thread
    CallByWorker(thread->getWorker(), [=]{
        QSqlDatabase::addDatabase(type, connectionName);
    });
    if(!connections->isPostRoutineAdded){ //if post routine not registered
        //register post routine after calling QSqlDatabase::addDatabase
        //since postRoutines are called in reverse order of their addition
        qAddPostRoutine(MSqlCleanup);
        connections->isPostRoutineAdded= true;
    }
    return db;
}

MSqlDatabase MSqlDatabase::database(const QString &connectionName) {
    MSqlDatabase db;
    db.m_connectionName = connectionName;
    return db;
}

void MSqlDatabase::setHostName(const QString &host) {
    QString connectionName = m_connectionName;
    PostToWorker(workerForConnection(connectionName), [=]{
        QSqlDatabase::database(connectionName, false)
                .setHostName(host);
    });
}

void MSqlDatabase::setDatabaseName(const QString &name) {
    QString connectionName = m_connectionName;
    PostToWorker(workerForConnection(connectionName), [=]{
        QSqlDatabase::database(connectionName, false)
                .setDatabaseName(name);
    });
}

void MSqlDatabase::setUserName(const QString &name) {
    QString connectionName = m_connectionName;
    PostToWorker(workerForConnection(connectionName), [=]{
        QSqlDatabase::database(connectionName, false)
                .setUserName(name);
    });
}

void MSqlDatabase::setPassword(const QString& password) {
    QString connectionName = m_connectionName;
    PostToWorker(workerForConnection(connectionName), [=]{
        QSqlDatabase::database(connectionName, false)
                .setPassword(password);
    });
}

void MSqlDatabase::setConnectionOptions(const QString &options) {
    QString connectionName = m_connectionName;
    PostToWorker(workerForConnection(connectionName), [=]{
        QSqlDatabase::database(connectionName, false)
                .setConnectOptions(options);
    });
}

void MSqlDatabase::setPort(int port) {
    QString connectionName = m_connectionName;
    PostToWorker(workerForConnection(connectionName), [=]{
        QSqlDatabase::database(connectionName, false)
                .setPort(port);
    });
}

QString MSqlDatabase::hostName() {
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).hostName();
    });
}

QString MSqlDatabase::databaseName() {
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).databaseName();
    });
}

QString MSqlDatabase::userName() {
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).userName();
    });
}

QString MSqlDatabase::password() {
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).password();
    });
}

QString MSqlDatabase::connectionOptions() {
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).connectOptions();
    });
}

int MSqlDatabase::port() {
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).port();
    });
}

bool MSqlDatabase::transaction() {
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).transaction();
    });
}

bool  MSqlDatabase::commit() {
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).commit();
    });
}

bool MSqlDatabase::rollback() {
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).rollback();
    });
}

QSqlError MSqlDatabase::lastError()const {
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).lastError();
    });
}

bool MSqlDatabase::open() {
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).open();
    });
}

void MSqlDatabase::close() {
    QString connectionName = m_connectionName;
    CallByWorker(workerForConnection(connectionName), [=]{
        QSqlDatabase::database(connectionName, false).close();
    });
}

bool MSqlDatabase::isOpen()const {
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).isOpen();
    });
}

bool MSqlDatabase::isOpenError()const {
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).isOpenError();
    });
}

bool MSqlDatabase::isValid()const {
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).isValid();
    });
}

bool MSqlDatabase::subscribeToNotification(const QString &name) {
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).driver()->subscribeToNotification(name);
    });
}

QStringList MSqlDatabase::subscribedToNotifications()const {
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).driver()->subscribedToNotifications();
    });
}

bool MSqlDatabase::unsubscribeFromNotification(const QString &name) {
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).driver()->unsubscribeFromNotification(name);
    });
}

const QSqlDriver *MSqlDatabase::driver() {
    QString connectionName = m_connectionName;
    return CallByWorker(workerForConnection(connectionName), [=]{
        return QSqlDatabase::database(connectionName, false).driver();
    });
}

MSqlThread* MSqlDatabase::threadForConnection(QString connectionName) {
    MSqlConnections* connections = getMSqlConnections();
    QReadLocker locker(&connections->lock);
    return connections->dict.value(connectionName, nullptr);
}

QObject* MSqlDatabase::workerForConnection(QString connectionName) {
    return threadForConnection(connectionName)->getWorker();
}
