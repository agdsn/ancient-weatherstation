/*

   weatherdeamon -- Weather Data Capture Program for the 
                    'ELV-PC-Wettersensor-Empfänger'
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


static int read_str(const char *, void *);
static read_log_data_flag(const char *, void *);
static read_log_error_flag(const char *, void *);
static read_foreground_flag(const char *, void *);
static read_verbose_flag(const char *, void *);
static int flag_handler(int *, int , const char *);


/* Zuordnung zwischen Schlüsselwörtern in der Config, Der Funktion, die diese auswertet 
 * und dem eld in der Options-Struktur */
static const struct config_keyword keywords[] = {
  /* keyword		handler  	 	variable address		default */
  {"interface",		read_str,  		&(global_opts.device),		DEFAULT_SERIAL_PORT},
  #ifndef NO_LOGING
  {"log_data",		read_log_data_flag, 	&(global_opts.flags), 		"no"},
  {"log_error",		read_log_error_flag, 	&(global_opts.flags), 		"no"},
  {"data_file",		read_str,  		&(global_opts.data_log), 	DEFAULT_DATA_FILE},
  {"error_file",	read_str, 		&(global_opts.error_log), 	DEFAULT_ERROR_FILE},
  #endif
  {"foreground",	read_foreground_flag, 	&(global_opts.flags), 		"no"},
  {"verbose",		read_verbose_flag, 	&(global_opts.flags), 		"no"},
  {"pg_host",		read_str,	 	&(global_opts.pg_host), 	DEFAULT_PG_HOST},
  {"pg_user",		read_str,  		&(global_opts.pg_user), 	DEFAULT_PG_USER},
  {"pg_pass",		read_str, 		&(global_opts.pg_pass), 	DEFAULT_PG_PASS},
  {"pg_database", 	read_str, 		&(global_opts.pg_database),	DEFAULT_PG_DATABASE},
  {"",			NULL, 	  		NULL,				""}
};

/* Liest eine Option (line) als String und speichert diese in 
 * dem entsprechendem Feld der Optionen-Struktur (arg) */
static int read_str(const char *line, void *arg){
	char **dest = arg;
	if (*dest) free(*dest);
	*dest = strdup(line);

	return 1;
}


/* Die 4 Funktionen reichen jeweils nur die Daten an den 
 * flag_handler durch und teilen diesem noch das Flag
 * mit, was er setzen soll (oder nicht) */
#ifndef NO_LOGING
static read_log_data_flag(const char *line, void *arg){
  return flag_handler(arg, LOG_DATA_FLAG, line);
}

static read_log_error_flag(const char *line, void *arg){
  return flag_handler(arg, LOG_ERROR_FLAG, line);
}
#endif

static read_foreground_flag(const char *line, void *arg){
  return flag_handler(arg, FOREGROUND_FLAG, line);
}

static read_verbose_flag(const char *line, void *arg){
  return flag_handler(arg, VERBOSE_FLAG, line);
}

/* Setzt Flags wenn der String gleich 'yes' ist */
static int flag_handler(int *dest, int flag, const char *line){
  if (strcmp("yes", line) == 0){
    *dest |= flag;
  } else {
    *dest &= ~flag;
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
      if (keywords[i].def[0])
        keywords[i].handler(keywords[i].def, keywords[i].var);
  }

  /* config-file öffnen */
  if (!(in = fopen(file, "r"))) {
    #ifndef NO_LOGING
    if(get_flag(LOG_ERROR_FLAG)){
      sprintf(get_error_buffer(), "Kann Config-File: %s nicht öffnen!",file);
      log_error(get_error_buffer());
    }
    #endif
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
          #ifndef NO_LOGING 
          if(get_flag(LOG_ERROR_FLAG)){
	    sprintf(get_error_buffer(), "Fehler beim Parsen des Parameters '%s' im file %s, Zeile %d",orig, file, lm);
            log_error(get_error_buffer());
          }
	  #endif
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


