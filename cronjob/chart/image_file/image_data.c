#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <postgresql/libpq-fe.h>
#include "image_data.h"
#include "image_common.h"
#include "../common.h"
#include "../definitions.h"

#define BUFFSIZE 512




static pix_list_ptr add_pix_value(pix_list_ptr , int , int );
static char *get_conn_string();
static PGconn *pg_check_connect(char *);
static PGresult *pg_check_exec(PGconn *, char *);
static char *get_type_table_by_id(PGconn *, int );





/* Holt eine Liste mit den Werten und den jeweiligen x-koordinaten */
pix_list_ptr get_pix_list(int c_width){
  double seconds_per_pix = ((double)c_width)/((double)img_cfg.show_interval);		/* Pixel pro Sekunde */
  char *conn_string 	 = get_conn_string();						/* Verbindungs-String */
  PGconn *conn 		 = pg_check_connect(conn_string);				/* Datenbank - Verbindung */
  char *table 		 = get_type_table_by_id(conn, img_cfg.sens_id);			/* Tabellen - Name */
  char *query		 = malloc(sizeof(char)*BUFFSIZE);				/* Query */
  PGresult *res		 = NULL;							/* Darenbank - Result */
  int time_field;									/* Id ses Timestamp-Feldes */
  int val_field;									/* Id des Wert - Feldes */
  long time_temp;									/* Hilfsvariable */
  int pix_coord;									/* x - Koordinate, an die der Wert gehört */
  int i;										/* Laufvariable zum durchlaufen des Datenbank-resuls */
  long base_time;									/* Zeit an der 0-Koordinate (lt. Datenbank!) */
  pix_list_ptr list_ptr = NULL;								/* Zeiger auf den Anfang der Wertliste */
  pix_list_ptr temp_ptr = NULL;								/* Zeiger zum durchlaufen der Wertliste */

  DEBUGOUT1("\nHole Daten...\n");
  DEBUGOUT2("  Ein Pixel entspricht %f sekunden\n", seconds_per_pix);

  snprintf(query, BUFFSIZE, "SELECT round(date_part('epoch', current_timestamp)) AS now, round(date_part('epoch', timestamp)) AS times, %s AS val FROM %s WHERE  timestamp > (current_timestamp - INTERVAL '%d seconds') ORDER BY times ASC", img_cfg.table_field, table, img_cfg.show_interval );

  res = pg_check_exec(conn, query);

  time_field = PQfnumber(res, "times");
  val_field  = PQfnumber(res, "val");

  base_time = atol(PQgetvalue(res, 0, PQfnumber(res, "now"))) - img_cfg.show_interval;

  for (i = 0; i < PQntuples(res); i++){
    time_temp = atol(PQgetvalue(res, i, time_field)) - base_time;
    pix_coord = floor( ((double)time_temp) * seconds_per_pix) ;
    temp_ptr = add_pix_value(temp_ptr, pix_coord, atoi( PQgetvalue(res, i, val_field) ) );

    if(list_ptr == NULL){
      list_ptr = temp_ptr;
    }

  }

  PQclear(res);
  PQfinish(conn);
  free(query);
  free(table);
  free(conn_string);

  return list_ptr;
}

/* Speichert einen geholten Wert ab */
static pix_list_ptr add_pix_value(pix_list_ptr ptr, int coord, int value){ 

  if(ptr == NULL){
    DEBUGOUT1("\nLese Daten ein:\n");
    ptr  		= malloc(sizeof(pix_list_t));
    ptr->next 		= NULL;
    ptr->x_pix_coord 	= 0;
    ptr->value_count    = 0;
    ptr->value_sum	= 0;
    DEBUGOUT1("  Erstes Element generiert...\n");
  }

  if(coord == ptr->x_pix_coord){
    ptr->value_sum += value;
    ptr->value_count++;

    DEBUGOUT5("  Zu x-pos. %d %d. Wert (%d) hinzugefügt. Durchschn.: %d\n", ptr->x_pix_coord, ptr->value_count, value, (ptr->value_sum/ptr->value_count) );
  } else {
    ptr->next 		= malloc(sizeof(pix_list_t));
    ptr 		= ptr->next;
    ptr->x_pix_coord	= coord;
    ptr->value_sum 	= value;
    ptr->value_count 	= 1;
    ptr->next           = NULL;

    DEBUGOUT3("  An x-pos. %d Wert %d eingefuegt\n", ptr->x_pix_coord, ptr->value_sum);
  }

  return ptr;

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
