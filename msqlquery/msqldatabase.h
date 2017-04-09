#ifndef MSQLDATABASE_H
#define MSQLDATABASE_H
#include <QString>
#include <QSqlError>

class QSqlDriver;

class MSqlDatabase //provides an interface similar to QSqlDatabase except that all connections are created in the MDbThread (singleton)
{
public:
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
    
    QSqlDriver* driver(); //you can only connect signals from here, do not call any functions on the returned object directly, as this has to be done from the database thread
    bool subscribeToNotification(const QString & name);
    QStringList subscribedToNotifications()const;
    bool unsubscribeFromNotification(const QString& name);
    
    
    bool isOpen()const;
    bool isOpenError()const;
    bool isValid()const;
    static const QString defaultConnectionName;
private:
    MSqlDatabase();
    QString m_connectionName;
};

#endif // MSQLDATABASE_H
