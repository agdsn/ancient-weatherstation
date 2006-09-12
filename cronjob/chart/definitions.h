/*

   definitions.h -- Part of Chart-generator for the weatherstation

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

/* Default-Werte ----------------------------------------------------- */
#define DEFAULT_PG_HOST 	"localhost"		/* Postgres-host */
#define DEFAULT_PG_USER		"localuser"		/* Postgres-Username */
#define DEFAULT_PG_PASS 	""			/* Postgres-Passwort */
#define DEFAULT_PG_DATABASE 	"localbase"		/* Postgres-Datenbank */
#define DEFAULT_PG_TIMEOUT 	"20"			/* Postgres-Timeout */
#define DEFAULT_CONFIG_FILE	"./chart.conf"		/* Standart-Configdatei */



/*Alle möglichen Definitionen, die in allen code-schnipseln benötigt werden*/


/* Fehlermeldungen ------------------------------------------------------*/
#define  ERROR_SEIINST "Signal-Fehler: Kann Signalhandler zum beenden nicht installieren\n"
#define  ERROR_FORK    "Fork-Fehler: Kann keinen neuen Prozess anlegen\n"
#define  ERROR_STAT    "Kann Stat nicht ausführen\n"
#define  ERROR_MAKECONN "PgSQL-Fehler: Kann Datenbankverbindung nicht herstellen\n"
#define  ERROR_QUERY	"PgSQL-Fehler: Kann query nicht sudführen \n"


/* Puffergrößen -------------------------------------------------------- */
#define CONFIG_BUFFERSIZE 512	/* Größe des Puffers zum Config-einlesen */
#define QUERY_BUFFERSIZE  512	/* Größe des Zeichenpuffers für SQL-Anfragen */


/* Verschiedenes ------------------------------------------------------- */
#define FALSE 0
#define TRUE 1


/* Debug --------------------------------------------------------------- */
#ifdef DEBUG
  #define DEBUGOUT1(x)         fprintf(stderr,x)
  #define DEBUGOUT2(x,y)       fprintf(stderr,x,y)
  #define DEBUGOUT3(x,y,z)     fprintf(stderr,x,y,z)
  #define DEBUGOUT4(x,y,z,a)   fprintf(stderr,x,y,z,a)
  #define DEBUGOUT5(x,y,z,a,b) fprintf(stderr,x,y,z,a,b)
  #define FOR(x)               for(x)
#else
  #define DEBUGOUT1(x)
  #define DEBUGOUT2(x,y)
  #define DEBUGOUT3(x,y,z)
  #define DEBUGOUT4(x,y,z,a)
  #define DEBUGOUT5(x,y,z,a,b)    
  #define FOR(x) 
#endif  

/* Dumping der Daten --------------------------------------------------- */
#ifndef NO_LOGING
  #define LOG_DATA(x,y)		log_data(x.y)
  #define LOG_ERROR(x)		log_error(x)
#else
  #define LOG_DATA(x,y)
  #define LOG_ERROR(x)
#endif
