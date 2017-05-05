#include "msqlquerymodel.h"
#include "msqlquery.h"

MSqlQueryModel::MSqlQueryModel(QObject *parent)
    : QAbstractTableModel(parent), m_query(nullptr) {
}

MSqlQueryModel::~MSqlQueryModel(){
}

int MSqlQueryModel::rowCount(const QModelIndex &parent) const {
    if(parent.isValid()) return 0;
    return m_records.size();
}

int MSqlQueryModel::columnCount(const QModelIndex &parent) const {
    if(parent.isValid()) return 0;
    if(!m_records.isEmpty())
        return m_records.first().count();
    else
        return 0;
}

QVariant MSqlQueryModel::data(const QModelIndex &index, int role) const {
    if(role == Qt::DisplayRole || role == Qt::EditRole)
        return m_records.at(index.row()).value(index.column());
    
    return QVariant();
}

QVariant MSqlQueryModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role == Qt::DisplayRole) {
        if(orientation == Qt::Horizontal) {
            if(!m_records.isEmpty()) {
                return m_records.first().fieldName(section);
            }
        }
        if(orientation == Qt::Vertical) {
            return QString::number(section);
        }
    }
    return QVariant();
}

void MSqlQueryModel::setQuery(MSqlQuery* query) {
    delete m_query; //delete old m_query
    //MSqlQuery::exec() should be called on the query object
    m_query= query;
    m_query->setParent(this); //take ownership
    queryGotResults(true);
}

void MSqlQueryModel::setQueryAsync(MSqlQuery* query) {
    delete m_query; //delete old m_query
    m_query= query;
    m_query->setParent(this); //take ownership
    connect(query, &MSqlQuery::resultsReady, this, &MSqlQueryModel::queryGotResults);
}

void MSqlQueryModel::setQuery(const QString &query, const QString &dbConnectionName){
    delete m_query; //delete old m_query
    m_query = new MSqlQuery(this, MSqlDatabase::database(dbConnectionName));
    bool success = m_query->exec(query);
    queryGotResults(success);
}

void MSqlQueryModel::setQueryAsync(const QString &query, const QString &dbConnectionName){
    delete m_query; //delete old m_query
    m_query = new MSqlQuery(this, MSqlDatabase::database(dbConnectionName));
    m_query->execAsync(query);
    connect(m_query, &MSqlQuery::resultsReady, this, &MSqlQueryModel::queryGotResults);
}

void MSqlQueryModel::queryGotResults(bool success){
    if(success) {
        //copy results to the internal m_recList
        beginResetModel();
        m_records = m_query->getAllRecords();
        endResetModel();
    } else {
        qCritical("MSqlQueryModel::queryGotResults success is false");
    }
}

bool MSqlQueryModel::isBusy()const{
    return m_query->isBusy();
}
