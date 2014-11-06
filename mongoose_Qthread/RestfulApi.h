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

static const char *s_login_uri = "/login.html";
static const char *s_error_uri = "/error.html";
static const char *s_authenticated_uri = "/apps/start/index.html";
static const char *s_restful_uri ="/restful/"; // hit a restful call
static  char *s_secret =NULL;  // This value changes when the user logins

class RestfulApi : public QThread
 {
     Q_OBJECT



 private:
 void run();

 static int check_login_form_submission(struct mg_connection *conn);
 struct mg_server *server;
 static void ApiWrap_Mult(struct mg_connection *conn);
 static void ApiWrap_OT(struct mg_connection *conn,int type);
 static int ev_handler(struct mg_connection *conn, enum mg_event ev);
 static int serve_request(struct mg_connection *conn);
 static int handle_api_requests(struct mg_connection *conn);


 private slots:
  void sl_quit();


};


#endif // RESTFULAPI_H
