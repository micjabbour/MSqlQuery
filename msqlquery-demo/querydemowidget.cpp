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
        ui->teResults->append("First 10 records: \n");
        //MSqlQuery provides an interface similar to that of QSqlQuery
        int rowCount = 0;
        while(m_query->next() && rowCount++<10) { //loop to display first 10 records
            QString recordStr;
            for(int i=0; i<m_query->record().count(); i++)
                recordStr+= m_query->record().value(i).toString()+ "\t";
            recordStr.append("\n");
            ui->teResults->append(recordStr);
        }
        ui->teResults->append(QString("\nNumber of rows affected: %0").arg(m_query->numRowsAffected()));
    } else {
        ui->teResults->setTextColor(QColor(Qt::red));
        ui->teResults->append(QString("Got error in %0 msecs.\n\n").arg(m_stopWatch.elapsed()));
        ui->teResults->append("Error: \n");
        ui->teResults->append(QString("driver text: %0\n\n").arg(m_query->lastError().driverText()));
        ui->teResults->append(QString("database text: %0\n\n").arg(m_query->lastError().databaseText()));
    }
}
