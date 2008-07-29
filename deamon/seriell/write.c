/*

   weatherdeamon -- Weather Data Capture Program for the 
                    'ELV-PC-Wettersensor-Empfaenger'
   write.c       -- Part of the weatherdeamon

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

/*
 * Allgemeines zu dieser File:
 * 
 * Sollte irgend jemand das Programm weiter verwenden,
 * jedoch die Daten anders oder woandershin wegschreiben
 * wollen, so muessen nur diese Funktionen neu implementiert
 * werden. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <postgresql/libpq-fe.h>
#include "definitions.h"
#include "main.h"
#include "write.h"


/* Globale Variablen -----------------------------------------*/
static char *query_buffer = NULL;
static PGconn *connection = NULL;
static char *conn_string = NULL;


/* Funktionen ----------------------------------------------- */
static void check_create_query_buffer();
static int pg_connect();
static void pg_insert(char *);
static void clean_write(void *);

/* Funktionen zum wegschreiben -------------------------------*/

/* 'einfacher' Auﬂensensor */
void write_auss1(auss1_data data){
  check_create_query_buffer();
  snprintf(query_buffer, QUERY_BUFFERSIZE,  "INSERT INTO auss1_data (sens_id, \"temp\") VALUES (get_sensor_id(%d,%d),%d)",AUSS1, data.address, data.temp);
  pg_insert(query_buffer);
}

/* 'besserer' Auﬂensensor */
void write_auss2(auss2_data data){
  check_create_query_buffer();
  snprintf(query_buffer, QUERY_BUFFERSIZE,  "INSERT INTO auss2_data (sens_id, \"temp\", hum) VALUES (get_sensor_id(%d,%d),%d,%d)",AUSS2, data.address, data.temp, data.hum);
  pg_insert(query_buffer);
}

/* Regensensor */
void write_regen(regen_data data){
  check_create_query_buffer();
  snprintf(query_buffer, QUERY_BUFFERSIZE,  "INSERT INTO regen_data (sens_id, count) VALUES (get_sensor_id(%d,%d),%d)",REGEN, data.address, data.counter);
  pg_insert(query_buffer);
}

/* Windsensor */
void write_winds(winds_data data){
  check_create_query_buffer();
  snprintf(query_buffer, QUERY_BUFFERSIZE,  "INSERT INTO winds_data (sens_id, geschw, schwank, richt) VALUES (get_sensor_id(%d,%d),%d,%d,%d)",WINDS, data.address, data.geschw, data.schwank, data.richt);
  pg_insert(query_buffer);
}

/* Innensensor */
void write_innen(innen_data data){
  check_create_query_buffer();
  snprintf(query_buffer, QUERY_BUFFERSIZE,  "INSERT INTO innen_data (sens_id, \"temp\", hum, press) VALUES (get_sensor_id(%d,%d),%d,%d,%d)",INNEN, data.address, data.temp, data.hum, data.press);
  pg_insert(query_buffer);
}

/* Helligkeitssensor */
void write_helli(helli_data data){
  check_create_query_buffer();
  snprintf(query_buffer, QUERY_BUFFERSIZE,  "INSERT INTO helli_data (sens_id, bight) VALUES (get_sensor_id(%d,%d),%d)",HELLI, data.address, data.abslum);
  pg_insert(query_buffer);
}

/* Pyarnometer */
void write_pyano(pyano_data data){
  check_create_query_buffer();
  snprintf(query_buffer, QUERY_BUFFERSIZE,  "INSERT INTO pyano_data (sens_id, intens) VALUES (get_sensor_id(%d,%d),%d)",PYANO, data.address, data.absrad);
  pg_insert(query_buffer);
}

/* Testen ob Puffer angelegt und bei bedarf anlegen */
static void check_create_query_buffer(){
  if(query_buffer == NULL){
    query_buffer= malloc(sizeof(char)*QUERY_BUFFERSIZE);
  }
}

/* Guckt ob Verbindung da und versucht aufzubauen. 
 * gibt 1 zurueck, wenn erfolgreich, sonst 0 */
static int pg_connect(){
  if (PQstatus(connection) == CONNECTION_OK){
    PQexec(connection,"SELECT 1");				/* Status neusetzen erzwingen */
  }
  if(PQstatus(connection) != CONNECTION_OK){
    if (connection == NULL){
      if(conn_string == NULL){
	conn_string = malloc(sizeof(char)*512);
	snprintf(conn_string, 512, "host=%s dbname=%s user=%s password=%s connect_timeout=%s", global_opts.pg_host, global_opts.pg_database, global_opts.pg_user, global_opts.pg_pass, global_opts.pg_timeout);
      }
      connection = PQconnectdb(conn_string);			/* Connection aufbauen */
      add_clean(clean_write, connection);			/* Callbackfunktion zum Aufraeumen registrieren */
    } else {
      PQreset(connection);					/* Connecion resetten */
    }
    if(PQstatus(connection) != CONNECTION_OK){
      DEBUGOUT2("\nFehler beim Aufbau der Datenbankverbindung\n%s\n", PQerrorMessage(connection));
      #ifndef NO_LOGING
      snprintf(get_error_buffer(), ERR_BUFFERSIZE, "Fehler beim Aufbau der Datenbankverbindung: %s", PQerrorMessage(connection));
      log_error(get_error_buffer());
      #endif
      return 0;
    }
    DEBUGOUT1("\nDatenbankverbindung erfolgreich hergestellt\n");
  } 
  return 1;
}

/* Ein Datum in die Datenbank schreiben */
static void pg_insert(char *query){
  PGresult *res;
  if(pg_connect()){
    res = PQexec(connection, query);
    if(!res || PQresultStatus(res) != PGRES_COMMAND_OK){
      DEBUGOUT2("Fehler beim INSERT: %s\n", query);
      #ifndef NO_LOGING
      snprintf(get_error_buffer(), ERR_BUFFERSIZE, "Fehler beim INSERT: %s", query);
      log_error(get_error_buffer());
      #endif
    } else {
      DEBUGOUT2("Query: '%s' ausgefuehrt\n", query);
    }
    PQclear(res);
  }
}

/* Callbackfunktion zum Aufraeume.
 * Schliesst die Verbindung zur Datenbank */
static void clean_write(void *data){
  PGconn *conn =  data;
  PQfinish(conn);
  DEBUGOUT1("\nVerbindung Geschlossen \n");
}
