/*

   weatherdeamon -- Weather Data Capture Program for the 
                    'ELV-PC-Wettersensor-Empf�nger'
   write.h       -- Part of the weatherdeamon

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

#include <time.h>

/* Datenstrukturen f�r die einzelnen Sensoren ------------------------------- */

/* Datenstruktur f�r den 'einfachen' Au�ensensor */
typedef struct {
  time_t        timestamp;	/*Zeitstempel*/
  unsigned char address;	/*Addresse*/
  int           temp;         	/*Temparatur*/
} auss1_data ;

/* Datenstruktur f�r den 'besseren' Au�ensensor */
typedef struct {
  time_t        timestamp;	/*Zeitstempel*/
  unsigned char address;	/*Addresse*/
  int           temp;           /*Temparatur*/
  unsigned int  hum;     	/*Luftfeuchte*/
} auss2_data;

/* Datenstruktur f�r den Regensensor */
typedef struct {
  time_t        timestamp;	/*Zeitstempel*/
  unsigned char address;	/*Addresse*/
  unsigned int  counter;	/*Z�hlerstand*/
} regen_data;

/* Datenstruktur f�r den Windsensor */
typedef struct {
  time_t        timestamp;	/*Zeitstempel*/
  unsigned char address;	/*Addresse*/
  unsigned int  geschw;		/*Windgeschwindigkeit*/
  unsigned int  schwank;	/*Schwankungsbreite*/
  unsigned int  richt;		/*Windrichtung*/
} winds_data;

/* Datenstruktur f�r den Innensensor */
typedef struct {
  time_t        timestamp;	/*Zeitstempel*/
  unsigned char address;	/*Addresse*/
  int           temp;		/*Temparatur*/
  unsigned int  hum;		/*Luftfeuchte*/
  unsigned int  press;		/*Luftdruck*/
} innen_data;

/* Datenstruktur f�r den Helligkeitssensor */
typedef struct {
  time_t        timestamp;	/*Zeitstempel*/
  unsigned char address;	/*Addresse*/
  unsigned int  rawlum;		/*'Roh-Helligkeit' - Helligkeit ohne Multiplikator*/
  unsigned int  mult;		/*Multiplikator*/
  unsigned int  abslum;		/*Absolute Helligkeit*/
} helli_data;

/* Datenstruktur f�r das Pyanometer */
typedef struct {
  time_t        timestamp;	/*Zeitstempel*/
  unsigned char address;	/*Addresse*/
  unsigned int  rawrad;		/*'Roh-Strahlung' - Strahlung ohne Multiplikator*/
  unsigned int  mult;    	/*Multiplikator*/
  unsigned int  absrad;  	/*Absolute Strahlung*/
} pyano_data;


/* Funktionen zum wegschreiben der Daten ----------------------------------- */

/* 'einfacher' Au�ensensor */
void write_auss1(auss1_data);

/* 'besserer' Au�ensensor */
void write_auss2(auss2_data);

/* Regensensor */
void write_regen(regen_data);

/* Windsensor */
void write_winds(winds_data);

/* Innensensor */
void write_innen(innen_data);

/* Helligkeitssensor */
void write_helli(helli_data);

/* Pyanometer */
void write_pyano(pyano_data);