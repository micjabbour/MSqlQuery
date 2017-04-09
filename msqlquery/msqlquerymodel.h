#ifndef MSQLQUERYMODEL_H
#define MSQLQUERYMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QSqlRecord>
#include "msqldatabase.h"

class MSqlQuery;


//avoid deleting any MSqlQueryObject while it is retrieving data (ie. by closing its parent dialog)
//that will cause the MDbThread to write its results into invalid memory

class MSqlQueryModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit MSqlQueryModel(QObject *parent = 0);
    ~MSqlQueryModel();
    
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool isBusy()const;
    
    
    //! Resets the model and sets the data provider to be the given query, returns immediately, does not block.
    //! If the function is called while model was busy executing another query,
    //! the model executes the query set by this function as soon as it is done with the current query
    //! 
    //! The provided query object should be prepar()ed, but NOT exec()ed.
    //! 
    //! WARNING: the function takes ownership of the query object, do NOT destroy it yourself.
    Q_INVOKABLE void setQueryAsync(MSqlQuery* query);
    
    //! Resets the model and sets the data provider to be the given query. this function blocks, until the query is finished.
    //! This function does basically the same thing done by QSqlQueryModel::setQuery.
    //! It just calls QSqlQueryModel::setQuery from the other thread. and blocks until the query finishes.
    //! 
    //! The provided query object should be prepar()ed, but NOT exec()ed.
    //! 
    //! WARNING: the function takes ownership of the query object, do NOT destroy it yourself.
    Q_INVOKABLE void setQuery(MSqlQuery* query);
    
    //! Resets the model and sets the data provider to be the given query, returns immediately, does not block.
    //! If the function is called while model was busy executing another query,
    //! the model executes the query set by this function as soon as it is done with the current query
    Q_INVOKABLE void setQueryAsync(const QString& query,  const QString& dbConnectionName = MSqlDatabase::defaultConnectionName);
    
    //! Resets the model and sets the data provider to be the given query. this function blocks, until the query is finished.
    //! This function does basically the same thing done by QSqlQueryModel::setQuery.
    //! It just calls QSqlQueryModel::setQuery from the other thread. and blocks until the query finishes.
    Q_INVOKABLE void setQuery(const QString& query,  const QString& dbConnectionName = MSqlDatabase::defaultConnectionName);
    
signals:
    
public slots:

    
private slots:
    void queryGotResults(bool success);
private:
    void HandleQuery(MSqlQuery* query);
    void HandleQueryFromStr(const QString& query, const QString& dbConnectionName= MSqlDatabase::defaultConnectionName);
    
    
    MSqlQuery* m_query;
    QList<QSqlRecord> m_recList;
    QString m_nextQueryToExecute;
    QString m_nextQueryDbConnectionName;
    MSqlQuery* m_nextMQueryToExecute;
};

#endif // MSQLQUERYMODEL_H
