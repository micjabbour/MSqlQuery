#include "widget.h"
#include "ui_widget.h"
#include "msqlquery.h"
#include "msqlquerymodel.h"
#include <QMessageBox>
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //if you want to use an async query, you should make sure that 
    //the MSqlQuery or MSqlQueryModel are allocated in manner that preserves
    //their lifetime while a query is running
    //(so that they can emit a signal when the query executes)
    m_query= new MSqlQuery(this);
    connect(m_query, &MSqlQuery::resultsReady, this, [=](bool success){
        if(success) {
            int i=0;
            while(m_query->next()) {
                qDebug() << i << " " << m_query->record().value(0).toString();
                i++;
            }
        } else {
            qCritical("query failed");
        }
    });
    m_model = new MSqlQueryModel(this);
    ui->tableView->setModel(m_model);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pbSetModelQuery_clicked()
{
    m_model->setQueryAsync(ui->lineEdit_2->text());
}

void Widget::on_pbAsyncExecQuery_clicked()
{
    m_query->execAsync(ui->lineEdit->text());
}

void Widget::on_pbExecQuery_clicked()
{
    MSqlQuery query;
    if(!query.exec(ui->lineEdit->text()))
    {
        qCritical("exec returned false!!");
        return;
    }
    int i=0;
    while(query.next()) {
        qDebug() << i << " " << query.record().value(0).toString();
        i++;
    }
}
