/*

   weatherdeamon -- Weather Data Capture Program for the 
                    'ELV-PC-Wettersensor-Empfänger'
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

   $Id: process.c v 1.00 11 Aug 2006 losinski $
*/


/* Datenstrukturen --------------------------------------------------------*/

/* Optionen */
typedef struct {
  unsigned char  flags;		/* Flags */
  char 		*device;	/* Das Serielle Device */
  char 		*pg_host;	/* Postgres-Host */
  char 		*pg_user;	/* Postgres-Username */
  char 		*pg_pass;	/* Postgres-Password */
  char 		*pg_database;	/* Postgres-Datenbank */
  char 		*pg_timeout;	/* Postgres-Timeout */
  #ifndef NO_LOGING
  char 		*data_log;	/* Das Daten-Logfile */
  char 		*error_log;	/* Das Fehler-Logfile */
  #endif
} w_opts;

/* Aufräumfunktionen */
/* TODO: sollen Funktionen werden, mit denen die eintelnen Teile funktionen registrieren können, die aufräiumen 
 * (Verbindung schließen, file schließen, ...) */
typedef struct clean_struct *clean_struct_ptr;
typedef struct clean_struct {
  void 		*data;
  void (*func)(void *data);
  clean_struct_ptr next;
} clean_data;


/* Funktionen -------------------------------------------------------------*/

/* Programm mit einem Fatalem Fehler beenden.
 * Argument: Fehlermeldung */
void exit_error(char*);

/* Ein Flag bekommen
 * Argument: Maske für das Flag, siehe definitions.h */
int get_flag(int);

#ifndef NO_LOGING
/* Schreiben einer Fehlermeldung in ein Fehler-Log
 * Das Argument ist die Fehermeldung */
void log_error(char *);

/* Schreiben eines Datensatzes in das Logfile
 * 1. Argument: Timestamp
 * 2. Argument: Log-Message */
void log_data(time_t, char *);

/* Gibt _EINEN_ Puffer für das Zusammensetzen 
 * der Fehler-Zeichenketten zurück. 
 * Es sollte wirklich bei jedem Aufruf der selbe 
 * Puffer sein */
char *get_error_buffer();
#endif

/* Ein neues Clean-Element anfügen. Ein Clean-Element ist eine Datenstruktur, die
 * einen Pointer auf eine Funktion vom Typ
 * void func(void *data),
 * einen Zeiger auf beliebige Daten und einen Zeiger auf das nächste Element hält.
 * Die Funktionen werden beim regulären beenden des Programmes aufgerufen um zum bsp. 
 * datenbankverbindungen zu schließen, etc. */
void add_clean(void (*func)(void *data), void *data);

/* Variablen --------------------------------------------------------------*/
extern w_opts global_opts;
