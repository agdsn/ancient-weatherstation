/*

   checksensor.c        -- Part of the weatherdeamon

   Copyright (C) 2006 Jan Losinski

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   $Id: process.c v 1.00 11 Aug 2006 losinski $
*/


#include <stdlib.h>         	/* EXIT_SUCCESS */
#include <errno.h>
#include <unistd.h>         	/* STDOUT_FILENO */
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <postgresql/libpq-fe.h>
#include "definitions.h"
#include "config.h"
#include "checksensor.h"


#define BUFFSIZE 512


/* Variablen ---------------------------------------------------------- */
w_opts global_opts;
static PGconn *connection = NULL;
static char *conn_string = NULL;

/* Funktionen ----------------------------------------------------------*/

static void clean();
static void exit_sig_handler(int); 

/* Implementierungen ---------------------------------------------------*/

static void generate_conn_string(){
  if(conn_string == NULL){
    conn_string = malloc(sizeof(char)*BUFFSIZE);
    snprintf(conn_string, BUFFSIZE, "host=%s dbname=%s user=%s password=%s", global_opts.pg_host, global_opts.pg_database, global_opts.pg_user, global_opts.pg_pass);
  }
}

static PGconn *pg_check_connect(char *conn_string){
  PGconn *conn = PQconnectdb(conn_string);			/* Connection aufbauen */
  if(PQstatus(conn) != CONNECTION_OK){
    DEBUGOUT2("\nFehler beim Aufbau der Datenbankverbindung\n%s\n", PQerrorMessage(conn));
    exit_error(ERROR_MAKECONN);
  }
  DEBUGOUT1("Verbindung hergestellt \n");
  return conn;
}

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

static void get_sensors_from_db(){
  int id_field;
  int i;
  PGresult *res;
  sens_id_list_ptr new_id, temp_id = NULL;
  
  connection = pg_check_connect(conn_string);
  
  res = pg_check_exec(connection, "SELECT id FROM sensoren");
  id_field = PQfnumber(res, "id");
  
  for (i = 0; i < PQntuples(res); i++){
    new_id = malloc(sizeof(sensor_id));
    new_id->id = atoi(PQgetvalue(res, i, id_field));
    new_id->next = NULL;
    if (temp_id == NULL){
      temp_id = new_id;
      global_opts.sens_id_list = temp_id;
    } else {
      temp_id->next = new_id;
      temp_id = temp_id->next;
    }
    DEBUGOUT2("add id: (%d)\n", new_id->id);
  }

  PQclear(res);
  PQfinish(connection);
  connection = NULL;
}

static int count_data_by_sensor_id(PGconn *connection, int sens_id){
  int table_field;
  int count_field;
  char *query_buff 	= malloc(sizeof(char)*BUFFSIZE);
  char *table;
  int count;
  PGresult *table_res;
  PGresult *count_res;

  DEBUGOUT2("\nPrüfe Sensor mit ID: %d ... \n",sens_id);

  snprintf(query_buff, BUFFSIZE, "select typen.tabelle as tbl FROM typen, sensoren WHERE sensoren.id=%d AND typen.typ=sensoren.typ", sens_id);
  table_res = pg_check_exec(connection, query_buff);
  
  if(PQntuples(table_res) < 1)
    return -1;
  
  table_field = PQfnumber(table_res, "tbl");
  table       = PQgetvalue(table_res, 0, table_field);
  
  DEBUGOUT2("\tTabelle: %s \n", table);

  snprintf(query_buff, BUFFSIZE, "SELECT count(sens_id) as num FROM %s WHERE sens_id=%d AND timestamp>(current_timestamp - INTERVAL '%d hours')",table, sens_id, global_opts.interval);
  count_res = pg_check_exec(connection, query_buff);
  
  if(PQntuples(count_res) < 1)
    return -2;

  count_field = PQfnumber(count_res, "num");
  count       = atoi(PQgetvalue(count_res, 0, count_field));

  DEBUGOUT3("\tWerte in den letzten %d Stunden: %d\n", global_opts.interval, count);

  PQclear(count_res);
  PQclear(table_res);
  free(query_buff);

  return count;
}



static int check_sensors(){
  sens_id_list_ptr temp_ptr = global_opts.sens_id_list;
  int count;
  
  connection = pg_check_connect(conn_string);

  for(;temp_ptr ; temp_ptr = temp_ptr->next){
    if((count = count_data_by_sensor_id(connection, temp_ptr->id)) < global_opts.sendings){
      
    } else {
      DEBUGOUT2("\tSensor mit ID %d scheint ok zu sein\n", temp_ptr->id);
    }
  }

  PQfinish(connection);
  connection = NULL;
}

/* Mainfkt. und diverse andere Funktionen zum beenden des Programmes ---*/

/* Main-Funktion */
int main(int argc, char *argv[]){
  /* errno 0 setzen */
  errno = 0;

  DEBUGOUT1("Programm gestartet\n");

  /* Signal-Handling zum Abbrechen ses Progammes */
  if(signal(SIGABRT, exit_sig_handler) == SIG_ERR)
    exit_error(ERROR_SEIINST);
  DEBUGOUT1("Signalhandler zum beenden per SIGABRT installiert\n");
  if(signal(SIGINT, exit_sig_handler) == SIG_ERR)
    exit_error(ERROR_SEIINST);
  DEBUGOUT1("Signalhandler zum beenden per SIGINT installiert\n");
  if(signal(SIGQUIT, exit_sig_handler) == SIG_ERR)
    exit_error(ERROR_SEIINST);
  DEBUGOUT1("Signalhandler zum beenden per SIGQUIT installiert\n");
  if(signal(SIGTERM, exit_sig_handler) == SIG_ERR)
    exit_error(ERROR_SEIINST);
  DEBUGOUT1("Signalhandler zum beenden per SIGTERM installiert\n");

  read_config(DEFAULT_CONFIG_FILE,1);

  /* Debug-Ausgaben, um zu sehen, ob die Optionen richtig gesetzt werden */
  DEBUGOUT1("\nPostgres:\n");
  DEBUGOUT2("  Host:     =  %s\n",global_opts.pg_host);
  DEBUGOUT2("  User:     =  %s\n",global_opts.pg_user);
  DEBUGOUT2("  Pass:     =  %s\n",global_opts.pg_pass);
  DEBUGOUT2("  Datenbank =  %s\n",global_opts.pg_database);

  generate_conn_string();
  get_sensors_from_db();
  check_sensors();

  clean();
  return EXIT_SUCCESS;
}


/* Diese Funktion beendet das Programm mit einer Fehlermeldung. */
void exit_error(char* err){
  DEBUGOUT1("\nEtwas unschoenes ist passiert\n");
  clean();
  if(errno != 0){
    perror("Fehler");  
  }
  write(STDOUT_FILENO, err, strlen(err));
  exit(1);
}

/* Wird bei Beendigungssignalen ausgefuehrt */
static void exit_sig_handler(int signr){
  #ifdef DEBUG
  DEBUGOUT1("\n");
  switch (signr){
    case SIGABRT:
      DEBUGOUT1("SIGABRT Interupt erhalten!\n");
    case SIGTERM:
      DEBUGOUT1("SIGTERM Interupt erhalten!\n");
    case SIGQUIT:
      DEBUGOUT1("SIGQUIT Interupt erhalten!\n");
    case SIGINT:
      DEBUGOUT1("SIGINT Interupt erhalten!\n");
  }
  #endif
  clean();
  DEBUGOUT1("Beende Programm...\n");
  exit(0);
}


/* Aufraumen ausfuehren */
static void clean(){
  DEBUGOUT1("\nRaeume auf...\n");
  if(connection != NULL)
    PQfinish(connection);
}


