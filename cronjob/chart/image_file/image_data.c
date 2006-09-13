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



static pix_list_ptr min = NULL;
static pix_list_ptr max = NULL;


static pix_list_ptr add_pix_value(pix_list_ptr , long, int , int );
static char *get_conn_string();
static PGconn *pg_check_connect(char *);
static PGresult *pg_check_exec(PGconn *, char *);
static char *get_type_table_by_id(PGconn *, int );


/* Skaliert die X-Koordinaten der Punkte im angegebenem Bereich
 * ausführliche Beschreibung im header-file */
int scale_y_coords(pix_list_ptr ptr, int c_height, int max_label, int min_label){
  int range            = (max_label - min_label + 1) * 10;				/* Anzahl von 0,1-Schritten */
  double pix_per_scale = ((double)c_height) / ((double)range); 				/* Pixel pro 0,1 */
  pix_list_ptr temp    = ptr;
  int zero_line        = floor( ((double)( (max_label * 10) + 1)) * pix_per_scale);			/* Nullinie */
  
  DEBUGOUT1("\nBerechne y-Koordinaten:\n");

  for (; temp; temp = temp->next){
    temp->y_pix_coord = -1 * floor( ( ((double)temp->value_sum) / ((double)temp->value_count) ) * pix_per_scale);
    DEBUGOUT3("  neue y-Koordinate: %d bei x: %d\n",temp->y_pix_coord, temp->x_pix_coord);
  }

  DEBUGOUT2(" Nullinie bei: %d\n", zero_line);

  return zero_line;
}

/* Maximaler wert */
pix_list_ptr get_max(){
  return min;
}

/* Minimaler Wert */
pix_list_ptr get_min(){
  return max;
}

/* Holt eine Liste mit den Werten und den jeweiligen x-koordinaten */
pix_list_ptr get_pix_list(int c_width){
  double seconds_per_pix = ((double)c_width)/((double)img_cfg.show_interval);		/* Pixel pro Sekunde */
  char *conn_string 	 = get_conn_string();						/* Verbindungs-String */
  PGconn *conn 		 = pg_check_connect(conn_string);				/* Datenbank - Verbindung */
  char *table 		 = NULL;							/* Tabellen - Name */
  char *query		 = malloc(sizeof(char)*BUFFSIZE);				/* Query */
  PGresult *res		 = NULL;							/* Darenbank - Result */
  int time_field;									/* Id ses Timestamp-Feldes */
  int val_field;									/* Id des Wert - Feldes */
  long time_temp;									/* Hilfsvariable */
  int pix_coord;									/* x - Koordinate, an die der Wert gehört */
  int i;										/* Laufvariable zum durchlaufen des Datenbank-resuls */
  long base_time;									/* Zeit an der 0-Koordinate (lt. Datenbank!) */
  long timestamp;
  pix_list_ptr list_ptr = NULL;								/* Zeiger auf den Anfang der Wertliste */
  pix_list_ptr temp_ptr = NULL;								/* Zeiger zum durchlaufen der Wertliste */

  DEBUGOUT1("\nHole Daten...\n");
  DEBUGOUT2("  Ein Pixel entspricht %f sekunden\n", seconds_per_pix);

  if (img_cfg.manual_table) {
    table = strdup(img_cfg.table_name);
  } else {
    table = get_type_table_by_id(conn, img_cfg.sens_id);
  }

  snprintf(query, BUFFSIZE, "SELECT round(date_part('epoch', current_timestamp)) AS now, round(date_part('epoch', timestamp)) AS times, %s AS val FROM %s WHERE  timestamp > (current_timestamp - INTERVAL '%d seconds') ORDER BY times ASC", img_cfg.table_field, table, img_cfg.show_interval );

  res = pg_check_exec(conn, query);

  time_field = PQfnumber(res, "times");
  val_field  = PQfnumber(res, "val");

  base_time = atol(PQgetvalue(res, 0, PQfnumber(res, "now"))) - img_cfg.show_interval;

  for (i = 0; i < PQntuples(res); i++){
    timestamp = atol(PQgetvalue(res, i, time_field));
    time_temp = timestamp - base_time;
    pix_coord = floor( ((double)time_temp) * seconds_per_pix) ;
    temp_ptr = add_pix_value(temp_ptr, timestamp, pix_coord, atoi( PQgetvalue(res, i, val_field) ) );

    if (list_ptr == NULL){
      list_ptr = temp_ptr;
      
      /* Globale Variablen neu initialisieren um 
       * mehtete Bilder in einem Thread bauen zu
       * können */
      min = temp_ptr;
      max = temp_ptr;
    }

    /* Min / Max ermitteln */
    if (min != NULL){
      if ( (temp_ptr->value_sum / temp_ptr->value_count) < (min->value_sum / min->value_count) )
        min = temp_ptr;
    } else {
      min = temp_ptr;
    }

    if (max != NULL){
      if ( (temp_ptr->value_sum / temp_ptr->value_count) > (max->value_sum / max->value_count) )
        max = temp_ptr;
    } else {
      max = temp_ptr;
    }
  }
  
  DEBUGOUT2(" %d Werte geholt \n", i);
  DEBUGOUT3(" Min: x-pos.: %d, Wert: %d\n", min->x_pix_coord, (min->value_sum / min->value_count) );
  DEBUGOUT3(" Max: x-pos.: %d, Wert: %d\n", max->x_pix_coord, (max->value_sum / max->value_count) );
  PQclear(res);
  PQfinish(conn);
  free(query);
  free(table);
  free(conn_string);

  return list_ptr;
}

/* Speichert einen geholten Wert ab */
static pix_list_ptr add_pix_value(pix_list_ptr ptr, long timestamp, int coord, int value){ 

  if(ptr == NULL){
    DEBUGOUT1("\nLese Daten ein:\n");
    ptr  		= malloc(sizeof(pix_list_t));
    ptr->next 		= NULL;
    ptr->timestamp	= timestamp;
    ptr->x_pix_coord 	= coord;
    ptr->y_pix_coord 	= 0;
    ptr->value_count    = 1;
    ptr->value_sum	= value;
    DEBUGOUT3("  Erstes Element generiert...x-pos.: %d Wert: %d\n",ptr->x_pix_coord, ptr->value_sum);
  } else {

    if(coord == ptr->x_pix_coord){
      ptr->value_sum += value;
      ptr->value_count++;

      DEBUGOUT5("  Zu x-pos. %d %d. Wert (%d) hinzugefügt. Durchschn.: %d\n", ptr->x_pix_coord, ptr->value_count, value, (ptr->value_sum/ptr->value_count) );
    } else {
      ptr->next		= malloc(sizeof(pix_list_t));
      ptr 		= ptr->next;
      ptr->timestamp	= timestamp;
      ptr->x_pix_coord	= coord;
      ptr->y_pix_coord	= 0;
      ptr->value_sum 	= value;
      ptr->value_count 	= 1;
      ptr->next           = NULL;

      DEBUGOUT3("  An x-pos. %d Wert %d eingefuegt\n", ptr->x_pix_coord, ptr->value_sum);
    }
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
