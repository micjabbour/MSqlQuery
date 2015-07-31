#include "msqlquerymodel.h"
#include "msqlquery.h"

MSqlQueryModel::MSqlQueryModel(QObject *parent) : QAbstractTableModel(parent), m_query(new MSqlQuery(this))
{
    connect(m_query, SIGNAL(gotResults(bool)), this, SLOT(queryGotResults(bool)));
}

MSqlQueryModel::~MSqlQueryModel()
{
    
}

int MSqlQueryModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid()) return 0;
    return m_recList.size();
}

int MSqlQueryModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid()) return 0;
    if(!m_recList.isEmpty())
        return m_recList.first().count();
    else
        return 0;
}

QVariant MSqlQueryModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        return m_recList.at(index.row()).value(index.column());
    }
    return QVariant();
}

QVariant MSqlQueryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
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

void MSqlQueryModel::setQuery(const QString &query, const QString &dbConnectionName)
{
    qDebug("setQuery here");
    if(m_query)
    {
        if(m_query->isBusy())
        {
            qDebug("my m_query was busy, so I am scheduling. . .");
            nextQueryToExecute = query;
            nextQueryDbConnectionName = dbConnectionName;
            return;
        }
        else
        {
            if(m_query->getDbConnectionName() != dbConnectionName)
            {
                delete m_query;
                m_query = NULL;
            }
        }
    }
    if(!m_query)
    {
        qDebug("setQuery creating a new MSqlQuery");
        m_query = new MSqlQuery(this, MSqlDatabase::database(dbConnectionName));
        qDebug("MSqlQuery object created");
        connect(m_query, SIGNAL(gotResults(bool)), this, SLOT(queryGotResults(bool)));
    }
    qDebug("setQuery: calling asyncExec");
    m_query->asyncExec(query);
    qDebug("setQuery: asyncExec returned");
}

void MSqlQueryModel::queryGotResults(bool success)
{
    if(success)
    {
        //copy results to the internal m_recList
        beginResetModel();
        m_recList.clear();
        while(m_query->next())
        {
            m_recList.append(m_query->record());
        }
        endResetModel();
    } else
    {
        qCritical("MSqlQueryModel::queryGotResults success is false");
    }
    //schedule executing the nextQueryToExecute
    if(!nextQueryToExecute.isEmpty())
    {
        QMetaObject::invokeMethod(this, "setQuery",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, nextQueryToExecute),
                                  Q_ARG(QString, nextQueryDbConnectionName));
        nextQueryToExecute.clear();
        nextQueryDbConnectionName.clear();
    }
}

bool MSqlQueryModel::isBusy()const
{
    return m_query->isBusy();
}
