#include <QMessageBox>
#include <QApplication>
#include "msqldatabase.h"
#include "msqlquery.h"
#include "widget.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    //Setup database application as you would do with QSqlDatabase
    //Just using MSqlDatabase instead
    
    //Here I am using an in-memory sqlite database for demonstration purposes
    //You can use any connect to any Sql Server supported by QSqlDatabase
    /*
    MSqlDatabase mdb = MSqlDatabase::addDatabase("QSQLITE");
    mdb.setDatabaseName(":memory:");
    if (!mdb.open()) {
        QMessageBox::critical(0, qApp->tr("Cannot open database"),
            qApp->tr("Unable to establish a database connection.\n"
                     "This example needs SQLite support. Please read "
                     "the Qt SQL driver documentation for information how "
                     "to build it.\n\n"
                     "Click Cancel to exit."), QMessageBox::Cancel);
        return 1;
    }
    
    //setting up a test database table with some dummy data
    
    MSqlQuery query;
    query.exec("create table person (id int primary key, "
               "firstname varchar(20), lastname varchar(20))");
    query.exec("insert into person values(101, 'Danny', 'Young')");
    query.exec("insert into person values(102, 'Christine', 'Holand')");
    query.exec("insert into person values(103, 'Lars', 'Gordon')");
    query.exec("insert into person values(104, 'Roberto', 'Robitaille')");
    query.exec("insert into person values(105, 'Maria', 'Papadopoulos')");
    */
    MSqlDatabase mdb = MSqlDatabase::addDatabase("QPSQL");
    mdb.setHostName("192.168.5.5");
    mdb.setUserName("postgres");
    mdb.setPassword("123456");
    mdb.setDatabaseName("test");
    if(!mdb.open()){
        QMessageBox::critical(Q_NULLPTR, qApp->tr("Error"),
                              qApp->tr("Error opening database."),
                              QMessageBox::Cancel);
        return 1;
    }
    Widget w;
    w.show();
    return a.exec();
}
