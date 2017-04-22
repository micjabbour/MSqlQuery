#include <QMessageBox>
#include <QApplication>
#include "msqldatabase.h"
#include "msqlquery.h"
#include "modeldemowidget.h"
#include "querydemowidget.h"
#include "loadingdialog.h"
#include <QTabWidget>


QString getRandomString() //to generate random strings (for dummy data)
{
   const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
   const int randomStringLength = 10; //random strings of 10 characters

   QString randomString;
   for(int i=0; i<randomStringLength; ++i)
   {
       int index = qrand() % possibleCharacters.length();
       QChar nextChar = possibleCharacters.at(index);
       randomString.append(nextChar);
   }
   return randomString;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    //Setup database application as you would do with QSqlDatabase
    //(using MSqlDatabase instead)
    
    //Here I am using an in-memory sqlite database for demonstration purposes
    //You can use any connect to any Sql Server supported by QSqlDatabase
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
    //set random sequence seed
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
    //create demo table and fill with dummy data
    MSqlQuery query;
    if(!query.exec("create table people (id integer primary key, "
               "firstname varchar(20), lastname varchar(20))"))
        return false;
    query.prepare("insert into people(firstname, lastname) values(?, ?);");
    QVariantList firstNames;
    QVariantList lastNames;
    for(int i=0; i<100000; i++) {
        firstNames << getRandomString();
        lastNames << getRandomString();
    }
    query.addBindValue(firstNames);
    query.addBindValue(lastNames);
    query.execBatchAsync();
    
    //show loading dialog
    LoadingDialog loadingDialog("Creating demo table, please wait. . .");
    QObject::connect(&query, &MSqlQuery::resultsReady, [&](bool success){
        if(success)
            loadingDialog.accept();
        else
            loadingDialog.reject();
    });
    
    if(!loadingDialog.exec()){
        QMessageBox::critical(0, qApp->tr("Error creating demo table"),
                              "Something went wrong while trying to create demo table.\n"
                              "Click Cancel to exit.", QMessageBox::Cancel);
        return 2;
    }
    
    QTabWidget tabWidget;
    tabWidget.addTab(new QueryDemoWidget, QStringLiteral("MSqlQuery demo"));
    tabWidget.addTab(new ModelDemoWidget, QStringLiteral("MSqlQueryModel demo"));
    tabWidget.show();
    
    
    return a.exec();
}
