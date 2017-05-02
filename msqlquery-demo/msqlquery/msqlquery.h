#ifndef MSQLQUERY_H
#define MSQLQUERY_H

#include <QObject>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QVariant>
#include "msqldatabase.h"
#include <QMutex>

class MSqlQueryWorker;

//all functions in this class do NOT block EXCEPT the exec() function
//use execAsync() and connect to resultsReady() signal instead

/*!
 * \brief The MSqlQuery class provides means of executing a single SQL statement
 *        synchronously or asynchronously.
 *
 * The class provides a subset of the interface of QSqlQuery, in addition to
 * some asynchronous functions. It can be used to execute any kind of SQL
 * statement on a database and get results synchronously or asynchronously. Most
 * of the functions in the class are just wrappers that call corresponding Qt
 * functions in the "right" thread.
 *
 * The class has a synchronous API, that can be used in the same way QSqlQuery
 * is used. The following snippet shows how to execute an SQL query and retrieve
 * results synchronously using MSqlQuery (similar to the QSqlQuery example):
 *
 *     MSqlQuery query("SELECT country FROM artist");
 *     while (query.next()) {
 *         QString country = query.value(0).toString();
 *         doSomething(country);
 *     }
 *
 * The class also has an asynchronous API, that can be used to execute a query
 * asynchronously. It works by calling functions that end with `Async()` (e.g.
 * execAsync(), execBatchAsync()). These functions emit a signal resultsReady()
 * when execution is finished. You can connect to this signal, and access the
 * results the same way as when using the synchronous API. Here is the same
 * example above written asynchronously:
 *
 *     //create an MSqlQuery object, and provide some parent to manage its lifetime
 *     MSqlQuery* query = new MSqlQuery(parent);
 *     //connect to resultsReady signal
 *     QObject::connect(&query, &MSqlQuery::resultsReady, [=](bool success){
 *         if(success) {
 *     	    while (query->next()) {
 *             QString country = query->value(0).toString();
 *             doSomething(country);
 *     	} else {
 *     	    //statement failed to execute
 *     		qDebug() << query->lastError().databaseText();
 *     	}
 *     });
 *     //make sure to keep the lifetime of the MSqlQuery object as long as you
 *     //don't want to cancel the ongoing operation
 *
 * The class also supports using QSqlQuery's batch execution mode. This can be
 * done using the functions execBatch (for synchronous execution) and
 * execBatchAsync (for asynchronous execution)
 */
class MSqlQuery : public QObject {
    Q_OBJECT
public:
    /*!
     * \brief constructs a new %MSqlQuery object.
     *
     * The new object uses the supplied MSqlDatabase connection
     *
     * \param parent parent QObject
     * \param db the MSqlDatabase connection to use
     */
    explicit MSqlQuery(QObject *parent = 0, MSqlDatabase db = MSqlDatabase::database());

    /*!
     * \brief destroys the %MSqlQuery object.
     *
     * The destructor will try to cancel any ongoing asynchronous executions
     */
    ~MSqlQuery();

    /*!
     * \brief Similar to QSqlQuery::prepare.
     *
     * The function prepares an SQL statement for execution.
     *
     * \note The function is fully asynchronous
     *
     * \warning Unlike QSqlQuery::prepare, this function does not report
     *          success or failure. This is by design (in order to make the
     *          function fully asynchronous). You can get failure/success status
     *          when executing the statement (using the resultsReady signal).
     *
     * \param query query statment to prepare
     */
    void prepare(const QString& query);

    /*!
     * \brief Similar to QSqlQuery::addBindValue.
     *
     * Adds the value *val* to the list of values when using positional binding.
     *
     * \note The function is fully asynchronous
     *
     * \param val value to bind
     * \param paramType can be QSql::In, QSql::Out, or QSql::InOut
     */
    void addBindValue(const QVariant& val, QSql::ParamType paramType = QSql::In);
    /*!
     * \brief Similar to QSqlQuery::bindValue.
     *
     * Binds the value *val* to the placeholder *placeholder*.
     *
     * \note The function is fully asynchronous
     *
     * \param placeholder the placeholder to bind the value to
     * \param val the value that needs to be bound
     * \param paramType can be QSql::In, QSql::Out, or QSql::InOut
     */
    void bindValue(const QString& placeholder, const QVariant& val, QSql::ParamType paramType = QSql::In);

    /*!
     * \brief Similar to QSqlQuery::exec(const QString& query).
     *
     * Executes the SQL statement in *query* synchronously.
     *
     * \note The function blocks the calling thread until execution is finished.
     *
     * \param query the SQL statement to execute
     * \return true on success, false otherwise.
     * \sa execAsync(const QString& query).
     */
    bool exec(const QString& query);

    /*!
     * \brief Similar to QSqlQuery::exec().
     *
     * Executes a previously prepared SQL statement.
     *
     * \note The function blocks the calling thread until execution is finished.
     *
     * \return true on success, false otherwise.
     * \sa execAsync()
     */
    bool exec();
    /*!
     * \brief Similar to QSqlQuery::execBatch
     *
     * Executes a previously prepared SQL query in a batch.
     *
     * \note The function blocks the calling thread until execution is finished.
     *
     * \param mode specifies batch execution mode (similar to QSqlQuery)
     * \return true on success, false otherwise.
     */
    bool execBatch(QSqlQuery::BatchExecutionMode mode = QSqlQuery::ValuesAsRows);
    /*!
     * \brief Similar to QSqlQuery::next
     *
     * Positions the query on the next record
     * \return true on success, false otherwise.
     *
     * \note The function is fully asynchronous
     *
     * \warning When using any of %MSqlQuery's exec functions, execution is not
     *          finished until all queries are retrieved. This might be slower
     *          in some situations, but it is by design (in order to make
     *          functions like this fully  asynchronous).
     */
    bool next();
    /*!
     * \brief Similar to QSqlQuery::seek
     * Positions the query on the record at position *index*
     * \param index record index
     * \return true on success, false otherwise.
     *
     * \note The function is fully asynchronous
     *
     * \warning When using any of %MSqlQuery's exec functions, execution is not
     *          finished until all queries are retrieved. This might be slower
     *          in some situations, but it is by design (in order to make
     *          functions like this fully  asynchronous).
     */
    bool seek(int index);
    /*!
     * \brief Similar to QSqlQuery::record
     *
     * \return a `QSqlRecord` that contains the current row's values.
     *
     * \note The function is fully asynchronous
     *
     * \warning When using any of %MSqlQuery's exec functions, execution is not
     *          finished until all queries are retrieved. This might be slower
     *          in some situations, but it is by design (in order to make
     *          functions like this fully  asynchronous).
     */
    QSqlRecord record() const;
    /*!
     * \brief value
     * \param index
     * \return 
     */
    QVariant value(int index) const;
    /*!
     * \brief value
     * \param name
     * \return 
     */
    QVariant value(const QString& name) const;
    /*!
     * \brief lastError
     * \return
     */
    QSqlError lastError() const;
    /*!
     * \brief execAsync
     * \param query
     */
    void execAsync(const QString& query);
    /*!
     * \brief execAsync
     */
    void execAsync();
    void execBatchAsync(QSqlQuery::BatchExecutionMode mode = QSqlQuery::ValuesAsRows);
    QString getDbConnectionName()const{return db.connectionName();}
    QVariant lastInsertId()const;

    //additional functions
    bool isBusy()const;
    QList<QSqlRecord> getAllRecords() const;
signals:
    void resultsReady(bool success);
    void busyToggled(bool isBusy);
private:
    Q_INVOKABLE void workerFinished(int queryId, bool success);
    
    bool execNextBlocking();
    //pointer accessed only from the client thread
    //passed to worker threads through lambdas capturing it by value, lives in database connection thread
    MSqlQueryWorker* w;
    MSqlDatabase db;
    bool m_isBusy = false; //accessed only from client thread
    //when a query is finished, its id is checked to make sure that it matches currentQueryId
    //(in order to emit resultsReady signal only for the last query set on this object)
    int currentQueryId = -1;
};

//the worker object lives in the database connection's thread and owns the QSqlQuery instance
//this class is internal to the library
//all functions in the worker object are accessed from the worker thread only, except
//functions marked as thread-safe
class MSqlQueryWorker : public QObject {
    Q_OBJECT
public:
    using PlaceHolderBind = std::tuple<QString, QVariant, QSql::ParamType>;
    using PositionalBind = std::tuple<QVariant, QSql::ParamType>;

    explicit MSqlQueryWorker(); //worker does not have a parent
    ~MSqlQueryWorker();
    QSqlQuery* q; //accessed only from worker threads
    //the following functions are thread-safe
    void prepare(const QString &query);
    void bindValue(const QString &placeholder, const QVariant &val, QSql::ParamType paramType);
    void addBindValue(const QVariant& val, QSql::ParamType paramType = QSql::In);
    void execAsync(int queryId, bool isBatch = false, QSqlQuery::BatchExecutionMode batchMode = QSqlQuery::ValuesAsRows);
    bool next();
    bool seek(int index);
    QSqlRecord record() const;
    QVariant lastInsertId() const;
    QSqlError lastError() const;
    void setNextQueryReady(bool isReady, bool isBatch = false, QSqlQuery::BatchExecutionMode batchMode = QSqlQuery::ValuesAsRows);
    bool isBusy() const;
    bool hasNextQuery() const;
    QList<QSqlRecord> getAllRecords() const;

    Q_SIGNAL void resultsReady(int queryId, bool success);
    Q_INVOKABLE void execNextQuery(); //always invoked in worker thread
private:
    mutable QMutex mutex;
    struct SqlQueryExec {
        //every query to be executed has an id, so that it can be overwritten later
        //when a query is finished, its id is checked to make sure that it has not been overwritten
        //(in order to emit resultsReady signal only for the last query set on this object)
        int queryId = -1;
        QString prepareStr;
        QList<PlaceHolderBind> placeHolderBinds;
        QList<PositionalBind> positionalBinds;
        bool isBatch = false;
        QSqlQuery::BatchExecutionMode batchMode;
        bool isReady = false;
    } m_nextQuery;
    QList<QSqlRecord> m_records; //to store query result
    int m_currentItem = -1; //before first item
    bool m_isBusy = false;
    QSqlError m_lastError;
    QVariant m_lastInsertId; //to store query last insert id
};

#endif // MSQLQUERY_H
