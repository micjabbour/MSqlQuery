[![Build Status](https://travis-ci.org/micjabbour/MSqlQuery.svg?branch=master)](https://travis-ci.org/micjabbour/MSqlQuery)

# MSqlQuery

An asynchronous interface to use functions in the Qt5 SQL module

![demo screenshot](/screenshot.png?raw=true "demo screenshot")


+ Include The .pri file in your project, and use MSqlQuery classes (starting with letter M instead of Q), they provide a similar interface to their
  equivalent classes in Qt, in addition to execAsync() function , which executes a query asynchronously and emits a signal when done.

+ Also, there is an MSqlQueryModel class, that provides the same interface of QSqlQueryModel, in addition to setQueryAsync() which returns immediately,
  and the model is reset later when the query actually returns
  
+ You can take a look at the msqlquery-demo.pro project for a quick demo on how to use the classes in this project

+ Support for the notifications system 

+ The Whole Thing Works By Moving all database operations, to a single thread per connection. Only one thread per connection is used because a database connection can
  be used from the thread that creates it only (a limitation in the Qt5 SQL module)

+ As A consequence to the previous point , you can't use the Qt Classes (QSqlQuery, QSqlDatabase, QSqlQueryModel, ..) directly when using MSqlQuery classes on the same connection.
  because Qt classes will try to access the database connection from a Thread Other the one that created it.
  
+ That is why synchronous functions exist in MSqlQuery classes (they have the same names of the corresponding functions in Qt), they even behave exactly the same
  as their Qt equivalents (as they just call them in the database thread behind the scenes).
  
+ The asynchronous functions's names end with *Async(), eg.: execAsync(), setQueryAsync(), ...

+ Better Readme coming soon, feel free to contact me ( micjabbour@gmail.com ) for more information

----------------------------------------------------------------------------------------------------------------

# Note:

This project is being rewritten from scratch (in the develop branch) to address some design issues:

- [x] get rid of the MDbWorker class, and use lambda functions instead of its slots.
- [x] get rid of the singleton db thread, and use a separate thread for each database connection.
- [ ] use std::future or QFuture to properly encapsulate asynchronous operations.
- [ ] write documentation and a better readme.
- [x] add support for batch queries.