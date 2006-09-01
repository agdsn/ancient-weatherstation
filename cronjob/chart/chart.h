/*

   chart.h        -- Part of Chart-generator for the weatherstation

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

typedef struct image_cfg_list_t *image_cfg_list_ptr;
typedef struct image_cfg_list_t {
  char 		 	*image_cfg_file;
  image_cfg_list_ptr  	next;
} image_cfg_list;

typedef struct config_t {
  char 			*pg_host;
  char 			*pg_database;
  char 			*pg_user;
  char 			*pg_pass;
  char 			*image_cfg_location;
  image_cfg_list_ptr 	image_cfg;
  int 			fork;
} config;

extern config global_opts;


/* Funktionen -------------------------------------------------------------*/

/* Programm mit einem Fatalem Fehler beenden.
 * Argument: Fehlermeldung */
void exit_error(char*);
