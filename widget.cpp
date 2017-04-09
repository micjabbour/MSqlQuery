#include "widget.h"
#include "ui_widget.h"
#include "msqlquery.h"
#include "msqlquerymodel.h"
#include <QMessageBox>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //if you want to use an async query, you should make sure that 
    //the MSqlQuery or MSqlQueryModel are allocated in manner that preserves
    //their lifetime while a query is running
    //(so that they can emit a signal when the query executes)
    mquery= new MSqlQuery(this);
    connect(mquery, SIGNAL(gotResults(bool)), this, SLOT(GotResult()));
    model = new MSqlQueryModel(this);
    ui->tableView->setModel(model);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::GotResult()
{
    if(!mquery->isSuccess())
    {
        qCritical("mquery failed");
        return;
    }
    int i=0;
    while(mquery->next())
    {
        qDebug("%d %s", i, mquery->record().value(0).toString().toLocal8Bit().data());
        i++;
    }
}

void Widget::on_pbSetModelQuery_clicked()
{
    model->setQueryAsync(ui->lineEdit_2->text());
}

void Widget::on_pbAsyncExecQuery_clicked()
{
    if(!mquery->execAsync(ui->lineEdit->text()))
    {
        qCritical("execAsync returned false");
    }
}

void Widget::on_pbExecQuery_clicked()
{
    /*if(!mquery->execAsync(ui->lineEdit->text()))
    {
        qCritical("execAsync returned false");
    }*/
    MSqlQuery mquery;
    if(!mquery.exec(ui->lineEdit->text()))
    {
        qCritical("exec returned false!!");
        return;
    }
    int i=0;
    while(mquery.next())
    {
        qDebug("%d %s", i, mquery.record().value(0).toString().toLocal8Bit().data());
        i++;
    }
}
