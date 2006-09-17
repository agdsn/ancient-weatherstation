/*

   image_common.h        -- Part of Chart-generator for the weatherstation

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

/* Repraesentiert eine Farbe */
typedef struct img_color *img_color_ptr; 
typedef struct img_color {
  int r;		/* Rotanteil */
  int g;		/* Gruenanteil */ 
  int b;		/* Blauanteil */
  int alpha;		/* Alphawert */
} img_color_t;


/* Config eines Bildes */
typedef struct image_cfg {
  char	 *file_name;		/* Filename des Bildes */
  char	 *headline;		/* Ueberschrift */
  char   *table_field;		/* Feldname in der Tabelle */
  int 	 manual_table;		/* Tabelle manuell angeben */
  char   *table_name;		/* manueller Tabellenname */
  long 	 gen_interval;		/* Generierungsinterval */
  long 	 show_interval;		/* Anzeigeinterval */
  long 	 label_interval;	/* Label - Interval */
  int 	 label_sum;		/* Sollen Werte aufsummiert statt gemittelt werden */
  int 	 width;			/* Breite */
  int 	 height;		/* Hoehe */
  char   *unit;			/* Einheit, die an die Skala angehangen werden soll */
  char   *x_desc;		/* Beschreibung der X - Achse */
  char   *y_desc;		/* Beschreibung der Y - Achse */
  int  	 sens_id;		/* Id des Sensors */
  double val_koeff;		/* Faktir mit dem die Werte multipliziert werden sollen */
  char   *x_fmt_extra;		/* Formatstring fuer die erste und letzte Zeit */
  char   *x_fmt;		/* Formatstring fuer die restlichen Labels */
  int    zero_min;		/* Nullinie auf jeden Fall anzeigen */
  int    bars;			/* Balken malen statt einer Linie */
  int 	 dflt_dir;		/* Standart - Verzeichnis fuer die Bilder */

  /* Farben */
  img_color_ptr bg_color;
  img_color_ptr dia_bg_color;
  img_color_ptr dia_line_color;
  img_color_ptr dia_grid_x_color;
  img_color_ptr dia_grid_y_color;
  img_color_ptr dia_border_color;
  img_color_ptr zero_line_color;
  img_color_ptr headline_color;
  img_color_ptr label_x_color;
  img_color_ptr label_extra_x_color;
  img_color_ptr label_y_color;
  img_color_ptr desc_x_color;
  img_color_ptr desc_y_color;
} image_cfg_t;

/* Entfaelt die aktuelle Bild - Config */
extern image_cfg_t img_cfg;
