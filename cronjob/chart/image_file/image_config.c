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




/* Zuordnung zwischen Schlüsselwörtern in der Config, Der Funktion, die diese auswertet 
 * und dem eld in der Options-Struktur */
static const config_keyword keywords[] = {
  /* keyword		handler  	 	variable address			default */
  {"filename", 		read_str,               &(img_cfg.file_name),         		""},
  {"headline", 		read_str,               &(img_cfg.headline),         		""},
  {"gen_interval",    	read_time,          	&(img_cfg.gen_interval),         	""},
  {"show_interval",    	read_time,          	&(img_cfg.show_interval),         	""},
  {"label_interval",   	read_time,          	&(img_cfg.label_interval),         	""},
  {"label_sum",   	read_yn,          	&(img_cfg.label_sum),	         	""},
  {"width",		read_int,               &(img_cfg.width),    			""},
  {"height",		read_int,               &(img_cfg.height),    			""},
  {"sensor_id",		read_int,               &(img_cfg.sens_id),    			""},
  
  {"",			NULL, 	  		NULL,					""}
};




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

  DEBUGOUT2("Lese Config-File: '%s' \n", buff);

  ret_var = read_config(buff, 1, keywords);
  return ret_var;
}
