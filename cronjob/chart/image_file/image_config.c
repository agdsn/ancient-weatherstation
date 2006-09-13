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

static int read_time(const char *, void *);
static int read_color(const char *, void *);
static int read_double(const char *, void *);



/* Zuordnung zwischen Schlüsselwörtern in der Config, Der Funktion, die diese auswertet 
 * und dem eld in der Options-Struktur */
static const config_keyword keywords[] = {
  /* keyword		handler  	 	variable address			default */
  {"filename", 		read_str,               &(img_cfg.file_name),         		""},
  {"headline", 		read_str,               &(img_cfg.headline),         		""},
  {"table_field", 	read_str,               &(img_cfg.table_field),        		""},
  {"manual_table",   	read_yn,          	&(img_cfg.manual_table),         	"no"},
  {"manual_table_name", read_str,               &(img_cfg.table_name),        		""},
  {"gen_interval",    	read_time,          	&(img_cfg.gen_interval),         	""},
  {"show_interval",    	read_time,          	&(img_cfg.show_interval),         	""},
  {"label_interval",   	read_time,          	&(img_cfg.label_interval),         	""},
  {"label_sum",   	read_yn,          	&(img_cfg.label_sum),	         	""},
  {"width",		read_int,               &(img_cfg.width),    			""},
  {"height",		read_int,               &(img_cfg.height),    			""},
  {"sensor_id",		read_int,               &(img_cfg.sens_id),    			""},
  {"value_koeffizient",	read_double,            &(img_cfg.val_koeff),  			""},
  

  {"bg_color",		read_color,             &(img_cfg.bg_color),   			""},
  {"dia_bg_color",	read_color,             &(img_cfg.dia_bg_color),		""},
  {"dia_line_color",	read_color,             &(img_cfg.dia_line_color),		""},
  {"dia_grid_color",	read_color,             &(img_cfg.dia_grid_color),		""},
  {"dia_border_color",	read_color,             &(img_cfg.dia_border_color),		""},
  {"zero_line_color",	read_color,             &(img_cfg.zero_line_color),   		""},

  {"",			NULL, 	  		NULL,					""}
};


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
  char *p   = NULL;
  
  p = strchr(line, '\0');
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

  *dest = atol(line) * mult;
  
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
