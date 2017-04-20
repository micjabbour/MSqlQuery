#ifndef MSQLDATABASE_H
#define MSQLDATABASE_H
#include <QString>
#include <QSqlError>

class QSqlDriver;
class QObject;
class MSqlThread;

class MSqlDatabase //provides an interface similar to QSqlDatabase except that all connections are created in the MDbThread
{
public:
    friend class MSqlQuery;
    ~MSqlDatabase();
    static MSqlDatabase addDatabase(const QString& type, const QString& connectionName = defaultConnectionName);
    static MSqlDatabase database(const QString& connectionName = defaultConnectionName);
    
    void setHostName(const QString& host);
    void setDatabaseName(const QString& name);
    void setUserName(const QString& name);
    void setPassword(const QString& password);
    void setConnectionOptions(const QString& options = QString());
    void setPort(int port);
    
    QString connectionName()const{return m_connectionName;}
    //warning: all the following functions block the calling thread
    QString hostName();
    QString databaseName();
    QString userName();
    QString password();
    QString connectionOptions();
    bool transaction();
    bool commit();
    bool rollback();
    int port();
    QSqlError lastError()const;
    bool open();
    void close();
    
    
    //notifications support
    
    //you can only connect signals from here, do not call any functions on the returned QSqlDriver directly
    //as this has to be done from the database thread
    const QSqlDriver* driver(); 
    //use the following functions to subscribe/unsubscribe to/from notifications
    //do NOT call the corresponding functions on the QSqlDriver object yourself
    bool subscribeToNotification(const QString & name);
    QStringList subscribedToNotifications()const;
    bool unsubscribeFromNotification(const QString& name);
    
    
    bool isOpen()const;
    bool isOpenError()const;
    bool isValid()const;
    static const QString defaultConnectionName;
private:
    static MSqlThread* threadForConnection(QString connectionName);
    static QObject* workerForConnection(QString connectionName);
    MSqlDatabase();
    QString m_connectionName;
};

#endif // MSQLDATABASE_H
