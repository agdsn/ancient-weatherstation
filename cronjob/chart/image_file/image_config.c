/*

   image_config.c        -- Part of Chart-generator for the weatherstation

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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../common.h"
#include "../config.h"
#include "image_common.h"

#define BUFFSIZE 512


/* Funktionsdefinitionen */
static int read_time(const char *, void *);
static int read_color(const char *, void *);
static int read_double(const char *, void *);
static int read_fmt_str(const char *, void *);
static int read_date(const char *, void *);


/* Zuordnung zwischen Schluesselwoertern in der Config, Der Funktion, die diese auswertet 
 * und dem eld in der Options-Struktur */
static const config_keyword keywords[] = {
  /* keyword			handler  	 	variable address			default */
  {"filename", 			read_str,               &(img_cfg.file_name),         		""},
  {"use_dflt_img_dir", 		read_yn,          	&(img_cfg.dflt_dir),	        	DEFAULT_LABEL_SUM},
  {"headline", 			read_str,               &(img_cfg.headline),         		DEFAULT_HEADLINE},
  {"table_field", 		read_str,               &(img_cfg.table_field),        		DEFAULT_TABLE_FIELD},
  {"manual_table",   		read_yn,          	&(img_cfg.manual_table),        	DEFAULT_MANUAL_TABLE},
  {"manual_table_name", 	read_str,               &(img_cfg.table_name),        		""},
  {"gen_interval",    		read_time,          	&(img_cfg.gen_interval),        	DEFAULT_GEN_INTERVAL},
  {"show_interval",    		read_time,          	&(img_cfg.show_interval),       	DEFAULT_SHOW_INTERVAL},
  {"label_interval",   		read_time,          	&(img_cfg.label_interval),      	DEFAULT_LABEL_INTERVAL},
  {"fixed_timepoint",  		read_yn,          	&(img_cfg.fix_timepoint),	       	DEFAULT_FIX_TIMEPOINT},
  {"timepoint",   		read_date,          	&(img_cfg.timepoint),		       	""},
  {"label_sum",   		read_yn,          	&(img_cfg.label_sum),	        	DEFAULT_LABEL_SUM},
  {"width",			read_int,               &(img_cfg.width),    			DEFAULT_WIDTH},
  {"height",			read_int,               &(img_cfg.height),    			DEFAULT_HEIGHT},
  {"unit",			read_str,               &(img_cfg.unit),    			DEFAULT_UNIT},
  {"sensor_id",			read_int,               &(img_cfg.sens_id),    			DEFAULT_SENS_ID},
  {"zero_min",			read_yn,                &(img_cfg.zero_min),    		DEFAULT_ZERO_MIN},
  {"as_bars",			read_yn,                &(img_cfg.bars),	    		DEFAULT_AS_BARS},
  {"x_axis_desc",		read_str,               &(img_cfg.x_desc),    			DEFAULT_X_AX_DESC},
  {"y_axis_desc",		read_str,               &(img_cfg.y_desc),    			DEFAULT_Y_AX_DESC},
  {"x_format",			read_fmt_str,           &(img_cfg.x_fmt),    			DEFAULT_X_FORMAT},
  {"x_format_extra",		read_fmt_str,           &(img_cfg.x_fmt_extra),			DEFAULT_X_FORMAT_EXTRA},
  {"value_koeffizient",		read_double,            &(img_cfg.val_koeff),  			DEFAULT_VAL_KOEFF},
  	

  {"bg_color",			read_color,             &(img_cfg.bg_color),   			DEFAULT_BG_COLOR},
  {"dia_bg_color",		read_color,             &(img_cfg.dia_bg_color),		DEFAULT_DIA_BG_COLOR},
  {"dia_line_color",		read_color,             &(img_cfg.dia_line_color),		DEFAULT_DIA_LINE_COLOR},
  {"dia_grid_x_color",		read_color,             &(img_cfg.dia_grid_x_color),		DEFAULT_DIA_GRID_X_COLOR},
  {"dia_grid_y_color",		read_color,             &(img_cfg.dia_grid_y_color),		DEFAULT_DIA_GRID_Y_COLOR},
  {"dia_border_color",		read_color,             &(img_cfg.dia_border_color),		DEFAULT_DIA_BORDER_COLOR},
  {"zero_line_color",		read_color,             &(img_cfg.zero_line_color),   		DEFAULT_ZERO_LINE_COLOR},
  {"headline_color",		read_color,             &(img_cfg.headline_color),   		DEFAULT_HEADLINE_COLOR},
  {"label_x_color",		read_color,             &(img_cfg.label_x_color),   		DEFAULT_LABEL_X_COLOR},
  {"label_extra_x_color",	read_color,             &(img_cfg.label_extra_x_color),  	DEFAULT_LABEL_EXTRA_X_COLOR},
  {"label_y_color",		read_color,             &(img_cfg.label_y_color),   		DEFAULT_LABEL_Y_COLOR},
  {"descr_y_color",		read_color,             &(img_cfg.desc_y_color),   		DEFAULT_DESCR_X_COLOR},
  {"descr_x_color",		read_color,             &(img_cfg.desc_x_color),   		DEFAULT_DESCR_Y_COLOR},

  {"",				NULL, 	  		NULL,					""}
};


/* Einen Zeit - Format - String lesen */
static int read_fmt_str(const char *line, void *arg){
  char **dest = arg;
  char *new_line = NULL;
  char *temp = malloc(sizeof(char)*BUFFSIZE);
  char *p;  

  read_str(line, &new_line);

  if(new_line != NULL){
    p = strchr(new_line, '|'); 
    if(p != NULL){				/* Wenn ein | gefunden wurde */	
      strcpy(p, "\0");				/* dann diesen durch ein Stringende ersetzen */
      strcpy(temp, new_line);			/* Das Srueck vor dem | in einen Puffer schreiben */
      strcat(temp, "\r\n");			/* ein zeilenumbruch mit Wagenruecklauf einfuegen */
      strcat(temp, p+1);			/* das stueck hinter dem | anfuegen */
    } else {
      strcpy(temp, new_line);			/* Wenn kein | gefunden, dann den ganzen String in den Puffer */
    }

    if (*dest) free(*dest);
    *dest = strdup(temp);
    free(new_line);
  }
  free(temp);

  return 1;
  
}


/* Liest ein double ein */
static int read_double(const char *line, void *arg){
  double *dest = arg;
  *dest = atof(line);
  return 1;
}


/* Liest eine Farbe ein 
 * Format: rr:gg:bb:aa */
static int read_color(const char *line, void *arg){
  img_color_ptr *col = arg;
  img_color_ptr tmp = NULL;
  char *buff = malloc(sizeof(char)*3);

  if(strlen(line) == 11){						/* Wenn String auch wirkle 11 Zeichen lang */
    if (strchr(line, ':') != NULL){					/* und min. 1 : vorkommt */ 
      strcpy(buff, "00\0");
      tmp        = malloc(sizeof(img_color_t));				/* Neues Farbelement allocieren */
      tmp->r     = strtol(strncpy(buff, line,   2), NULL, 16);		/* r */
      tmp->g     = strtol(strncpy(buff, line+3, 2), NULL, 16);		/* g */
      tmp->b     = strtol(strncpy(buff, line+6, 2), NULL, 16);		/* b */
      tmp->alpha = strtol(strncpy(buff, line+9, 2), NULL, 16);		/* alpha */

      DEBUGOUT5(" Farbe gelesen: rot:%2x gelb:%2x gruen:%2x mit alpha:%2x\n", tmp->r, tmp->b, tmp->g, tmp->alpha) ;
    }
  } 

  *col = tmp;

  free(buff);

  return 1; 
}

/* Ein Interval einlesen.
 * wandelt Zeitangaben von der Form:
 * 10s 10m 10h 10d 10y
 * in Sekunden-Werte um */
static int read_time(const char *line, void *arg){
  long *dest = arg;
  long mult  = 0;
  char *p    = NULL;
  char *tmp  = strdup(line);
  
  p = strchr(tmp, '\0');	/* P auf das ende des Strings setzen */
  if(p != NULL){
    switch( *(p - 1) ){		/* Letztes Zeichen pruefen */
      case 's':			/* Sekunden */
        mult = 1;
	break;
      case 'm':			/* Minuten */
	mult = 60;
	break;
      case 'h':			/* Stunden */
	mult = 3600;
	break;
      case 'd':			/* Tage */
	mult = 86400;
	break;
      case 'y':			/* Jahre */
	mult = 31536000;
	break;
      default:
	break;
    }
    if( mult ){
      strcpy(p - 1, "\0");
    } else {
      mult = 1;
    }
  }

  *dest = atol(tmp) * mult;
  
  return 1;
}

/* Liest das Datum, an dem die Bilder-generierung 
 * 'festgemacht' werden soll.
 * Format: yyyy-mm-dd-hh-mm */
static int read_date(const char *line, void *arg){
  struct tm timestruct;
  long *dest = arg;
  char *buff = malloc(sizeof(char)*5);
  time_t temp = 0;

  if(line == NULL){
    *dest = 0;
    free(buff);
    return 1;
  }

  if(strlen(line) != 16){
    DEBUGOUT2(" Invalid Date-String '%s'!\n", line);
    *dest = 0;
    free(buff);
    return 1;
  } else {
    timestruct.tm_sec = 0;
    timestruct.tm_isdst = -1;
    strcpy(buff, "0000\0");
    timestruct.tm_year = (atol(strncpy(buff, line,   4)) - 1901);
    strcpy(buff, "00\0");
    timestruct.tm_mon  = atol(strncpy(buff, line+5, 2));
    timestruct.tm_mday = atol(strncpy(buff, line+8, 2));
    timestruct.tm_hour = atol(strncpy(buff, line+11, 2));
    timestruct.tm_min  = atol(strncpy(buff, line+14, 2));

    temp = mktime(&timestruct);

    DEBUGOUT2(" Datum gelesen: %s\n", ctime(&temp));
  }
 
  *dest = temp;

  free(buff);
  return 1;
}

/* Liest ein Bild-Configfile */
int get_image_cfg(char *file){
  int ret_var; 
  char *buff; 

  /* Config - URL zusammenbauen */
  buff = malloc(sizeof(char)*(strlen(file)+strlen(global_opts.image_cfg_location)+1));
  buff = strcpy(buff, global_opts.image_cfg_location);
  buff = strcat(buff, file);

  DEBUGOUT2("\nLese Config-File: '%s' \n", buff);
  
  /* File einlesen */
  ret_var = read_config(buff, 1, keywords);
  
  return ret_var;
}
