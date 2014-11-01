#include "RestfulApi.h"


//protected method that start the mongoose server in a separate thread
void RestfulApi::run()
{
      qDebug()<<"Webserver API running in a process thread: " << currentThreadId();
      // Create and configure the server
      server = mg_create_server(NULL, ev_handler);
      mg_set_option(server, "listening_port", "38000");

      qDebug() << "Society Pro Web API. by Central Services. Listening at port 38000...";

      while(true) {
        mg_poll_server(server, 1000);
      }


}

void RestfulApi::sl_quit()
{
    // Cleanup, and free server instance
    mg_destroy_server(&server);
}

 void RestfulApi::handle_restful_call(struct mg_connection *conn) {
  char n1[100], n2[100];

  // Get form variables
  mg_get_var(conn, "n1", n1, sizeof(n1));
  mg_get_var(conn, "n2", n2, sizeof(n2));

  mg_printf_data(conn, "{ \"result\": %lf }", strtod(n1, NULL) * strtod(n2, NULL));
}

 void RestfulApi::handle_restful_ot (struct mg_connection *conn,int type) {
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

 int RestfulApi::ev_handler(struct mg_connection *conn, enum mg_event ev) {


    switch (ev) {
    case MG_AUTH: return MG_TRUE;
    case MG_REQUEST:
      if (!strcmp(conn->uri, "/api/mult")) {
        handle_restful_call(conn);

        return MG_TRUE;
      }
      // get  nym id
      if (!strcmp(conn->uri, "/api/getnym")) {
        handle_restful_ot(conn,1);
        return MG_TRUE;
      }
      // get pubkey
      if (!strcmp(conn->uri, "/api/getpubkey")) {
        handle_restful_ot(conn,2);

        return MG_TRUE;
      }


      mg_send_file(conn, "index.html", s_no_cache_header);
      return MG_MORE;
    default: return MG_FALSE;
  }
}
