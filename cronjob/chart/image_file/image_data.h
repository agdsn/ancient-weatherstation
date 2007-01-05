/*

   image_data.h        -- Part of Chart-generator for the weatherstation

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


/* Liste mit den Werten fuer das Diagram */
typedef struct pix_list *pix_list_ptr;
typedef struct pix_list {
  pix_list_ptr 	next;		/* naechstes Element */
  int 		x_pix_coord;	/* X - Koordinate */
  int 		y_pix_coord;	/* Y - Koordinate */
  int 		value_count;	/* Anzahl der Werte */
  int 		value_sum;	/* Summe der Werte */
  int 		no_line;
} pix_list_t;


/* Liste mit Labels */
typedef struct label_list *label_list_ptr;
typedef struct label_list {
  int  		 pos;		/* Position (X oder Y) */
  long 		 value;		/* Wert */
  char 		 *text;		/* Anzuzeigender Text */
  label_list_ptr next;		/* naechstes Element */
} label_list_t;



/* Holt die Liste mit den Daten fuer das Bild.
 * 1. Argument: die effektive Breite des Bereiches in dem gezeichnet werden soll
 * Rueckgabe: die liste mit den Werten 
 */
pix_list_ptr get_pix_list(int );

/* Baut die Durchschnittslinie.
 * 1. Argument: Die Daten der Linie mit den 'realen' Werten
 * 2. Argument: die effektive Breite des Bereiches in dem gezeichnet werden soll
 * Rueckgabe: die liste mit den Werten 
 */
pix_list_ptr build_average_line(pix_list_ptr real_list, int c_width);


/* Max. Wert */
pix_list_ptr get_min_elem();
double get_min_val();


/* Min. Wert */
pix_list_ptr get_max_elem();
double get_max_val();


/* Max. Zeit */ 
char *get_max_time();


/* Min. Zeit */
char *get_min_time();


/* Skaliert die X-Koordinaten der Punkte im angegebenem Bereich
 * 1. Argument: die Pix-Liste die skaliert werden soll
 * 2. Argument: die anzahl der Pixel in y-Richtung
 * Rueckgabe:   Position der nullinie. (wenn nicht sichtbar -1)
 */
int scale_y_coords(pix_list_ptr , int );


/* Baut die Liste mit den Labels an der X-Achse 
 * 1. Argument: Breite des bildes
 * Rueckgabe: List mit den Labels
 */
label_list_ptr get_x_label_list(int );


/* Baut die Liste mit den Labels an der Y-Achse
 * 1. Argument: Hoehe des Bildes
 * 2. Argument: Einrueckung oben und unten 
 * Rueckgabe: List mit den Labels
 * */
label_list_ptr get_y_label_list(int, int);
