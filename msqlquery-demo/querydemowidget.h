#ifndef QUERYDEMOWIDGET_H
#define QUERYDEMOWIDGET_H

#include <QWidget>
#include <QTime>

namespace Ui {
class QueryDemoWidget;
}

class MSqlQuery;

class QueryDemoWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit QueryDemoWidget(QWidget *parent = 0);
    ~QueryDemoWidget();
    
private:
    //connected to GUI buttons
    Q_SLOT void on_pbExecAsync_clicked();
    Q_SLOT void on_pbExec_clicked();
    
    //connected to resultsReady signal from MSqlQuery
    Q_SLOT void showResults(bool success);
    
    Ui::QueryDemoWidget *ui;
    MSqlQuery* m_query;
    QTime m_stopWatch; //used to calculate query time completion
};

#endif // QUERYDEMOWIDGET_H
