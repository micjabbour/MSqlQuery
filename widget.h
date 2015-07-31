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
    void on_pushButton_clicked();
    
    void on_pushButton_2_clicked();
    
    void on_pushButton_3_clicked();
    
private:
    Ui::Widget *ui;
    MSqlQuery* mquery;
    MSqlQueryModel* model;
};

#endif // WIDGET_H
