/*

   weatherdeamon -- Weather Data Capture Program for the 
                    'ELV-PC-Wettersensor-Empf�nger'
   config.c      -- Part of the weatherdeamon

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

   $Id: process.c v 1.00 11 Aug 2006 losinski $
*/



#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "config.h"
#include "definitions.h"
#include "main.h"



static int set_interval(const char *, void *);
static int set_from_db_flag(const char *, void *);
static int add_sens_id(const char *, void *);
static int add_address(const char *, void *);
static int read_str(const char *, void *);
 

/* Zuordnung zwischen Schl�sselw�rtern in der Config, Der Funktion, die diese auswertet 
 * und dem eld in der Options-Struktur */
static const struct config_keyword keywords[] = {
  /* keyword		handler  	 	variable address			default */
  {"checkinterval",	set_interval,  		&(global_opts.interval),		DEFAULT_CHECK_INTERVAL},
  {"sensorid_from_db",	set_from_db_flag, 	&(global_opts.id_from_db), 		"yes"},
  {"sensorid",		add_sens_id,	 	&(global_opts.sens_id_list), 		NULL},
  {"adminaddress",	add_address,	 	&(global_opts.address_list), 		NULL},
  {"pg_host",           read_str,               &(global_opts.pg_host),         	DEFAULT_PG_HOST},
  {"pg_user",           read_str,               &(global_opts.pg_user),         	DEFAULT_PG_USER},
  {"pg_pass",           read_str,               &(global_opts.pg_pass),         	DEFAULT_PG_PASS},
  {"pg_database",       read_str,               &(global_opts.pg_database),     	DEFAULT_PG_DATABASE},
  {"",			NULL, 	  		NULL,					""}
};

/* Liest eine Option (line) als String und speichert diese in 
 * dem entsprechendem Feld der Optionen-Struktur (arg) */
static int read_str(const char *line, void *arg){
	char **dest = arg;
	if (*dest) free(*dest);
	*dest = strdup(line);
	return 1;
}


/* Interval lesen, indem die Sensoren das letzte mal etwas gesendet haben sollen */
static int set_interval(const char *line, void *arg){
  int *dest = arg;
  *dest = atoi(line);
}

/* lesen, ob id's aus der Datenbank gelesen werden sollen oder nicht */
static int set_from_db_flag(const char *line, void *arg){
  char *dest = arg;
  int retval = 1;
  if (!strcasecmp("yes", line))
    *dest = 1;
  else if (!strcasecmp("no", line))
    *dest = 0;
  else retval = 0;
  return retval;
}

static int add_sens_id(const char *line, void *arg){
  if (line == NULL){
    *((sens_id_list_ptr*)arg) = NULL;
    return 1;
  } else {
    sens_id_list_ptr id_new, id_temp;

    id_new = malloc(sizeof(sensor_id));
    id_new->next = NULL;
    id_new->id = atoi(line);

    id_temp = *((sens_id_list_ptr*)arg);
    if (id_temp == NULL){
      id_temp = id_new;
    } else {
      while (id_temp->next != NULL){
	id_temp = id_temp->next;
      }
      id_temp->next = id_new;
    }
    return 1;
  }
}


static int add_address(const char *line, void *arg){
  if (line == NULL){
    *((mail_list_ptr*)arg) = NULL;
    return 1;
  } else {
    mail_list_ptr adr_new, adr_temp;

    adr_new = malloc(sizeof(mail_address));
    adr_new->next = NULL;
    adr_new->address = strdup(line);

    adr_temp = *((mail_list_ptr*)arg);
    if (adr_temp == NULL){
      adr_temp = adr_new;
    } else {
      while (adr_temp->next != NULL){
	adr_temp = adr_temp->next;
      }
      adr_temp->next = adr_new;
    }
    return 1;
  }
}



/* Liest und verarbeitet die Config-File 
 * der Integer reset gibt an, ob zu begin defaultwerte
 * gesetzt werden sollen oder nicht */
int read_config(const char *file, int reset){
  FILE *in;						/* File-Handler */
  char buffer[CONFIG_BUFFERSIZE], *token, *line;	/* Puffer zum lesen der config-file (imer 1 Zeile)*/
  char orig[CONFIG_BUFFERSIZE];				/* Originalpuffer zum lesen der config-file, um die Log und debug-ausgabe zu realisieren */
  int i; 						/* Laufvariable */
  int lm = 0; 						/* Zeilen-Nummer */


  /* Optionen mit default-werten f�llen */
  if(reset){
    for (i = 0; keywords[i].keyword[0]; i++)
      if (keywords[i].def[0])
        keywords[i].handler(keywords[i].def, keywords[i].var);
  }

  /* config-file �ffnen */
  if (!(in = fopen(file, "r"))) {
    DEBUGOUT2("Kann Config-File: %s nicht �ffnen!\n", file);
    return 0;
  }

  /* Datei Zeilenweise lesen */
  while (fgets(buffer, CONFIG_BUFFERSIZE, in)) {
    lm++;

    /* Zeilenvorsch�be gegen null-terminierungs-Zeichen ersetzen */
    if (strchr(buffer, '\n')) *(strchr(buffer, '\n')) = '\0';
  
    /* Originlzeile f�r eventuelle log-, bzw. debug-meldungen */
    strcpy(orig, buffer);
  
    /* Kommentazeichen gegen null-terminierungs-Zeichen ersetzen und damit alles dahinter ignorieren */
    if (strchr(buffer, '#')) *(strchr(buffer, '#')) = '\0';


    if (!(token = strtok(buffer, " \t"))) continue;
    if (!(line = strtok(NULL, ""))) continue;

    /* eat leading whitespace */
    line = line + strspn(line, " \t=");
    /* eat trailing whitespace */
    for (i = strlen(line); i > 0 && isspace(line[i - 1]); i--);
    line[i] = '\0';

    for (i = 0; keywords[i].keyword[0]; i++){
      if (!strcasecmp(token, keywords[i].keyword)){
        if (!keywords[i].handler(line, keywords[i].var)) {
	  /* Fehler ins Logfile schreiben */
	  DEBUGOUT4("Kann '%s' nicht Parsen (%s:%d)!\n", orig, file, lm);
          /* reset back to the default value */
          keywords[i].handler(keywords[i].def, keywords[i].var);
        }
      }
    }
  }
  fclose(in);
  return 1;
}

