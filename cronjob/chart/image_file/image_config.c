/*

   chart.c        -- Part of Chart-generator for the weatherstation

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
#include "../common.h"
#include "../config.h"
#include "image_common.h"

#define BUFFSIZE 512

static int read_time(const char *, void *);
static int read_color(const char *, void *);
static int read_double(const char *, void *);
static int read_fmt_str(const char *, void *);



/* Zuordnung zwischen Schlüsselwörtern in der Config, Der Funktion, die diese auswertet 
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
  {"label_sum",   		read_yn,          	&(img_cfg.label_sum),	        	DEFAULT_LABEL_SUM},
  {"width",			read_int,               &(img_cfg.width),    			DEFAULT_WIDTH},
  {"height",			read_int,               &(img_cfg.height),    			DEFAULT_HEIGHT},
  {"unit",			read_str,               &(img_cfg.unit),    			DEFAULT_UNIT},
  {"sensor_id",			read_int,               &(img_cfg.sens_id),    			DEFAULT_SENS_ID},
  {"zero_min",			read_yn,               &(img_cfg.zero_min),    		DEFAULT_ZERO_MIN},
  {"as_bars",			read_yn,               &(img_cfg.bars),	    		DEFAULT_AS_BARS},
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

static int read_fmt_str(const char *line, void *arg){
  char **dest = arg;
  char *new_line = NULL;
  char *temp = malloc(sizeof(char)*BUFFSIZE);
  char *p;  

  read_str(line, &new_line);

  if(new_line != NULL){
    p = strchr(new_line, '|');
    if(p != NULL){
      strcpy(p, "\0");
      strcpy(temp, new_line);
      strcat(temp, "\r\n");
      strcat(temp, p+1);
    } else {
      strcpy(temp, new_line);
    }

    if (*dest) free(*dest);
    *dest = strdup(temp);
    free(new_line);
  }
  free(temp);

  return 1;
  
}


static int read_double(const char *line, void *arg){
  double *dest = arg;
  *dest = atof(line);
  return 1;
}

static int read_color(const char *line, void *arg){
  img_color_ptr *col = arg;
  img_color_ptr tmp = NULL;
  char *buff = malloc(sizeof(char)*3);

  if(strlen(line) == 11){
    if (strchr(line, ':') != NULL){
      tmp        = malloc(sizeof(img_color_t));
      tmp->r     = strtol(strncpy(buff, line, 2), NULL, 16);
      tmp->b     = strtol(strncpy(buff, line+3, 2), NULL, 16);
      tmp->g     = strtol(strncpy(buff, line+6, 2), NULL, 16);
      tmp->alpha = strtol(strncpy(buff, line+9, 2), NULL, 16);

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
  
  p = strchr(tmp, '\0');
  if(p != NULL){
    switch( *(p - 1) ){
      case 's':
        mult = 1;
	break;
      case 'm':
	mult = 60;
	break;
      case 'h':
	mult = 3600;
	break;
      case 'd':
	mult = 86400;
	break;
      case 'y':
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


/* Liest ein Bild-Configfile */
int get_image_cfg(char *file){
  int ret_var; 
  char *buff; 

  buff = malloc(sizeof(char)*(strlen(file)+strlen(global_opts.image_cfg_location)+1));
  buff = strcpy(buff, global_opts.image_cfg_location);
  buff = strcat(buff, file);

  DEBUGOUT2("\nLese Config-File: '%s' \n", buff);

  ret_var = read_config(buff, 1, keywords);
  return ret_var;
}
