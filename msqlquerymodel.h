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
signals:
    
public slots:
    void setQuery(const QString& query,  const QString& dbConnectionName = MSqlDatabase::defaultConnectionName);
private slots:
    void queryGotResults(bool success);
private:
    MSqlQuery* m_query;
    QList<QSqlRecord> m_recList;
    QString nextQueryToExecute;
    QString nextQueryDbConnectionName;
};

#endif // MSQLQUERYMODEL_H
