/*

   common.h        -- Part of Chart-generator for the weatherstation

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

#include "definitions.h"

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

typedef void (*clean_func_t)(void *data);

typedef struct clean_struct *clean_struct_ptr;
typedef struct clean_struct {
  void 		   *data;
  clean_func_t 	   func;
  clean_struct_ptr next;
} clean_data;



extern config global_opts;

/* Funktionen -------------------------------------------------------------*/

/* Programm mit einem Fatalem Fehler beenden.
 * Argument: Fehlermeldung */
void exit_error(char*);

/* Signal-handler-Funktion zum beenden des Programmes */
void exit_sig_handler(int);

/* Eine (neuste) Clean-Funktion entfernen */
void remove_clean();

/* Eine Clean-Funktion hinzufügen */
void add_clean(clean_func_t , void *);

/* Alle clean-Funktionen entfernen */
void clear_clean();
