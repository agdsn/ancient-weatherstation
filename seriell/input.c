/*

   weatherdeamon -- Weather Data Capture Program for the 
                    'ELV-PC-Wettersensor-Empfänger'
   input.c       -- Part of the weatherdeamon

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


#include <string.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>          /* File control definitions */
#include <termios.h>        /* POSIX terminal control definitions */
#include <sys/types.h>
#include <poll.h>           /* Polling */

#include "config.h"
#include "definitions.h"
#include "main.h"


static int fd;					/* File-Deskriptor */

static void check_data(int, u_char*);
static int set_port();
static void clean_port(void *);
        
static int set_port()
{
  /** Diese Funktion stellt die Schnittstelle zum RS232 Port zur Verfuegung.  *
   *  Gleichzeitig werden die Optionen so eingestellt, dass ein Abfragen der  *
   *  Daten moeglich ist.                                                     *
   *  Zurueck gegeben wird der Filedeskriptor.                                */

  struct termios options;                       /* Optionen Schnittstelle */
  int fd;					/* File-Deskriptor */

  /* oeffnen der Schnittstelle - nur lesen, nicht blockierend */
  if ((fd = open(global_opts.device,O_RDONLY | O_NONBLOCK | O_NOCTTY | O_NDELAY)) < 0)
    exit_error(ERROR_OPEN);

  /* Erstmal alles zurücksetzen*/
  cfmakeraw(&options);
  /* Uebertragungsgeschwindigkeit 19200 Baud */
  cfsetispeed(&options, B19200);
  /* Control-Flags fertig codiert, funktioniert so!
   * (so lange mit minicom rumgespielt bis es geklappt hat)
   * wer lust hat kann das gern dekodieren und die Flags einzeln setzen.*/
  options.c_cflag = 0x80000BFE;
  /* Output-Flags brauchmer net*/
  options.c_oflag = 0;
  /* Break-conditions ignorieren wir einfach*/
  options.c_iflag =  IGNBRK;
  /* Local-Flags brauchmer auch keine*/
  options.c_lflag = 0;

  /* Zeug für die Flusskontrolle */
  //options.c_cc[VMIN] = 1;
  //options.c_cc[VTIME] = 5;


  tcflush(fd, TCIFLUSH);
  if ( tcsetattr(fd, TCSANOW, &options) != 0 )  /* Optionen schreiben */
    exit_error(ERROR_AKTIV);
  
  DEBUGOUT2("\nInterface '%s' offen\n", global_opts.device);

  return fd;
}

/* Diese Funktion liest einen Datensatz der seriellen Schnittstelle aus    *
 * und gibt diesen nach STDOUT.                                            *
 * Uebergeben wird der Filedeskriptor.                                     */
int read_port(){
  fd = set_port();
  int len=0; 				   		/* Anzahl gelesener Daten */
  int i, readlen =1;					/* Laufvariable, Lesezähler */ 
  u_char  buffer[INPUT_BUFFERSIZE], *temp = NULL;       /* Lesepuffer, Temp-Zeiger */
  struct pollfd pfd = {fd, POLLIN, (short)NULL};					/* polling-Optionen */

  temp = buffer;					/* Temporärer Zeiger bekommt die Anfangsaddresse vom Puffer */

  add_clean(clean_port, &fd);


  DEBUGOUT1("\nWarte auf Daten\n\n");

  do {
    if((len = poll(&pfd, 1, INPUT_TIMEOUT)) > 0){ 	/* warten auf Eingabepuffer */
      len = read(fd, temp, 1);				/* Zeichenweise lesen */
      if(readlen == 1){					/* Solange noch nix verwertbares gelesen wurde auf das Startbyte prüfen */
        if(*temp == STX){
	  temp++;					/* Wenn startbyte gefunden Zeiger auf das 2. Puffer-Element setzen */
	  readlen++;					/* Und Lese-Zähler erhöhen */
	}
      } else {						/* Wenn schon was relevantes gelesen wurde */
        if(readlen == 8 && *temp == ETX){		/* Wenn schon 8 Byte gelesen wurden und das letzte Byte das Stoppbyte ist */
          
	  FOR(i=0; i < readlen; i++)
	    DEBUGOUT2("%x ",buffer[i]);
          DEBUGOUT1("\n");
          
	  check_data(readlen,buffer);			/* Daten nochmals Prüfen und verarbeiten */
          
	  DEBUGOUT1("------------------------------------------\n");
          
	  readlen = 1;					/* Lesezähler zurücksetzen */
	  temp = buffer;				/* Temp.-Zeiger wieder auf Pufferanfang setzen */
	} else {					/* Wenn schon was relevantes da war, aber noch keine 8 byte incl. Stoppbyte */
          if(*temp == ETX || readlen >= 8){		/* Wenn das aktuelle byte das Stoppbyte oder das 8. Byte, dann sind die daten nicht ok */
            readlen = 1;				/* Lesezähler zurücksetzen */
	    temp = buffer;				/* Temp-Zeiger auf Pufferanfang */
	  } else {					/* Sonst */
            readlen++;					/* Lesezähler erhöhen */
	    temp++;					/* Temp.-Zeiger auf das nächste element setzen */
	  }
	}
      }
    //write(STDOUT_FILENO, buffer, n);            /* Ausgabe Messung */
    }
  } while (buffer[0] != '$');                   /* Ende Datensatz */

  close(fd);                                    /* Schnittstelle schliessen */

  return 1;
}

/* Überprüfung, ob die Struktur der Daten stimmt
 * es sollten 8 Byte ankommen:
 * <STX><Typ><W1><W2><W3><W4><W5><EXT>
 * wobei <STX> = 02h
 * und   <EXT> = 03h
 * Bei <Typ>,<W1>...<W7> ist bit 7 (MSB) immer gesetzt, gehört also nicht zu den Daten!
 * Im <Typ>-Byte sind die unteren 4 bit die Addresse und die oberen 3 (weil 7. ist ja nicht benutzt)
 * der Typ des Sensors. 
 * Die Typenzuordnung steht in der definitions.h.
 * <W1>...<W7> sind die Werte des Sensors.
 * Eine genaue Beschreibung ist unter 
 * http://www.elv-downloads.de/service/manuals/PC-WS-Testempfaenger/39061-PCSensor_km.pdf
 * zu finden. */
static void check_data(int len, u_char *buffer){
  time_t timestamp = time(NULL);
  if(len==8){
    if(buffer[0]==STX && check_msb_set(buffer[1]) && buffer[7]==ETX){
      process_data(timestamp, buffer);
    }
  }
}

static void clean_port(void *data){
  close(*((int *)data));
  DEBUGOUT1("\nPort geschlossen\n");
}
