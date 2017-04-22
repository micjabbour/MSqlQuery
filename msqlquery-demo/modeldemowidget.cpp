#include "modeldemowidget.h"
#include "ui_modeldemowidget.h"
#include "msqlquerymodel.h"

ModelDemoWidget::ModelDemoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModelDemoWidget) {
    ui->setupUi(this);
    m_model = new MSqlQueryModel(this);
    ui->tvResults->setModel(m_model);
}

ModelDemoWidget::~ModelDemoWidget() {
    delete ui;
}

void ModelDemoWidget::on_pbSetQueryAsync_clicked() {
    m_model->setQueryAsync(ui->leQuery->text());
}

void ModelDemoWidget::on_pbSetQuery_clicked() {
    m_model->setQuery(ui->leQuery->text());
}
