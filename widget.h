#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class MSqlQuery;
class MSqlQueryModel;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    
public slots:
    void GotResult();
private slots:
    void on_pbSetModelQuery_clicked();
    
    void on_pbAsyncExecQuery_clicked();
    
    void on_pbExecQuery_clicked();
    
private:
    Ui::Widget *ui;
    MSqlQuery* mquery;
    MSqlQueryModel* model;
};

#endif // WIDGET_H
