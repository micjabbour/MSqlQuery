#ifndef MSQLTHREAD_H
#define MSQLTHREAD_H

#include <QObject>
#include <QThread>

//a thread that can be destroyed at any time
//see http://stackoverflow.com/a/25230470
class SafeThread : public QThread{
    using QThread::run;
public:
    SafeThread(QObject* parent= nullptr):QThread(parent){}
    ~SafeThread(){ quit(); wait(); }
};



class MSqlThread : public SafeThread
{
public:
    explicit MSqlThread(QObject *parent = nullptr);
    ~MSqlThread() {}

    QObject* getWorker(){ return m_worker; }
private:
    QObject* m_worker;
};

#endif // MSQLTHREAD_H
