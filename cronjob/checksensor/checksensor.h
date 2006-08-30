/*

   main.h        -- Part of the weatherdeamon

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

#include "mailer.h"


/* Datenstrukturen --------------------------------------------------------*/

typedef struct sens_id_list *sens_id_list_ptr;
typedef struct sens_id_list {
  sens_id_list_ptr 	next;
  int 			id;
} sensor_id;

typedef struct address_t *mail_list_ptr;

/* Optionen */
typedef struct {
  int 			*interval;	/* Das Interval, in dem ein Sensor zuletzt gesendet haben sollte */
  mail_list_ptr 	address_list;	/* Liste der Ids der Sensoren, die ueberprueft werden sollen */
  sens_id_list_ptr 	sens_id_list;	/* Liste der Mail-Addressen, die benachichtigt werden sollen wenn was ist */
  char			id_from_db;	/* Flag, ob die Id's aus der Datenbank gelesen werden sollen */
  char 			*pg_host;	/* Postgres-Host */
  char 			*pg_user;	/* Postgres-Username */
  char 			*pg_pass;	/* Postgres-Password */
  char 			*pg_database;	/* Postgres-Datenbank */
  char 			*mail_host;
  int 			mail_port;
  char			mail_ssl;
  char 			mail_auth;
  char 			*mail_auth_user;
  char 			*mail_auth_pass;
} w_opts;



/* Funktionen -------------------------------------------------------------*/

/* Programm mit einem Fatalem Fehler beenden.
 * Argument: Fehlermeldung */
void exit_error(char*);

/* Variablen --------------------------------------------------------------*/
extern w_opts global_opts;
