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

/* Zuordnung zwischen Schlüsselwörtern in der Config, Der Funktion, die diese auswertet 
 * und dem eld in der Options-Struktur */
static const config_keyword keywords[] = {
  /* keyword		handler  	 	variable address			default */
  {"filename", 		read_str,                &(img_cfg.img_name),         		""},
/*  {"image_cfg",    	add_image_cfg,          &(global_opts.image_cfg),         	""},
  {"image_cfg_location",read_str,               &(global_opts.image_cfg_location),    	DEFAULT_PG_HOST},
  */
  {"",			NULL, 	  		NULL,					""}
};


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
