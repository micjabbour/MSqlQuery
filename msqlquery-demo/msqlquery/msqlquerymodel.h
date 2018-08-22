#ifndef MSQLQUERYMODEL_H
#define MSQLQUERYMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QSqlRecord>
#include "msqldatabase.h"

class MSqlQuery;

/*!
 * \brief The MSqlQueryModel class provides a read-only data model for accessing
 *        SQL result sets synchronously or asynchronously.
 * 
 * The class provides a subset of the interface of QSqlQueryModel, in addition 
 * to some asynchronous functions. It can be used to provide data to Qt
 * view classes (such as `QTableView`) synchronously (block the GUI thread until
 * results are ready) or asynchronously (update the view when results get
 * ready). It can be used in the same way `QSqlQueryModel` is usually used, for
 * example:
 * 
 *     MSqlQueryModel *model = new MSqlQueryModel;
 *     model->setQuery("SELECT name, salary FROM employee");
 *     
 *     QTableView *view = new QTableView;
 *     view->setModel(model);
 *     view->show();
 * 
 * The above example will block the executing thread until the query returns (
 * the same way `QSqlQueryModel` behaves). MSqlQueryModel can be used to update
 * data asynchronously. You can call functions that end with `Async()` (e.g. 
 * setQueryAsync()) to do this. Here is the same example above written
 * asynchronously:
 * 
 *     MSqlQueryModel *model = new MSqlQueryModel;
 *     model->setQueryAsync("SELECT name, salary FROM employee");
 *     
 *     QTableView *view = new QTableView;
 *     view->setModel(model);
 *     view->show();
 * 
 * The above example works by running the query asynchronously, and resetting 
 * the model when the query is done. It does not block the GUI thread. The View
 * continues to display old data until the query is finished.
 * 
 * The model can also be subclassed (the same way `QSqlQueryModel` can) to
 * customize the contents of the data before showing it to the user.
 */

class MSqlQueryModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    /*!
     * \brief creates an empty MSqlQueryModel with the given parent
     * 
     * \param parent parent QObject
     */
    explicit MSqlQueryModel(QObject *parent = 0);
    /*!
     * \brief Destroys the model and frees any allocated resources
     */
    ~MSqlQueryModel();
    /*!
     * \brief returns the number of the rows the query returns
     * 
     * the function returns the number of all rows the query returns.
     * \param parent
     * \return 
     * 
     * \sa MSqlQuery, MSqlQuery::exec().
     */
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    /*!
     * \brief isBusy
     * \return 
     */
    bool isBusy()const;
    
    
    //! Resets the model and sets the data provider to be the given query, returns immediately, does not block.
    //! If the function is called while model was busy executing another query,
    //! the model executes the query set by this function as soon as it is done with the current query
    //! 
    //! The provided query object should be prepar()ed, but NOT exec()ed.
    //! 
    //! WARNING: the function takes ownership of the query object, do NOT destroy it yourself.
    void setQueryAsync(MSqlQuery* query);
    
    //! Resets the model and sets the data provider to be the given query. this function blocks, until the query is finished.
    //! This function does basically the same thing done by QSqlQueryModel::setQuery.
    //! It just calls QSqlQueryModel::setQuery from the other thread. and blocks until the query finishes.
    //! 
    //! The provided query object should be prepar()ed, but NOT exec()ed.
    //! 
    //! WARNING: the function takes ownership of the query object, do NOT destroy it yourself.
    void setQuery(MSqlQuery* query);
    
    //! Resets the model and sets the data provider to be the given query, returns immediately, does not block.
    //! If the function is called while model was busy executing another query,
    //! the model executes the query set by this function as soon as it is done with the current query
    void setQueryAsync(const QString& query,  const QString& dbConnectionName = MSqlDatabase::defaultConnectionName);
    
    //! Resets the model and sets the data provider to be the given query. this function blocks, until the query is finished.
    //! This function does basically the same thing done by QSqlQueryModel::setQuery.
    //! It just calls QSqlQueryModel::setQuery from the other thread. and blocks until the query finishes.
    void setQuery(const QString& query,  const QString& dbConnectionName = MSqlDatabase::defaultConnectionName);

    //! Aborts the current background operation in the model (if any).
    //! Please note that the SQL operation might continue in the background but
    //! the model will not show any of the newly retrieved data after calling
    //! this method.
    void abort();
    
private slots:
    void queryGotResults(bool success);
private:
    MSqlQuery* m_query;
    QList<QSqlRecord> m_records;
};

#endif // MSQLQUERYMODEL_H
