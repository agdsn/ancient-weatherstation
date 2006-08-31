/*

   main.h        -- Part of checksensor

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

*/
/*
 * Wenn mehr kommentare benoetigt werden:
 * losinski@wh2.tu-dresden.de 
 * */

#include "mailer.h"


/* Datenstrukturen --------------------------------------------------------*/

/* Liste der IDs der zu ueberpruefenden sensoren */
typedef struct sens_id_list *sens_id_list_ptr;
typedef struct sens_id_list {
  sens_id_list_ptr 	next;
  int 			id;
} sensor_id;

/* Pointer auf die Liste der Addresen an die die mail versendet wird */
typedef struct address_t *mail_list_ptr;

/* Optionen */
typedef struct {
  int 			interval;	/* Das Interval, in dem ein Sensor zuletzt gesendet haben sollte */
  int 			sendings;	/* Wie oft im gegebenen interval daten angekommen sein sollten */
  mail_list_ptr 	address_list;	/* Liste der Ids der Sensoren, die ueberprueft werden sollen */
  sens_id_list_ptr 	sens_id_list;	/* Liste der Mail-Addressen, die benachichtigt werden sollen wenn was ist */
  char			id_from_db;	/* Flag, ob die Id's aus der Datenbank gelesen werden sollen */
  char 			*pg_host;	/* Postgres-Host */
  char 			*pg_user;	/* Postgres-Username */
  char 			*pg_pass;	/* Postgres-Password */
  char 			*pg_database;	/* Postgres-Datenbank */
  char 			*mail_host;	/* Hostname (oder ip) des Mailservers */
  int 			mail_port;	/* Port des Mailservers */
  char			mail_ssl;	/* Flag ob SSL (TLS) genutzt werden soll */
  char 			mail_auth;	/* Flag ob authentifiziert werden soll */
  char 			*mail_auth_user;  /* User für die authentifizierung */
  char 			*mail_auth_pass;  /* Passwort für die Authentifizierung */
} w_opts;

/* Struktur, die Infos ueber den Sensor enthaelt */
typedef struct sens_info_list *sens_info_list_ptr;
typedef struct sens_info_list {
  int 			id;			/* ID des Sensors */
  int 			count;			/* Anzahl der datensaetze im Interval */
  char 			*type_desc;		/* Typenbeschreibung */
  char 			*sens_location;		/* Standort */
  char 			*sens_desc;		/* Beschreibung des Sensors */
  sens_info_list_ptr 	next;			/* naecstes Element */
} sensor_info;

/* Funktionen -------------------------------------------------------------*/

/* Programm mit einem Fatalem Fehler beenden.
 * Argument: Fehlermeldung */
void exit_error(char*);

/* Variablen --------------------------------------------------------------*/
extern w_opts global_opts;
