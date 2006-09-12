#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <postgresql/libpq-fe.h>
#include "image_data.h"
#include "image_common.h"
#include "../common.h"
#include "../definitions.h"

#define BUFFSIZE 512
#define BUFFSIZE_EXTRA 2048


typedef struct pix_list *pix_list_ptr;
typedef struct pix_list {
  pix_list_ptr 	next;
  int 		x_pix_coord;
  int 		value_count;
  int 		value_sum;
} pix_list_t;



static char *get_conn_string();
static PGconn *pg_check_connect(char *);
static PGresult *pg_check_exec(PGconn *, char *);
static char *get_type_table_by_id(PGconn *, int );






pix_list_ptr get_pix_list(int c_width){
  double seconds_per_pix = (img_cfg.show_interval * 0.1)/(c_width * 0.1);
  PGconn *connection = pg_check_connect(get_conn_string());

}

/* baut den String fuer die Postgres-Verbindung zusammen */
static char *get_conn_string(){
    char *conn_string = malloc(sizeof(char)*BUFFSIZE);
    snprintf(conn_string, BUFFSIZE, "host=%s dbname=%s user=%s password=%s connect_timeout=%s", global_opts.pg_host, global_opts.pg_database, global_opts.pg_user, global_opts.pg_pass, global_opts.pg_timeout);
    return conn_string;
}


/* Baut eine Vebindung zur postgres auf, bricht mit fehler ab wenn nicht moeglich */
static PGconn *pg_check_connect(char *conn_string){
  PGconn *conn = PQconnectdb(conn_string);			/* Connection aufbauen */
  if(PQstatus(conn) != CONNECTION_OK){
    DEBUGOUT2("\nFehler beim Aufbau der Datenbankverbindung\n%s\n", PQerrorMessage(conn));
    exit_error(ERROR_MAKECONN);
  }
  DEBUGOUT1("Verbindung hergestellt \n");
  return conn;
}

/* Fuehrt ein SQL-Statement aus. Bricht mit fehler ab wenn nicht moeglich */
static PGresult *pg_check_exec(PGconn *conn, char *query){
  PGresult *res;
  res = PQexec(conn, query);
  if(!res || PQresultStatus(res) != PGRES_TUPLES_OK){
    DEBUGOUT2("Fehler beim exec: %s\n", query);
    exit_error(ERROR_QUERY);
  } else {
    DEBUGOUT2("Query: '%s' ausgeführt\n", query);
  }
  return res;
}


/* Tabellenname des typs aus der Datenbank holen */
static char *get_type_table_by_id(PGconn *connection, int sens_id){
  char *table;
  int table_field;
  PGresult *table_res;
  char *query_buff 	= malloc(sizeof(char)*BUFFSIZE);

  snprintf(query_buff, BUFFSIZE, "select typen.tabelle as tbl FROM typen, sensoren WHERE sensoren.id=%d AND typen.typ=sensoren.typ", sens_id);
  table_res = pg_check_exec(connection, query_buff);
  
  if(PQntuples(table_res) < 1)
    return NULL;
  
  table_field = PQfnumber(table_res, "tbl");
  table       = strdup(PQgetvalue(table_res, 0, table_field));
  
  DEBUGOUT2("\tTabelle: %s \n", table);

  PQclear(table_res);
  free(query_buff);

  return table;
}
