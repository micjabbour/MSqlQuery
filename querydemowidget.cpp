#include "querydemowidget.h"
#include "ui_querydemowidget.h"
#include "msqlquery.h"

QueryDemoWidget::QueryDemoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QueryDemoWidget) {
    ui->setupUi(this);
    m_query = new MSqlQuery(this);
    connect(m_query, &MSqlQuery::resultsReady, this, &QueryDemoWidget::showResults);
}

QueryDemoWidget::~QueryDemoWidget() {
    delete ui;
}

void QueryDemoWidget::on_pbExecAsync_clicked() {
    ui->teResults->clear(); //clear old results text
    m_stopWatch.start(); //start stopWatch
    
    m_query->execAsync(ui->leQuery->text());
}

void QueryDemoWidget::on_pbExec_clicked() {
    ui->teResults->clear(); //clear old results text
    m_stopWatch.start();
    //calling exec does not emit the resultsReady signal
    bool success = m_query->exec(ui->leQuery->text());
    //call the slot manually
    showResults(success);
}

void QueryDemoWidget::showResults(bool success) {
    if(success) {
        ui->teResults->setTextColor(QColor(Qt::black));
        ui->teResults->append(QString("Got result in %0 msecs.\n\n").arg(m_stopWatch.elapsed()));
        ui->teResults->append("Records: \n");
        //MSqlQuery provides an interface similar to that of QSqlQuery
        while(m_query->next())
            //show values from the first column in the textedit
            ui->teResults->append(m_query->record().value(0).toString());
    } else {
        ui->teResults->setTextColor(QColor(Qt::red));
        ui->teResults->append(QString("Got error in %0 msecs.\n\n").arg(m_stopWatch.elapsed()));
        ui->teResults->append("Error: \n");
        ui->teResults->append(QString("driver text: %0\n\n").arg(m_query->lastError().driverText()));
        ui->teResults->append(QString("database text: %0\n\n").arg(m_query->lastError().databaseText()));
    }
}
