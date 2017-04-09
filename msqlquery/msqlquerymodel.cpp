#include "msqlquerymodel.h"
#include "msqlquery.h"

MSqlQueryModel::MSqlQueryModel(QObject *parent)
    : QAbstractTableModel(parent), m_query(Q_NULLPTR), m_nextMQueryToExecute(Q_NULLPTR)
{}

MSqlQueryModel::~MSqlQueryModel(){
    if(m_query) delete m_query;
}

int MSqlQueryModel::rowCount(const QModelIndex &parent) const {
    if(parent.isValid()) return 0;
    return m_recList.size();
}

int MSqlQueryModel::columnCount(const QModelIndex &parent) const {
    if(parent.isValid()) return 0;
    if(!m_recList.isEmpty())
        return m_recList.first().count();
    else
        return 0;
}

QVariant MSqlQueryModel::data(const QModelIndex &index, int role) const {
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        return m_recList.at(index.row()).value(index.column());
    }
    return QVariant();
}

QVariant MSqlQueryModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if(role == Qt::DisplayRole)
    {
        if(orientation == Qt::Horizontal){
            if(!m_recList.isEmpty())
            {
                return m_recList.first().fieldName(section);
            }
        }
        if(orientation == Qt::Vertical){
            return QString::number(section);
        }
    }
    return QVariant();
}

void MSqlQueryModel::HandleQuery(MSqlQuery *query){
    if(m_query){
        if(m_query->isBusy()){
            //if there is an already scheduled query, delete it.
            if(m_nextMQueryToExecute) {
                delete m_nextMQueryToExecute;
                m_nextMQueryToExecute= Q_NULLPTR;
            }
            if(m_nextQueryToExecute.isNull()) {
                m_nextQueryToExecute.clear();
                m_nextQueryDbConnectionName.clear();
            }
            //schedule this query to be executed after the currently running query
            m_nextMQueryToExecute= query;
            return;
        } else {
            delete m_query;
            m_query= Q_NULLPTR;
        }
    }
    m_query= query;
    m_query->setParent(this);
    connect(m_query, SIGNAL(gotResults(bool)), this, SLOT(queryGotResults(bool)));
}

void MSqlQueryModel::HandleQueryFromStr(const QString &query, const QString &dbConnectionName){
    if(m_query){
        if(m_query->isBusy()){
            if(m_nextMQueryToExecute) {
                delete m_nextQueryToExecute;
                m_nextMQueryToExecute= Q_NULLPTR;
            }
            if(m_nextQueryToExecute.isNull()){
                m_nextQueryToExecute.clear();
                m_nextQueryDbConnectionName.clear();
            }
            //schedule this query to be executed after the currently running query
            m_nextQueryToExecute= query;
            m_nextQueryDbConnectionName= dbConnectionName;
            return;
        } else if(m_query->getDbConnectionName() != dbConnectionName) {
            delete m_query;
            m_query= Q_NULLPTR;
        }
    }
    if(!m_query)
        m_query= new MSqlQuery(this, MSqlDatabase::database(dbConnectionName));
    connect(m_query, SIGNAL(gotResults(bool)), this, SLOT(queryGotResults(bool)));
}

void MSqlQueryModel::setQuery(MSqlQuery* query){
    HandleQuery(query);
    m_query->exec();
}

void MSqlQueryModel::setQueryAsync(MSqlQuery* query){
    HandleQuery(query);
    m_query->execAsync();
}

void MSqlQueryModel::setQuery(const QString &query, const QString &dbConnectionName){
    HandleQueryFromStr(query, dbConnectionName);
    m_query->exec(query);
}

void MSqlQueryModel::setQueryAsync(const QString &query, const QString &dbConnectionName){
    HandleQueryFromStr(query, dbConnectionName);
    m_query->execAsync(query);
}

void MSqlQueryModel::queryGotResults(bool success){
    if(success) {
        //copy results to the internal m_recList
        beginResetModel();
        m_recList.clear();
        while(m_query->next()) {
            m_recList.append(m_query->record());
        }
        endResetModel();
    } else {
        qCritical("MSqlQueryModel::queryGotResults success is false");
    }
    //schedule executing the nextQueryToExecute
    if(m_nextMQueryToExecute) {
        QMetaObject::invokeMethod(this, "setQueryAsync",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, m_nextMQueryToExecute));
        m_nextMQueryToExecute= Q_NULLPTR;
    }
    if(!m_nextQueryToExecute.isNull()) {
        QMetaObject::invokeMethod(this, "setQueryAsync",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, m_nextQueryToExecute),
                                  Q_ARG(QString, m_nextQueryDbConnectionName));
        m_nextQueryToExecute.clear();
        m_nextQueryDbConnectionName.clear();
    }
}

bool MSqlQueryModel::isBusy()const{
    return m_query->isBusy();
}
