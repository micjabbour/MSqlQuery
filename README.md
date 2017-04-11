# Note:

This project is being rewritten from scratch (in the develop branch) to address some design issues:

- [ ] get rid of the MDbWorker class, and use lambda functions instead of its slots.
- [ ] get rid of the singleton db thread, and use a separate thread for each database connection.
- [ ] use std::future or QFuture to properly encapsulate asynchronous operations.
- [ ] write documentation and a better readme.

----------------------------------------------------------------------------------------------------------------

# MSqlQuery
An Asynchronous Interface To Use Functions in The Qt5 SQL Module


+ Just Include The .pri file in your project, and use my classes (starting with letter M instead of Q), they provide a similar interface to their
  equivalent classes in qt, in addition to asyncExec() function , which executes a query asynchronously and emits a signal when done

+ I Also have a class MSqlQueryModel , that provides an async , calling setQuery() on it returns immediately, and the model is reset later when the
  the query actually returns
  
+ You can take a look at the .pro project for a quick demo on how to use the classes in this project

+ Support For The notifications system is there , and is widely tested.

+ The Whole Thing Works By Moving all database operations, to a single thread behind the scenes. Only one thread is used because a database connection can
  be used from the thread that creates it only (a limitation in the Qt5 SQL module)
  
+ As A consequence to the previous point , you can't use the Qt Classes (QSqlQuery, QSqlDatabase, QSqlQueryModel, ..) directly when using my classes. Because 
  they will try to access the database connection from A Thread Other the one that created it.
  
+ That is why I provide many synchronous functions , that behave exactly the same as the qt equivalents (as they just call them in the database thread behind the
  scenes).

+ Better Readme coming soon, feel free to contact me ( micjabbour@gmail.com ) for more information