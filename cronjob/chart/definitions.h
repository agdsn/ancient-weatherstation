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
//#define DEFAULT_CONFIG_FILE	"/etc/weatherstation/chart.conf"		/* Standart-Configdatei */
#define DEFAULT_CONFIG_FILE	"/home/jan/wettertest/chart/chart.conf"		/* Standart-Configdatei */


/* Image-Default-Werte ----------------------------------------------- */
#define DEFAULT_HEADLINE 	"Wetter - Diagram"
#define DEFAULT_MANUAL_TABLE	"no"
#define DEFAULT_TABLE_FIELD	"temp"
#define DEFAULT_GEN_INTERVAL	"15m"
#define DEFAULT_SHOW_INTERVAL	"2d"
#define DEFAULT_LABEL_INTERVAL	"2h"
#define DEFAULT_FIX_TIMEPOINT	"no"
#define DEFAULT_TIMEPOINT	"2006-01-01-12-00"
#define DEFAULT_LABEL_SUM	"no"
#define DEFAULT_ZERO_MIN	"no"
#define DEFAULT_AS_BARS		"no"
#define DEFAULT_SHOW_AVERAGE	"yes"
#define DEFAULT_SHOW_MIN	"no"
#define DEFAULT_SHOW_MAX	"no"
#define DEFAULT_INVERT_MIN	"yes"
#define DEFAULT_INVERT_MAX	"yes"
#define DEFAULT_WIDTH		"800"
#define DEFAULT_HEIGHT		"300"
#define DEFAULT_SENS_ID		"1"
#define DEFAULT_VAL_KOEFF	"0.1"
#define DEFAULT_UNIT		"∞C"
#define DEFAULT_X_AX_DESC	"X-Axis"
#define DEFAULT_Y_AX_DESC	"Y-Axis"
#define DEFAULT_X_FORMAT	"%H:%M"
#define DEFAULT_X_FORMAT_EXTRA	"%d.%m.%y|  %H:%M"

/* Image-Default-Farben ---------------------------------------------- */
#define DEFAULT_BG_COLOR		"DF:DF:EF:00"
#define DEFAULT_DIA_BG_COLOR		"F0:A0:D0:D0"
#define DEFAULT_ZERO_LINE_COLOR		"BF:80:80:40"
#define DEFAULT_DIA_LINE_COLOR		"60:30:50:30"
#define DEFAULT_DIA_AV_LINE_COLOR	"30:60:50:30"
#define DEFAULT_DIA_GRID_X_COLOR	"60:60:60:EF"
#define DEFAULT_DIA_GRID_Y_COLOR	"60:60:60:EF"
#define DEFAULT_DIA_BORDER_COLOR	"a0:a0:a0:af"
#define DEFAULT_HEADLINE_COLOR		"80:10:10:30"
#define DEFAULT_LABEL_X_COLOR		"60:60:60:30"
#define DEFAULT_LABEL_EXTRA_X_COLOR	"60:10:10:20"
#define DEFAULT_LABEL_Y_COLOR		"20:20:20:20"
#define DEFAULT_DESCR_X_COLOR		"10:50:10:20"
#define DEFAULT_DESCR_Y_COLOR		"10:10:50:20"
#define DEFAULT_MIN_COLOR		"32:93:32:10"
#define DEFAULT_MAX_COLOR		"be:4a:4a:10"



/*Alle moeglichen Definitionen, die in allen code-schnipseln benoetigt werden*/


/* Fehlermeldungen ------------------------------------------------------*/
#define  ERROR_SEIINST "Signal-Fehler: Kann Signalhandler zum beenden nicht installieren\n"
#define  ERROR_FORK    "Fork-Fehler: Kann keinen neuen Prozess anlegen\n"
#define  ERROR_STAT    "Kann Stat nicht ausfuehren\n"
#define  ERROR_MAKECONN "PgSQL-Fehler: Kann Datenbankverbindung nicht herstellen\n"
#define  ERROR_QUERY	"PgSQL-Fehler: Kann query nicht ausfuehren \n"


/* Puffergroeﬂen -------------------------------------------------------- */
#define CONFIG_BUFFERSIZE 512	/* Groeﬂe des Puffers zum Config-einlesen */
#define QUERY_BUFFERSIZE  512	/* Groeﬂe des Zeichenpuffers fuer SQL-Anfragen */


/* Verschiedenes ------------------------------------------------------- */
#define FALSE 0
#define TRUE 1
#define IMG_FONT "/usr/share/fonts/truetype/freefont/FreeSans.ttf"
#define CRON_INTERVAL 15  //Minuten


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
