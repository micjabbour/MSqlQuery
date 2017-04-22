#ifndef MODELDEMOWIDGET_H
#define MODELDEMOWIDGET_H

#include <QWidget>

namespace Ui {
class ModelDemoWidget;
}

class MSqlQueryModel;

class ModelDemoWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ModelDemoWidget(QWidget *parent = 0);
    ~ModelDemoWidget();
    
private:
    //connected to GUI buttons
    Q_SLOT void on_pbSetQueryAsync_clicked();
    Q_SLOT void on_pbSetQuery_clicked();
    
    Ui::ModelDemoWidget *ui;
    MSqlQueryModel* m_model;
};

#endif // MODELDEMOWIDGET_H
