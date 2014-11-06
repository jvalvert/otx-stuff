#include "RestfulApi.h"

//======================================THREAD MANAGEMENT AND SERVER STARTUP===========================================
//protected method that start the mongoose server in a separate thread



void RestfulApi::run()
{
      qDebug()<<"Webserver API running in a process thread: " << currentThreadId();
      // Create and configure the server
     server = mg_create_server(NULL, ev_handler);
      mg_set_option(server, "listening_port", "38000");
      mg_set_option(server, "document_root", ".");

      qDebug() << "Society Pro Web API. by Central Services. Listening at port 38000...";

      for (;;)  {
        mg_poll_server(server, 1000);
      }
}

void RestfulApi::sl_quit()
{

    qDebug()<<"Closing Webserver API with thread: "<< QThread::currentThreadId();
    // Cleanup, and free server instance
    mg_destroy_server(&server);
   // close the thread
    terminate();
}




//================================WEBSERVICES HANDLING===========================

void RestfulApi::ApiWrap_Mult(struct mg_connection *conn) {
  char n1[100], n2[100];

  // Get form variables
  mg_get_var(conn, "n1", n1, sizeof(n1));
  mg_get_var(conn, "n2", n2, sizeof(n2));

  mg_printf_data(conn, "{ \"result\": %lf }", strtod(n1, NULL) * strtod(n2, NULL));
}

 void RestfulApi::ApiWrap_OT(struct mg_connection *conn,int type) {

 char role[100];
 role[0]='\0';

 //Get Role name
 mg_get_var(conn, "role", role, sizeof(role));

 if (strcmp(role,"Jorge")==0) //hit Jorge role for this example
   {
    if (type==1) // ask for nym
     //mg_printf_data(conn, "{ \"nymid\": }",role);
    mg_printf_data(conn, "{ \"result\": %s }", "\"quNQE0foapIxUtjPKnFA93W0pqUJCjhCIA2hlZapvNF\"" );
    else
    if (type==2)
      mg_printf_data(conn, "{ \"result\": %s }", "\"- -----BEGIN PUBLIC KEY-----CpACLS0tLS1CRUdJTiBQVUJMSUMgS0VZLS0tLS0KTUlHZk1BMEdDU3FHU0liM0RR- -----END PUBLIC KEY-----\"");
   }
  else
    mg_printf_data(conn, "{ \"result\": %s }", "\"The role does not exist\"");

}
int  RestfulApi::handle_api_requests(struct mg_connection *conn)
{

    std::string restUri(conn->uri);
    if (restUri.compare("/restful/mult")==0) {
    ApiWrap_Mult(conn);

      return MG_TRUE;
    }
    // get  nym id
    if (restUri.compare("/restful/getnym")==0) {
      ApiWrap_OT(conn,1);
      return MG_TRUE;
    }
    // get pubkey
    if (restUri.compare("/restful/getpubkey")==0) {
      ApiWrap_OT(conn,2);

      return MG_TRUE;
    }

    //the request is wrong go to the home page
     mg_send_file(conn, s_authenticated_uri, s_no_cache_header);
    return MG_MORE;


}


//============================================HANDLE WEB REQUESTS======================================================


//======================SESSION AND API KEY=====================================



 static void generate_ssid(const char *user_name, const char *expiration_date,
                          char *ssid, size_t ssid_size) {
  char hash[33];
  mg_md5(hash, user_name, ":", expiration_date, ":", s_secret, NULL);
  snprintf(ssid, ssid_size, "%s|%s|%s", user_name, expiration_date, hash);
}



// *************  CHECK AUTHENTICATION EVENT: Check if the session cookie is created, if not go to login page or show error
static int check_auth(struct mg_connection *conn) {

  char ssid[100], calculated_ssid[100], name[100], expire[100];
  // Always authenticate requests to login page

  if (strcmp(conn->uri, s_login_uri) == 0 ||strcmp(conn->uri, s_error_uri) == 0) {

      return MG_TRUE;
  }

  // Look for session ID in the Cookie.
  // That session ID can be validated against the database that stores
  // current active sessions.
   mg_parse_header(mg_get_header(conn, "Cookie"), "ssid", ssid, sizeof(ssid));
  if (sscanf(ssid, "%[^|]|%[^|]|", name, expire) == 2) {
      generate_ssid(name, expire, calculated_ssid, sizeof(calculated_ssid));
    if (strcmp(ssid, calculated_ssid) == 0) {
        //fprintf(stderr,"\n SSid generated: ssid %s ,calculated ssid %s , name %s,expire %s ... \n",ssid,calculated_ssid,name,expire);
        return MG_TRUE;  // Authenticate
    }
  }

  // Auth failed, do NOT authenticate, redirect to login page

  mg_printf(conn, "HTTP/1.1 302 Moved\r\nLocation: %s\r\n\r\n", s_error_uri);


 return MG_FALSE;
}



//TO DO: BASIC AUTHENTICATION Right now for demo purposses a hardcoded user is validated

// this function perform the call to the users and passwords handler (TO DO)
bool AuthenticateUser (char * name, char * password)
{
   // for POC pruposes use sopro as user and password
   if (strcmp(name, "sopro") == 0 && strcmp(password, "sopro") == 0)
   {  s_secret=password;
       return true;}
   else
   {return false;}
}
int RestfulApi::check_login_form_submission(struct mg_connection *conn)
  {

    char name[100], password[100], ssid[100], expire[100], expire_epoch[100];

     mg_get_var(conn, "name", name, sizeof(name));
     mg_get_var(conn, "password", password, sizeof(password));

     //To do: routine that validate users (maybe passprhase?)
     if (AuthenticateUser(name,password)){
       // Generate expiry date
       time_t t = time(NULL) + 3600;  // Valid for 1 hour
       snprintf(expire_epoch, sizeof(expire_epoch), "%lu", (unsigned long) t);
       strftime(expire, sizeof(expire), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&t));

       generate_ssid(name, expire_epoch, ssid, sizeof(ssid));
       // Set "session id" cookie, there could be some data encoded in it.
       mg_printf(conn,
                 "HTTP/1.1 302 Moved\r\n"
                 "Set-Cookie: ssid=%s; expire=\"%s\"; http-only; HttpOnly;\r\n"
                 "Location: /\r\n\r\n",
                 ssid, expire);
      fprintf(stderr,"\nAutenticated successfully\n");

      return MG_FALSE;
     }
      mg_printf(conn, "HTTP/1.1 302 Moved\r\nLocation: %s\r\n\r\n", s_error_uri);
     return MG_FALSE;
   }


//=================HTML REQUESTS ENTRY POINT=====================

// handle events

int RestfulApi::ev_handler(struct mg_connection *conn, enum mg_event ev) {

  switch (ev) {
   case MG_AUTH:
      return check_auth(conn);

  case MG_REQUEST:

    return serve_request(conn);

  default: return MG_FALSE;
 }
}


// manage the request
int RestfulApi::serve_request(struct mg_connection *conn) {
    fprintf(stderr,"\nUri: %s\n",conn->uri);
    fprintf(stderr,"\nserver requests at  %s\n",conn->uri);

    std::string uri (conn->uri);

    if ( uri.compare(s_login_uri) == 0 &&
         strcmp(conn->request_method, "POST") == 0)
         {
          fprintf(stderr,"\nPost received send the form to check user and password. %s \n",conn->uri);
         return check_login_form_submission(conn);
         }
        else
        {  //check if is a restful request
            if (uri.find(s_restful_uri) !=std::string::npos) // check if its a restfull call
            {
                fprintf(stderr,"\nHit Restful Request at uri %s",conn->uri);
                return handle_api_requests(conn);

            }


      }

  // mg_send_file(conn,"test/index.html", s_no_cache_header);
  return MG_FALSE;  // Serve files in the document_root
}
