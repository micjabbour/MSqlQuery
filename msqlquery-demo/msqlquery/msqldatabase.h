#ifndef MSQLDATABASE_H
#define MSQLDATABASE_H
#include <QString>
#include <QSqlError>

class QSqlDriver;
class QObject;
class MSqlThread;

/*!
 * \brief The MSqlDatabase class represents a database connection that can be
 *        used from MSqlQuery.
 *
 * The class provides an interface similar to QSqlDatabase. Create a connection
 * by calling the static addDatabase() function, This connection can afterwards
 * be used from MSqlQuery. If you don't pass the connection name argument,
 * MSqlQuery's default connection is assumed (this is different from QSqlQuery's
 * default connection). The following snippet shows how to create and open a
 * default MSqlQuery connection to a PostgreSQL database (similar to the
 * QSqlDatabase example):
 *
 *     MSqlDatabase db = MSqlDatabase::addDatabase("QPSQL");
 *     db.setHostName("acidalia");
 *     db.setDatabaseName("customdb");
 *     db.setUserName("mojito");
 *     db.setPassword("J0a1m8");
 *     bool ok = db.open();
 *
 * The class supports all connection types that are supported in QSqlDatabase.
 *
 * Internally, the class sets up a QSqlDatabase connection and a QThread, and
 * ensures that all accesses to that connection is done from its reserved
 * QThread.
 *
 * \warning Connections that are set up using MSqlDatabase should be used only
 *          from MSqlQuery classes (So that MSqlQuery can ensure that calls are
 *          done in the right thread).
 *
 * \warning Connections that are set up using QSqlDatabase should be used only
 *          from QSqlQuery classes (the caller is responsible to ensure that a
 *          connection is accessed only from the thread that created it).
 */
class MSqlDatabase
{
public:
    friend class MSqlQuery;
    ~MSqlDatabase();
    /*!
     * \brief Similar to QSqlDatabase::addDatabase.
     *
     * Adds a database to the list of database connections using the supplied
     * driver type and the connectionName. Internally, The database connection
     * is created in a background thread managed by the library. When using the
     * library to execute queries on that database connection, all calls are
     * internally done in the connection's background thread.
     *
     * \param type a registered driver type
     * \param connectionName
     * \return an uninitialized MSqlDatabase object that controls the added
     *         database connection.
     */
    static MSqlDatabase addDatabase(const QString& type,
                                    const QString& connectionName = defaultConnectionName);
    /*!
     * \brief Similar to QSqlDatabase::database.
     *
     * Returns the database connection called *connectionName*. If no
     * connectionName is specified, MSqlQuery's default connection is used.
     *
     * \param connectionName
     * \return an MSqlDatabase object that controls the specified connection.
     */
    static MSqlDatabase database(const QString& connectionName = defaultConnectionName);
    /*!
     * \brief Similar to QSqlDatabase::setHostName.
     *
     * Sets the connection's host name to *host*.
     *
     * \param host new host name.
     */
    void setHostName(const QString& host);
    /*!
     * \brief Similar to QSqlDatabase::setDatabaseName.
     *
     * Sets the connection's database name to *name*.
     *
     * \param name database name
     */
    void setDatabaseName(const QString& name);
    /*!
     * \brief Similar to QSqlDatabase::setUserName.
     *
     * Sets the connection's user name to *name*.
     *
     * \param name username
     */
    void setUserName(const QString& name);
    /*!
     * \brief Similar to QSqlDatabase::setPassword.
     *
     * Sets the connection's password to *password*.
     *
     * \param password password
     */
    void setPassword(const QString& password);
    /*!
     * \brief Similar to QSqlDatabase::setConnectOptions.
     *
     * Sets database-specific options.
     *
     * \param options a QString of a semicolon separated list of option names or
     *                option=value pairs.
     */
    void setConnectOptions(const QString& options = QString());
    /*!
     * \brief Similar to QSqlDatabase::setPort
     *
     * Sets the connection's port number to *port*.
     *
     * \param port port.
     */
    void setPort(int port);
    /*!
     * \brief Similar to QSqlDatabase::connectionName.
     *
     * \return the connection name.
     */
    QString connectionName()const{return m_connectionName;}
    /*!
     * \brief Similar to QSqlDatabase::hostName.
     * \return the connection's host name.
     *
     * \warning The function is implemented by calling QSqlDatabase::hostName in
     *          the database connection's thread and returning its result.
     *          This means that if the database connection's thread is busy
     *          (executing a query for example), this function might block the
     *          calling thread until the ongoing query finishes.
     */
    QString hostName()const;
    /*!
     * \brief Similar to QSqlDatabase::databaseName.
     * \return the connection's database name.
     *
     * \warning The function is implemented by calling QSqlDatabase::databaseName
     *          in the database connection's thread and returning its result.
     *          This means that if the database connection's thread is busy
     *          (executing a query for example), this function might block the
     *          calling thread until the ongoing query finishes.
     */
    QString databaseName()const;
    /*!
     * \brief Similar to QSqlDatabase::userName.
     * \return the connection's username.
     *
     * \warning The function is implemented by calling QSqlDatabase::userName in
     *          the database connection's thread and returning its result.
     *          This means that if the database connection's thread is busy
     *          (executing a query for example), this function might block the
     *          calling thread until the ongoing query finishes.
     */
    QString userName()const;
    /*!
     * \brief Similar to QSqlDatabase::password.
     * \return the connection's password.
     *
     * \warning The function is implemented by calling QSqlDatabase::password in
     *          the database connection's thread and returning its result.
     *          This means that if the database connection's thread is busy
     *          (executing a query for example), this function might block the
     *          calling thread until the ongoing query finishes.
     */
    QString password()const;
    /*!
     * \brief Similar to QSqlDatabase::connectOptions.
     * \return the connection options used for this connection.
     *
     * \warning The function is implemented by calling QSqlDatabase::connectOptions
     *          in the database connection's thread and returning its result.
     *          This means that if the database connection's thread is busy
     *          (executing a query for example), this function might block the
     *          calling thread until the ongoing query finishes.
     */
    QString connectOptions()const;
    /*!
     * \brief Similar to QSqlDatabase::transaction.
     *
     * Begins a transaction on the database if the driver supports transactions.
     *
     * \return true if the operation succeeded. Otherwise it returns false.
     *
     * \warning The function is implemented by calling QSqlDatabase::transaction
     *          in the database connection's thread and returning its result.
     *          This means that if the database connection's thread is busy
     *          (executing a query for example), this function might block the
     *          calling thread until the ongoing query finishes.
     *
     * \sa commit(), rollback()
     */
    bool transaction();
    /*!
     * \brief Similar to QSqlDatabase::commit.
     *
     * Commits a transaction to the database of thee driver supports
     * transactions and a transation() has been started.
     *
     * \return true if the operation succeeded. Otherwise it returns false.
     *
     * \warning The function is implemented by calling QSqlDatabase::commit in
     *          the database connection's thread and returning its result.
     *          This means that if the database connection's thread is busy
     *          (executing a query for example), this function might block the
     *          calling thread until the ongoing query finishes.
     *
     * \sa transaction(), rollback()
     */
    bool commit();
    /*!
     * \brief Similar to QSqlDatabase::rollback.
     *
     * Rolls back a transaction on the database if the driver supports
     * transactions and a transaction() has been started.
     *
     * \return true if the operation succeeded. Otherwisse it returns false.
     *
     * \warning The function is implemented by calling QSqlDatabase::rollback in
     *          the database connection's thread and returning its result.
     *          This means that if the database connection's thread is busy
     *          (executing a query for example), this function might block the
     *          calling thread until the ongoing query finishes.
     *
     * \sa transaction(), commit()
     */
    bool rollback();
    /*!
     * \brief Similar to QSqlDatabase::port.
     * \return the connection's port number.
     *
     * \warning The function is implemented by calling QSqlDatabase::port in
     *          the database connection's thread and returning its result.
     *          This means that if the database connection's thread is busy
     *          (executing a query for example), this function might block the
     *          calling thread until the ongoing query finishes.
     */
    int port();
    /*!
     * \brief Similar to QSqlDatabase::lastError.
     * \return information about the last error that occured on the database.
     *
     * \warning The function is implemented by calling QSqlDatabase::lastError
     *          in the database connection's thread and returning its result.
     *          This means that if the database connection's thread is busy
     *          (executing a query for example), this function might block the
     *          calling thread until the ongoing query finishes.
     *          Use MSqlQuery::lastError for a non-blocking solution.
     *
     * \sa MSqlQuery::lastError
     */
    QSqlError lastError()const;
    /*!
     * \brief Similar to QSqlDatabase::open
     *
     * Opens the database connection using the current connection values.
     *
     * \return true on success. Otherwise, it returns false.
     *
     * \warning The function is implemented by calling QSqlDatabase::open in the
     *          database connection's thread and returning its result.
     *          This means that if the database connection's thread is busy
     *          (executing a query for example), this function might block the
     *          calling thread until the ongoing query finishes.
     */
    bool open();
    /*!
     * \brief Similar to QSqlDatabase::close
     *
     * Closes the database connection, freeing any resources acquired, and
     * invalidating any MSqlQuery objects that are used with the database.
     *
     * \warning The function is implemented by calling QSqlDatabase::close in
     *          the database connection's thread and returning its result.
     *          This means that if the database connection's thread is busy
     *          (executing a query for example), this function might block the
     *          calling thread until the ongoing query finishes.
     */
    void close();
    
    
    //notifications support
    
    /*!
     * \brief Similar to QSqlDatabase::driver()->%subscribeToNotification()
     *
     * Call this function to subscribe to event notifications from the database.
     *
     * \warning Do not use MSqlDatabase::driver()->%subscribeToNotification(),
     *          (because it will try to access the driver from the current
     *          thread). Instead, call this function.
     *
     * \warning The function is implemented by calling
     *          QSqlDatabase::driver()->%subscribeToNotification() in the
     *          database connection's thread and returning its result.
     *          This means that if the database connection's thread is busy
     *          (executing a query for example), this function might block the
     *          calling thread until the ongoing query finishes.
     *
     * \param name identifies the event notification
     * \return true if the operation succeeded. Otherwisse it returns false.
     * \sa subscribedToNotifications(), unsubscribeFromNotification(), driver()
     */
    bool subscribeToNotification(const QString & name);
    /*!
     * \brief Similar to QSqlDatabase::driver()->%subscribedToNotifications()
     *
     * \warning Do not use MSqlDatabase::driver()->%subscribedToNotifications(),
     *          (because it will try to access the driver from the current
     *          thread). Instead, call this function.
     *
     * \warning The function is implemented by calling
     *          QSqlDatabase::driver()->%subscribedToNotifications() in the
     *          database connection's thread and returning its result.
     *          This means that if the database connection's thread is busy
     *          (executing a query for example), this function might block the
     *          calling thread until the ongoing query finishes.
     *
     * \return a list of the names of the event notifications that are currently
     *         subscribed to.
     * \sa subscribeToNotification(), unsubscribeFromNotification(), driver()
     */
    QStringList subscribedToNotifications()const;
    /*!
     * \brief Similar to QSqlDatabase::driver()->%unsubscribeFromNotification()
     *
     * \warning Do not use MSqlDatabase::driver()->%unsubscribeFromNotification,
     *          (because it will try to access the driver from the current
     *          thread). Instead, call this function.
     *
     * \warning The function is implemented by calling
     *          QSqlDatabase::driver()->%unsubscribeFromNotification() in the
     *          database connection's thread and returning its result.
     *          This means that if the database connection's thread is busy
     *          (executing a query for example), this function might block the
     *          calling thread until the ongoing query finishes.
     * \param name event notification identifier name
     * \return true if the operation succeeded. Otherwisse it returns false.
     * \sa subscribeToNotification(), subscribedToNotifications(), driver()
     */
    bool unsubscribeFromNotification(const QString& name);
    /*!
     * \brief Similar to QSqlDatabase::driver().
     *
     * \warning The function returns a QSqlDriver object. Do not call any
     *          functions on the returned object. You can only connect to
     *          signals being emitted from this object (notification() signal).
     *
     * \return the database driver used to access the database connection. Any
     *         function call on the returned object may cause undefined behavior
     * \sa subscribeToNotification(), subscribedToNotifications(),
     *     unsubscribeFromNotification()
     */
    const QSqlDriver* driver();
    /*!
     * \brief Similar to QSqlDatabase::isOpen()
     *
     * \warning The function is implemented by calling QSqlDatabase::isOpen in
     *          the database connection's thread and returning its result.
     *          This means that if the database connection's thread is busy
     *          (executing a query for example), this function might block the
     *          calling thread until the ongoing query finishes.
     *
     * \return true if the database connection is currently open. Otherwise
     *         returns false
     */
    bool isOpen()const;
    /*!
     * \brief Similar to QSqlDatabase::isOpenError()
     *
     * \warning The function is implemented by calling QSqlDatabase::isOpenError
     *          in the database connection's thread and returning its result.
     *          This means that if the database connection's thread is busy
     *          (executing a query for example), this function might block the
     *          calling thread until the ongoing query finishes.
     *
     * \return true if there was an error opening the database connection.
     *         Otherwise returns false.
     */
    bool isOpenError()const;
    /*!
     * \brief Similar to QSqlDatabase::isValid()
     *
     * \warning The function is implemented by calling QSqlDatabase::isValid
     *          in the database connection's thread and returning its result.
     *          This means that if the database connection's thread is busy
     *          (executing a query for example), this function might block the
     *          calling thread until the ongoing query finishes.
     *
     * \return true if the QSqlDatabase has a valid driver.
     */
    bool isValid()const;
    static const QString defaultConnectionName;
private:
    static MSqlThread* threadForConnection(QString connectionName);
    static QObject* workerForConnection(QString connectionName);
    MSqlDatabase();
    QString m_connectionName;
};

#endif // MSQLDATABASE_H
