/*

   config.c      -- Part of checksensor

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

/*
 * Wenn mehr kommentare benoetigt werden:
 * losinski@wh2.tu-dresden.de 
 * */


#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "config.h"
#include "definitions.h"
#include "checksensor.h"

#define CONFIG_BUFFERSIZE 2048

/* Funktionsdefinitionen */
static int read_int(const char *, void *);
static int read_yn(const char *, void *);
static int add_sens_id(const char *, void *);
static int add_address(const char *, void *);
static int read_str(const char *, void *);
 

/* Zuordnung zwischen Schlüsselwörtern in der Config, Der Funktion, die diese auswertet 
 * und dem eld in der Options-Struktur */
static const struct config_keyword keywords[] = {
  /* keyword		handler  	 	variable address			default */
  {"checkinterval",	read_int,  		&(global_opts.interval),		DEFAULT_CHECK_INTERVAL},
  {"min_sendings",	read_int,  		&(global_opts.sendings),		DEFAULT_MIN_SENDINGS},

  {"sensorid_from_db",	read_yn,	 	&(global_opts.id_from_db), 		"yes"},
  {"sensorid",		add_sens_id,	 	&(global_opts.sens_id_list), 		""},
  {"adminaddress",	add_address,	 	&(global_opts.address_list), 		""},

  {"mail_host",         read_str,               &(global_opts.mail_host),         	""},
  {"mail_port",         read_int,               &(global_opts.mail_port),         	""},
  {"mail_use_ssl",      read_yn,                &(global_opts.mail_ssl),         	""},
  {"mail_use_auth",     read_yn,                &(global_opts.mail_auth),         	""},
  {"mail_auth_user",    read_str,               &(global_opts.mail_auth_user),         	""},
  {"mail_auth_pass",    read_str,               &(global_opts.mail_auth_pass),         	""},
  
  {"pg_host",           read_str,               &(global_opts.pg_host),         	DEFAULT_PG_HOST},
  {"pg_user",           read_str,               &(global_opts.pg_user),         	DEFAULT_PG_USER},
  {"pg_pass",           read_str,               &(global_opts.pg_pass),         	DEFAULT_PG_PASS},
  {"pg_database",       read_str,               &(global_opts.pg_database),     	DEFAULT_PG_DATABASE},
  {"",			NULL, 	  		NULL,					""}
};


/* Implementierungen */


/* Liest eine Option (line) als String und speichert diese in 
 * dem entsprechendem Feld der Optionen-Struktur (arg) */
static int read_str(const char *line, void *arg){
	char **dest = arg;
	if (*dest) free(*dest);
	*dest = strdup(line);
	return 1;
}


/* Interval lesen, indem die Sensoren das letzte mal etwas gesendet haben sollen */
static int read_int(const char *line, void *arg){
  int *dest = arg;
  *dest = atoi(line);
}

/* lesen, ob id's aus der Datenbank gelesen werden sollen oder nicht */
static int read_yn(const char *line, void *arg){
  char *dest = arg;
  int retval = 1;
  if (!strcasecmp("yes", line))
    *dest = 1;
  else if (!strcasecmp("no", line))
    *dest = 0;
  else retval = 0;
  return retval;
}

/* fuegt eine id zur liste hinzu */
static int add_sens_id(const char *line, void *arg){
  sens_id_list_ptr id_new, id_temp;

  id_new = malloc(sizeof(sensor_id));
  id_new->next = NULL;
  id_new->id = atoi(line);

  DEBUGOUT2("add ID: %d\n", atoi(line));

  id_temp = *((sens_id_list_ptr*)arg);
  if (id_temp == NULL){
    id_temp                   = id_new;
    *((sens_id_list_ptr*)arg) = id_temp;
  } else {
    while (id_temp->next != NULL){
      id_temp = id_temp->next;
    }
    id_temp->next = id_new;
  }
  return 1;
}

/* furgt eine addresse zur liste hinzu */
static int add_address(const char *line, void *arg){
  mail_list_ptr adr_new, adr_temp;

  adr_new = malloc(sizeof(address_struct));
  adr_new->next = NULL;
  adr_new->mailbox = strdup(line);

  DEBUGOUT2("add addr: %s\n", line);

  adr_temp = *((mail_list_ptr*)arg);
  if (adr_temp == NULL){
    adr_temp               = adr_new;
    *((mail_list_ptr*)arg) = adr_temp;
  } else {
    while (adr_temp->next != NULL){
      adr_temp = adr_temp->next;
    }
    adr_temp->next = adr_new;
  }
  return 1;
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


  /* Optionen mit default-werten füllen */
  if(reset){
    for (i = 0; keywords[i].keyword[0]; i++)
    //printf("keyword: %s \n",keywords[i].keyword);
      if (keywords[i].def[0])
        keywords[i].handler(keywords[i].def, keywords[i].var);
  }

  /* config-file öffnen */
  if (!(in = fopen(file, "r"))) {
    DEBUGOUT2("Kann Config-File: %s nicht öffnen!\n", file);
    return 0;
  }

  /* Datei Zeilenweise lesen */
  while (fgets(buffer, CONFIG_BUFFERSIZE, in)) {
    lm++;

    /* Zeilenvorschübe gegen null-terminierungs-Zeichen ersetzen */
    if (strchr(buffer, '\n')) *(strchr(buffer, '\n')) = '\0';
  
    /* Originlzeile für eventuelle log-, bzw. debug-meldungen */
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


