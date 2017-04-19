#ifndef QTHREADUTILS_H
#define QTHREADUTILS_H

#include <QObject>
#include <QThread>

//FunctorTraits is used to get the return type of a lambda expression
//see http://stackoverflow.com/a/7943765
template <typename T>
struct FunctorTraits : public FunctorTraits<decltype(&T::operator())> {};
template <typename ClassType, typename ReturnType, typename... Args>
struct FunctorTraits< ReturnType (ClassType::*)(Args...) const> {
    typedef ReturnType return_t;
};

//The function queues a functor to get executed in a specified worker's thread
//the connectionType argument determines if the function needs to wait for the functor to finish (By default it does NOT)
template <typename Func>
void PostToWorker(QObject* worker, Func&& f, Qt::ConnectionType connectionType = Qt::QueuedConnection) {
    //see http://stackoverflow.com/a/21653558
    QObject temporaryObject;
    QObject::connect(&temporaryObject, &QObject::destroyed,
                     worker, std::forward<Func>(f), connectionType);
}

//The function executes a functor in a specified worker's thread
//it waits for the functor to finish, and returns its result to the caller in the current thread
template <typename Func> //for functors returning non-void
typename std::enable_if<!std::is_void<typename FunctorTraits<Func>::return_t>::value, typename FunctorTraits<Func>::return_t>::type
CallByWorker(QObject* worker, Func&& f) {
    Qt::ConnectionType blockingConnectionType = QThread::currentThread() == worker->thread() ?
                Qt::DirectConnection : Qt::BlockingQueuedConnection;
    typename FunctorTraits<Func>::return_t returnValue;
    auto myFunctor = [&]{
        returnValue= std::forward<Func>(f)();
    };
    PostToWorker(worker, myFunctor, blockingConnectionType);
    return returnValue;
}
//if the functor returns void, no need to use a custom functor like above
template <typename Func> //for functors returning void
typename std::enable_if<std::is_void<typename FunctorTraits<Func>::return_t>::value, void>::type
CallByWorker(QObject* worker, Func&& f) {
    Qt::ConnectionType blockingConnectionType = QThread::currentThread() == worker->thread() ?
                Qt::DirectConnection : Qt::BlockingQueuedConnection;
    PostToWorker(worker, std::forward<Func>(f), blockingConnectionType);
}

//overload for methods/slots
//the slot gets invoked in the thread where the QObject lives
template <typename Object, typename T>
void InvokeLater(Object* object, T (Object::* f)()){
    QObject signalSource;
    QObject::connect(&signalSource, &QObject::destroyed,
                     object, f, Qt::QueuedConnection);
}
#endif // QTHREADUTILS_H
