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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include "definitions.h"
#include "config.h"
#include "chart.h"
#include "common.h"
#include "image_file/image_file.h"

static int walk_image_cfg_list();				/* Definition fuer walk_image_cfg_list */
static void wait_for_childs();					/* Definition fuer wait_for_childs */


config global_opts;						/* Globale Optionen */


/* Geht die Bilder-Configs durch und startet das 
 * Generieren der Bilder 
 * Rueckgabe: Anzahl der Childs die Geforkt wurden */
static int walk_image_cfg_list(){
  int has_forked             = 0;			/* Zaehler der Childs */ 
  pid_t pid                  = 0;			/* Temporaer abgespeicherte Pid */
  image_cfg_list_ptr tmp_ptr = global_opts.image_cfg;	/* Hilfszeiger auf das aktuelle element der Config-Liste */

  for(; tmp_ptr; tmp_ptr = tmp_ptr->next){		/* Configliste durchgehen */
    if(global_opts.fork){				/* Wenn geforkt werden soll */
      if((pid = fork()) == 0){				/* Unterscheidung ob im Child oder im Parent */
        clear_clean();					/* Wenn im Child, dann die Clean-Liste leeren */
	process_image_cfg(tmp_ptr->image_cfg_file);	/* Bild generieren */
	exit(EXIT_SUCCESS);				/* und Child beenden */
      } 
      else if (pid == -1){				/* Probleme beim Fork */
	exit_error(ERROR_FORK);
      } 
      if(!has_forked){					/* beim ersten durchlauf die warte-funktion an die Clean-Liste anhaengen */
        add_clean(wait_for_childs, NULL);
      }
      has_forked++;					/* Den fork-zaehler erhoehen */
      DEBUGOUT2("Prozess %d angelegt\n",pid);
    } else {						/* Wenn nicht geforkt werden soll */
      process_image_cfg(tmp_ptr->image_cfg_file);	/* dann so das Bild generieren */
    }
  }
  return has_forked;
}


/* Wartet auf gestartete Child-Prozesse */
static void wait_for_childs(void *dummy){
  int ret_val;
  pid_t pid;
  while((pid = wait(&ret_val)) != -1){
    DEBUGOUT2("Prozess %d beendet\n", pid);
  }
}


/* Main - Funktion */
int main(int argc, char *argv[]){

  DEBUGOUT1("Programm gestartet\n");

  if(signal(SIGABRT, exit_sig_handler) == SIG_ERR)
    exit_error(ERROR_SEIINST);
  DEBUGOUT1("Signalhandler zum beenden per SIGABRT installiert\n");
  if(signal(SIGINT, exit_sig_handler) == SIG_ERR)
    exit_error(ERROR_SEIINST);
  DEBUGOUT1("Signalhandler zum beenden per SIGINT installiert\n");
  if(signal(SIGQUIT, exit_sig_handler) == SIG_ERR)
    exit_error(ERROR_SEIINST);
  DEBUGOUT1("Signalhandler zum beenden per SIGQUIT installiert\n");
  if(signal(SIGTERM, exit_sig_handler) == SIG_ERR)
    exit_error(ERROR_SEIINST);
  DEBUGOUT1("Signalhandler zum beenden per SIGTERM installiert\n");

  /* Haupt-Config einlesen */
  read_config(DEFAULT_CONFIG_FILE, 1, NULL);					

  /* Debug-Ausgaben, um zu sehen, ob die Optionen richtig gesetzt werden */
  DEBUGOUT1("\nOptionen:\n");
  DEBUGOUT2("  cfg_location = %s\n", global_opts.image_cfg_location);
  DEBUGOUT2("  fork         = %i\n", global_opts.fork);

  DEBUGOUT1("\nPostgres:\n");
  DEBUGOUT2("  Host:     =  %s\n",global_opts.pg_host);
  DEBUGOUT2("  User:     =  %s\n",global_opts.pg_user);
  DEBUGOUT2("  Pass:     =  %s\n",global_opts.pg_pass);
  DEBUGOUT2("  Datenbank =  %s\n",global_opts.pg_database);
  DEBUGOUT2("  Timeout   =  %s\n",global_opts.pg_database);

  /* Bilder - Configs durchgehen */
  if(walk_image_cfg_list())
    wait_for_childs(NULL);

  DEBUGOUT1("\n-------------------------------------------------------------------\nEnde\n");

  return EXIT_SUCCESS;
}


