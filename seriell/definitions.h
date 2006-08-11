/*

   weatherdeamon -- Weather Data Capture Program for the 
                    'ELV-PC-Wettersensor-Empfänger'
   definitions.h -- Part of the weatherdeamon

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

/* Default-Werte ----------------------------------------------------- */
#define DEFAULT_SERIAL_PORT	"/dev/ttyS0"		/* Serielle Schnittstelle */
#define DEFAULT_DATA_FILE 	"./data.log"		/* Daten-Logfile */
#define DEFAULT_ERROR_FILE 	"./error.log"		/* Error-Logfile */
#define DEFAULT_PG_HOST 	"localhost"		/* Postgres-host */
#define DEFAULT_PG_USER		"localuser"		/* Postgres-Username */
#define DEFAULT_PG_PASS 	""			/* Postgres-Passwort */
#define DEFAULT_PG_DATABASE 	"localbase"		/* Postgres-Datenbank */
#define DEFAULT_CONFIG_FILE	"./weather.conf"	/* Standart-Configdatei */



/*Alle möglichen Definitionen, die in allen code-schnipseln benötigt werden*/

/* Sensor definitionen ------------------------------------------------- */
#define FERNB 0x07	/* Funkfernbedienung */
#define AUSS1 0x00	/* 'einfacher' Außensensor */
#define AUSS2 0x01	/* 'besserer' Außensensor */
#define REGEN 0x02	/* Regensensor */
#define WINDS 0x03	/* Windsensor */
#define INNEN 0x04	/* Innensensor */
#define HELLI 0x05	/* Helligkeitssensor */
#define PYANO 0x06	/* Pyanometer */


/* Fehlermeldungen ------------------------------------------------------*/
#define  ERROR_OPEN    "sensor-rs232interface: Fehler beim Oeffnen der RS232-Schnittstelle\n"
#define  ERROR_OPTIONS "sensor-rs232interface: Fehler bei der Abfrage der Optionen\n"
#define  ERROR_AKTIV   "sensor-rs232interface: Fehler beim Aktivieren der Optionen\n"
#define  ERROR_TIMEOUT "sensor-rs232interface: keine Daten - timeout-Fehler\n"
#define  ERROR_POLLING "sensor-rs232interface: keine Daten - polling-Fehler\n"
#define  ERROR_READ    "sensor-rs232interface: Fehler beim Lesen\n"
#define  ERROR_LOGFILE "Log-Error: Kann Logfile nicht öffnen\n"
#define  ERROR_FORK    "Fork-Fehler: Kann den Prozess nicht in den Hintergrund schieben\n"


/* Parameter-Flags ----------------------------------------------------- */
#define LOG_ERROR_FLAG 	0x01
#define LOG_DATA_FLAG  	0x02
#define FOREGROUND_FLAG	0x04
#define VERBOSE_FLAG	0x08


/* Puffergrößen -------------------------------------------------------- */
#define LOG_BUFFERSIZE    100 	/* Größe für den Log-Puffer in process.c */
#define ERR_BUFFERSIZE    512 	/* Größe für den error-Puffer in main.c  */
#define INPUT_BUFFERSIZE  8	/* Größe für den Input-Puffer in input.c */
#define CONFIG_BUFFERSIZE 512	/* Größe des Puffers zum Config-einlesen */
#define QUERY_BUFFERSIZE  512	/* Größe des Zeichenpuffers für SQL-Anfragen */


/* Timeout-Zeiten ------------------------------------------------------ */
#define INPUT_TIMEOUT 10       


/* serial protocol definitions ----------------------------------------- */
#define STX 0x02		/* Startbyte */
#define ETX 0x03 		/* Endbyte */


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
