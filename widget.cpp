#include "widget.h"
#include "ui_widget.h"
#include "msqlquery.h"
#include "msqlquerymodel.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    MSqlDatabase mdb = MSqlDatabase::addDatabase("QPSQL");
    mdb.setHostName("192.168.5.5");
    mdb.setDatabaseName("PlaySyDb");
    mdb.setUserName("postgres");
    mdb.setPassword("123456");
    mdb.setConnectionOptions("requiressl=1");
    if(!mdb.open())
    {
        qCritical("no connection, disabling ssl. . .");
        mdb.setConnectionOptions();
        if(!mdb.open())
        {
            qCritical("no connection after disabling ssl");
        }
    }
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

void Widget::on_pushButton_clicked()
{
    /*if(!mquery->asyncExec(ui->lineEdit->text()))
    {
        qCritical("asyncExec returned false");
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

void Widget::on_pushButton_2_clicked()
{
    if(!mquery->asyncExec(ui->lineEdit->text()))
    {
            qCritical("asyncExec returned false");
    }
}

void Widget::on_pushButton_3_clicked()
{
    model->setQuery(ui->lineEdit_2->text());
}
