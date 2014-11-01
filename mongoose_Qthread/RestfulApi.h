#ifndef RESTFULAPI_H
#define RESTFULAPI_H
/* Restful Json Api

  Restfull api is designed to embedd a "mongoose http server" that serves Restful webservices.
  the whole servers runs parallel in a gui separate thread.

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <QTCore>
#include "mongoose.h"

//aditional server headers
static const char *s_no_cache_header =
  "Cache-Control: max-age=0, post-check=0, "
  "pre-check=0, no-store, no-cache, must-revalidate\r\n";
class RestfulApi : public QThread
 {
     Q_OBJECT

struct mg_server *server;
static void handle_restful_call(struct mg_connection *conn);
static void handle_restful_ot (struct mg_connection *conn,int type);
static int ev_handler(struct mg_connection *conn, enum mg_event ev);

 private:
 void run();

 private slots:
  void sl_quit();


};


#endif // RESTFULAPI_H
