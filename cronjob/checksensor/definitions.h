/*

   definitions.h -- Part of the checksensor

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
#define DEFAULT_CONFIG_FILE	"/etc/weatherstation/checksensor.conf"	/* Standart-Configdatei */
#define DEFAULT_CHECK_INTERVAL	"24"			/* Standart-Interval, indem der Sensor das letzte mal haette senden sollen (in h) */
#define DEFAULT_MIN_SENDINGS	"24"			/* Standart-Wert, wieviele Daten der Sensor in den letzten x stunden haette senden sollen */


/*Alle moeglichen Definitionen, die in allen code-schnipseln benoetigt werden*/

/* Fehlermeldungen ------------------------------------------------------*/
#define  ERROR_MAKECONN "PgSQL-Fehler: Kann Datenbankverbindung nicht herstellen\n"
#define  ERROR_QUERY	"PgSQL-Fehler: Kann query nicht sudfuehren \n"
#define  ERROR_SEIINST "Signal-Fehler: Kann Signalhandler zum beenden nicht installieren\n"

/* Mail - Kopf -----------------------------------------------------------*/
#define MAIL_HEAD	"--Wetterstation--\r\nEs sind Probleme mit einigen Sensoren aufgetreten:\r\n\r\n"


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

