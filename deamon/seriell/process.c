/*

   weatherdeamon -- Weather Data Capture Program for the 
                    'ELV-PC-Wettersensor-Empfänger'
   process.c     -- Part of the weatherdeamon

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


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>
#include "process.h"
#include "main.h"
#include "definitions.h"
#include "config.h"
#include "write.h"

/* Funktionsdefinitionen */
static u_char get_bit(u_char, u_char);
static u_char get_2bits(u_char, u_char);
static u_char get_hi_nibble(u_char);
static u_char get_lo_nibble(u_char);
static u_char remove_msb(u_char);
static int convert_signed_int(u_char, u_char);
static unsigned int convert_unsigned_int(u_char, u_char);
static auss1_data process_auss1(time_t, u_char, u_char *);
static auss2_data process_auss2(time_t, u_char, u_char *);
static regen_data process_regen(time_t, u_char, u_char *);
static winds_data process_winds(time_t, u_char, u_char *);
static innen_data process_innen(time_t, u_char, u_char *);
static helli_data process_helli(time_t, u_char, u_char *);
static pyano_data process_pyano(time_t, u_char, u_char *);
static void check_log_buffer();
static char* prepend_type_address(u_char, u_char);

/* globale Variablen */
char *log_buffer = NULL;   	/* Puffer für die Log-Ausgabe */
int last_rain_count = -1;	/* Letzter gemessener Wasserstand */


/* Bestimmung der Sensoren und Weiterreichung an die Funktionen zum auswerten der Werte
 * sowie zum wegschreiben */
void process_data(time_t timestamp, u_char *buffer){
  u_char type; 			/* Sensortyp */
  u_char address;		/* Sensoraddresse */

  type = get_hi_nibble(remove_msb(buffer[1]));
  address = get_lo_nibble(buffer[1]);

  DEBUGOUT1(asctime(localtime(&timestamp)));

  switch (type){
  case FERNB :
    DEBUGOUT2("Fernbedienung an Addresse %i\n", address);
    DEBUGOUT1("\nnoch nix zu implementiert!\n\n");
    break;
  case AUSS1 :
    write_auss1(process_auss1(timestamp,address, buffer));
    break;
  case AUSS2 :
    write_auss2(process_auss2(timestamp,address, buffer));
    break;
  case REGEN :
    write_regen(process_regen(timestamp,address, buffer));
    break;
  case WINDS :
    write_winds(process_winds(timestamp,address, buffer));
    break;
  case INNEN :
    write_innen(process_innen(timestamp,address, buffer));
    break;
  case HELLI :
    write_helli(process_helli(timestamp,address, buffer));
    break;
  case PYANO :
    write_pyano(process_pyano(timestamp,address, buffer));
    break;
  }
}


/* Wertkonvertierungen --------------------------------------------------- */

/* Wertkonvertierungen für den 'einfachen' Außensensor */
static auss1_data process_auss1(time_t timestamp, u_char address, u_char *buffer){
  auss1_data data;						/* Datenstruktur */

  data.timestamp = timestamp;					/* Zeitstempel */
  data.address   = address; 					/* Addresse */
  data.temp      = convert_signed_int(buffer[2],buffer[3]);	/* Temparatur */

  DEBUGOUT2("Aussensensor1 an Addresse %i\n", address);
  DEBUGOUT2("Temp.: %3.1f\n", (data.temp*0.1));

  #ifndef NO_LOGING 
    if(get_flag(LOG_DATA_FLAG)){
      check_log_buffer();
      sprintf(prepend_type_address(AUSS1,address), "Temp: %3.1f°C",(data.temp*0.1));
      log_data(timestamp, log_buffer);
    }
  #endif

  return data;
}

/* Wertkonvertierungen für den 'besseren' Außensensor */
static auss2_data process_auss2(time_t timestamp, u_char address, u_char *buffer){
  auss2_data data;						/* Datenstruktur */

  data.timestamp = timestamp;					/* Zeitstempel */
  data.address   = address;					/* Addresse */
  data.temp      = convert_signed_int(buffer[2],buffer[3]);	/* Temparatur */
  data.hum       = remove_msb(buffer[4]);			/* Luftfeuchte */

  DEBUGOUT2("Aussensensor2 an Addresse %i\n", address);
  DEBUGOUT3("Temp.: %3.1f°C  Feuchte: %d%%\n",(data.temp*0.1),data.hum);

  #ifndef NO_LOGING 
    if(get_flag(LOG_DATA_FLAG)){
      check_log_buffer();
      sprintf(prepend_type_address(AUSS2,address), "Temp: %3.1f°C  Hum: %d%%",(data.temp*0.1),data.hum);
      log_data(timestamp, log_buffer);
    }
  #endif

  return data;
}

/* Wertkonvertierungen für den Regensensor (ein Zählschritt = 370ml/m^2)*/
static regen_data process_regen(time_t timestamp, u_char address, u_char *buffer){
  regen_data data;							/* Datenstruktur */
  int new_rain_count = 0;						/* Neuer Zählerstand */
  int now_rain_count = 0;						/* Delta-Zählerstand */

  data.timestamp = timestamp;						/* Zeitstempel */
  data.address   = address;						/* Addresse */
  new_rain_count = ((buffer[2] & 0x1F) << 7) | remove_msb(buffer[3]);	/* Niederschlagszähler */
  
  if(last_rain_count == -1)						/* Nach Programmstart Zähler initialisieren */
    last_rain_count = new_rain_count;

  now_rain_count = new_rain_count - last_rain_count;			/* neuen Niederschlag berechnen */
  
  if(now_rain_count < 0){						/* Wenn Integerüberlauf im Sensor */
    now_rain_count = (0x3FFF - last_rain_count) + new_rain_count;	/* Dann letzten gemessenen Wert vom Max-Integer-Wert abziehen und neuen Zählwert dazurechnen */
    DEBUGOUT1("Integer-Überlauf\n");
  }

  data.counter = (now_rain_count * 370);				/* Ein Zählschritt entspricht 370ml/m^2, also änderung mit 370 multiplizieren und zuweisen */

  last_rain_count = new_rain_count;					/* Zähler neu setzen */

  DEBUGOUT2("Regensensor an Addresse %i\n", address);
  DEBUGOUT3("Zähler: %d  Differenz: %d\n", new_rain_count,now_rain_count);
  DEBUGOUT2("Niederschlag: %dml/m^2\n", data.counter);

  #ifndef NO_LOGING 
    if(get_flag(LOG_DATA_FLAG)){
      check_log_buffer();
      sprintf(prepend_type_address(REGEN,address), "Rain: %dml/m^2",data.counter);
      log_data(timestamp, log_buffer);
    }
  #endif

  return data;
}

/* Wertkonvertierungen für den Windsensor */
static winds_data process_winds(time_t timestamp, u_char address, u_char *buffer){
  winds_data data;						/* Datenstruktur */
  
  data.timestamp = timestamp;					/* Zeitstempel */
  data.address   = address;					/* Addresse */
  data.geschw    = convert_unsigned_int(buffer[2],buffer[3]);	/* Windgeschwindigkeit */
  data.schwank   = buffer[4] & 0x03;				/* Schwankungsbreite */
  data.richt     = convert_unsigned_int(buffer[5],buffer[6]);	/* Windrichtung */

  DEBUGOUT2("Windsensor an Addresse %i\n", address);
  DEBUGOUT4("Geschw.: %3.1fKm/h  Schwank: %d  Richtung: %d°\n",(data.geschw*0.1),data.schwank,data.richt);

  #ifndef NO_LOGING 
    if(get_flag(LOG_DATA_FLAG)){
      check_log_buffer();
      sprintf(prepend_type_address(WINDS,address), "Spd: %3.1fKm/h  Fluct: %d%%  Dir:%d°",(data.geschw*0.1),data.schwank,data.richt);
      log_data(timestamp, log_buffer);
    }
  #endif

  return data;
}

/* Wertkonvertierungen für den Innensensor */
static innen_data process_innen(time_t timestamp, u_char address, u_char *buffer){
  innen_data data;						/* Datenstruktur */

  data.timestamp = timestamp;					/* Zeitstempel */
  data.address   = address;					/* Addresse */
  data.temp      = convert_signed_int(buffer[2],buffer[3]);	/* Temparatur */
  data.hum       = remove_msb(buffer[4]);			/* Luftfeuchte */
  data.press     = convert_unsigned_int(buffer[5],buffer[6]);	/* Luftdruck */

  DEBUGOUT2("Innensensor an Addresse %i\n",address);
  DEBUGOUT4("Temp.: %3.1f°C  Feuchte: %d%%  Druck: %dhPa\n",(data.temp*0.1),data.hum,data.press);

  #ifndef NO_LOGING 
    if(get_flag(LOG_DATA_FLAG)){
      check_log_buffer();
      sprintf(prepend_type_address(INNEN,address), "Temp: %3.1f°C  Hum: %d%%  Press: %dhPa",(data.temp*0.1),data.hum,data.press);
      log_data(timestamp, log_buffer);
    }
  #endif

  return data;
}

/* Wertkonvertierungen für den Helligkeitssensor */
static helli_data process_helli(time_t timestamp, u_char address, u_char *buffer){
  helli_data data;						/* Datenstruktur */

  data.timestamp = timestamp;					/* Zeitstempel */
  data.address   = address;					/* Addresse */
  data.rawlum    = convert_unsigned_int(buffer[2],buffer[3]);	/* 'Roh-Helligkeit' - Helligkeit ohne Multiplikator */
  data.mult      = buffer[4] & 0x03;				/* Multiplikator */
  data.abslum    = (0x01 << data.mult) * data.rawlum;		/* Absolute Helligkeit */

  DEBUGOUT2("Helligkeitssensor an Addresse %i\n", address);
  DEBUGOUT4("Rel. Hell.: %d  Mult.: %d  Abs. Hell.: %d\n",data.rawlum,data.mult,data.abslum);  

  #ifndef NO_LOGING 
    if(get_flag(LOG_DATA_FLAG)){
      check_log_buffer();
      sprintf(prepend_type_address(HELLI,address), "Bright:  %d",data.abslum);
      log_data(timestamp, log_buffer);
    }
  #endif

  return data;
}

/* Wertkonvertierungen für das Pyranometer */
static pyano_data process_pyano(time_t timestamp, u_char address, u_char *buffer){
  pyano_data data;						/* Datenstruktur */

  data.timestamp = timestamp;					/* Zeitstempel */
  data.rawrad    = convert_unsigned_int(buffer[2],buffer[3]);	/* 'Roh-Strahlung' - Strahlung ohne Multiplikator */
  data.mult      = buffer[4] & 0x03;				/* Multiplikator */
  data.absrad    = (0x01 << data.mult) * data.rawrad;		/* Absolute Strahlung */

  DEBUGOUT2("Pyranometer an Addresse %i\n", address);
  DEBUGOUT4("Rel. Strahl.: %d  Mult.: %d  Abs. Strahl.: %d\n",data.rawrad,data.mult,data.absrad);             

  #ifndef NO_LOGING 
    if(get_flag(LOG_DATA_FLAG)){
      check_log_buffer();
      sprintf(prepend_type_address(PYANO,address), "Int: %d",data.absrad);
      log_data(timestamp, log_buffer);
    }
  #endif

  return data;
}



/* Verschiedenes ----------------------------------------------------------*/

/* einen vorzeichenbehafteten 14-Bit Binärwert in einen Int umwandeln*/
static int convert_signed_int(u_char hi_byte, u_char lo_byte){
  int val;  
  val = convert_unsigned_int(hi_byte,lo_byte);
  if (get_bit(val,14)){
    val |= ~(0x3FFF);
  }
  return val;
}

/* einen 14-Bit Binärwert in einen Int umwandeln*/
static unsigned int convert_unsigned_int(u_char hi_byte, u_char lo_byte){
  return (remove_msb(hi_byte) << 7) | remove_msb(lo_byte);
}


/* Ein Bit vom Byte */
static u_char get_bit(u_char byte, u_char bit){       
  return byte >> bit & 0x01;
}
                
/* 2 Bit vom Byte */
static u_char get_2bits(u_char byte, u_char bit){       
  return byte >> bit & 0x03;
}               
                
/* oberes halbbyte */
static u_char get_hi_nibble(u_char byte){       
  return byte >> 4 & 0x0f;
}       

/* unteres halbbyte */
static u_char get_lo_nibble(u_char byte){       
  return byte & 0x0f;
}    

/* Erstes Bit 0 setzen */
static u_char remove_msb(u_char byte){
  return (byte & 0x7f);
}

/* Checkt, das das MSB gesetzt ist */
int check_msb_set(u_char byte){
  return (byte & 0x80);
}

#ifndef NO_LOGING

/* Prüfen ob Puffer für Logausgaben angelegt und renn nicht, dann anlegen */
static void check_log_buffer(){
  if (log_buffer == NULL){
    log_buffer = malloc(sizeof(char)*LOG_BUFFERSIZE);
  }
}

/* In der Log-Zeile die Addresse und den Typ des Sensors davorbauen 
 * gibt die Addresse zurück, an der die Werte weitergeschrieben werden können */
static char* prepend_type_address(u_char type, u_char address){
  return (log_buffer + sprintf(log_buffer, "Sens: %i:%i - ", type, address));
}

#endif
