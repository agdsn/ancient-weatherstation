/*

   weatherdeamon -- Weather Data Capture Program for the 
                    'ELV-PC-Wettersensor-Empfaenger'
   main.c        -- Part of the weatherdeamon

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


#include <stdlib.h>         	/* EXIT_SUCCESS */
#include <errno.h>
#include <unistd.h>         	/* STDOUT_FILENO */
#include <stdio.h>
#include <fcntl.h>		/* O_WRONLY O_APPEND O_CREAT ... */
#include <string.h>
#include <signal.h>
#include "definitions.h"
#include "config.h"
#include "main.h"
#include "input.h"

/* Variablen ---------------------------------------------------------- */
w_opts global_opts;
clean_struct_ptr clean_ptr  = NULL;
#ifndef NO_LOGING
  static char *error_buffer = NULL;	/* Puffer fuer Fehler-ausgaben */
#endif

/* Funktionen ----------------------------------------------------------*/
static void background();
static void write_help();
static void merge_options(w_opts *);
#ifndef NO_LOGING
  static void log_message(char *, time_t, char *);
  static int close_logfile(int);
  static int open_logfile(char *);
#endif
static clean_struct_ptr get_last_clean(clean_struct_ptr);
static void clean();
static void exit_sig_handler(int); 

/* Jetzt gehts los.... --------------------------------------------------*/

/* Main-Funktion */
int main(int argc, char *argv[]){
  char *extra_conf_file = NULL;
  char c;
  w_opts opts;	/* temp. Optionen */

  /* errno 0 setzen */
  errno = 0;

  /* temp. opts initialisieren (0 setzen) */
  opts.flags = 0;
  opts.device = NULL;
  #ifndef NO_LOGING
  opts.data_log = NULL;
  opts.error_log = NULL;
  #endif

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

  read_config(DEFAULT_CONFIG_FILE,1);

  /* Optionen auswerten */  
  while ((c = getopt (argc, argv, "vfc:e:d:i:h")) != -1)
    switch (c) {
      #ifndef NO_LOGING
      case 'e':
	opts.flags |= LOG_ERROR_FLAG;
        opts.error_log = optarg;
        break;
      case 'd':
	opts.flags |= LOG_DATA_FLAG;
        opts.data_log = optarg;
        break;
      #endif
      case 'c':
        extra_conf_file = optarg;
	break;
      case 'v':
        opts.flags |= VERBOSE_FLAG;
        break;
      case 'f':
        opts.flags |= FOREGROUND_FLAG;
        break;
      case 'i':
        opts.device = optarg;
        break;
      case 'h':
        write_help();
        return 0;
      case '?':
        write_help();
        return 1;
    }
  
  /* eventuelle 2. config-file (mit -c angegeben) auswerten */
  if(extra_conf_file != NULL){
    read_config(extra_conf_file,0);
  }

  /* Komandozeilen-Optionen zu denen aus der Config-File hinzufuegen */
  merge_options(&opts);

  /* Debug-Ausgaben, um zu sehen, ob die Optionen richtig gesetzt werden */
  DEBUGOUT1("\nOptionen:\n");
  DEBUGOUT2("  Device     = %s\n", global_opts.device);
  DEBUGOUT2("  Foreground:  %i\n", get_flag(FOREGROUND_FLAG));
  DEBUGOUT2("  Verbose:     %i\n", get_flag(VERBOSE_FLAG));
  DEBUGOUT2("  LogData:     %i\n", get_flag(LOG_DATA_FLAG));
  DEBUGOUT2("   DataFile  = %s\n", global_opts.data_log);
  DEBUGOUT2("  LogError:    %i\n", get_flag(LOG_ERROR_FLAG));
  DEBUGOUT2("   ErrorFile = %s\n", global_opts.error_log);

  DEBUGOUT1("\nPostgres:\n");
  DEBUGOUT2("  Host:     =  %s\n",global_opts.pg_host);
  DEBUGOUT2("  User:     =  %s\n",global_opts.pg_user);
  DEBUGOUT2("  Pass:     =  %s\n",global_opts.pg_pass);
  DEBUGOUT2("  Datenbank =  %s\n",global_opts.pg_database);


  /* Programm in den Hintergrund schicken ? */
  background();
while (1);
  DEBUGOUT1("\nBeginne Interface zu oeffnen\n\n");

  /* Port Pollen */
  read_port();

  /* Theoretisch sollte das Programm hier _nie_ hinkommen */
  clean();
  return EXIT_SUCCESS;
}

/* Diese Funktion ist dazu da, das Programm in den Hintergrund zu Forken */
static void background(){
  if (!get_flag(FOREGROUND_FLAG)){ 
    DEBUGOUT1("Forke in den Hintergrund");
    if (daemon(0, get_flag(VERBOSE_FLAG)) == -1) {
      exit_error(ERROR_FORK);
    }
  }
}

/* Fuehrt die Optionen aus der Komandozeile und die aus dem Conf-File zusammen */
static void merge_options(w_opts *priv){
  if((*priv).device != NULL)
    global_opts.device = (*priv).device;
  #ifndef NO_LOGING
  if((*priv).data_log != NULL)
    global_opts.data_log = (*priv).data_log;
  if((*priv).error_log != NULL)
    global_opts.error_log = (*priv).error_log;
  #endif
  global_opts.flags |= (*priv).flags;
}

/* Bisschen Hilfe ausgeben */
static void write_help(){
  printf("Usage:\n");
  #ifndef NO_LOGING
    printf("weatherdeamon [-f] [-v] [-i <interface>] [-c <Conf-File>] [-d <Data-logfile>] [-e <Error-Logfile>]\n");
  #else 
    printf("weatherdeamon [-i <interface>]\n");
  #endif  
  printf("\t-i <interface>\n\t\tThe interface where the reciver is connected \n\t\t(default:/dev/ttyS0)\n");
  printf("\t-f\n\t\tDon't Fork to background\n");
  printf("\t-v\n\t\tVerbose, log errors to stdout, even it's forked to background\n");
  printf("\t-c <Conf-File>\n\t\t tells the Program to use an aditional config-file\n");
  #ifndef NO_LOGING
    printf("\t-d <Data-logfile>\n\t\tLog all Data to the given Logfile\n\t\t(default: no logging!)\n");
    printf("\t-e <Error-Logfile>\n\t\tLog all non-critical-Errors to the given Logfile\n\t\t(default: no logging!)\n");
  #endif
}

/* ein Flag lesen */
int get_flag(int mask){
  return (global_opts.flags & mask) != 0 ; 
}

/* Diese Funktion beendet das Programm mit einer Fehlermeldung. */
void exit_error(char* err){
  DEBUGOUT1("\nEtwas unschoenes ist passiert\n");
  clean();
  if(errno != 0){
    perror("Fehler");  
  }
  write(STDOUT_FILENO, err, strlen(err));
  exit(1);
}

/* Wird bei Beendigungssignalen ausgefuehrt */
static void exit_sig_handler(int signr){
  #ifdef DEBUG
  DEBUGOUT1("\n");
  switch (signr){
    case SIGABRT:
      DEBUGOUT1("SIGABRT Interupt erhalten!\n");
    case SIGTERM:
      DEBUGOUT1("SIGTERM Interupt erhalten!\n");
    case SIGQUIT:
      DEBUGOUT1("SIGQUIT Interupt erhalten!\n");
    case SIGINT:
      DEBUGOUT1("SIGINT Interupt erhalten!\n");
  }
  #endif
  clean();
  DEBUGOUT1("Beende Programm...\n");
  exit(0);
}

/* Funktionen fuer die Logfiles */

#ifndef NO_LOGING
/* Schreiben einer Fehlermeldung in das fehler-Logfile */
void log_error(char *msg){
  if(get_flag(LOG_ERROR_FLAG)){
    log_message(global_opts.error_log, time(NULL), msg);
    DEBUGOUT1(" -->Error Logged \n");
  }
}

/* Schreiben eines Datensatzes in das Daten-Logfile */
void log_data(time_t timestamp, char *msg){
  log_message(global_opts.data_log, time(NULL), msg);
  DEBUGOUT1(" -->Data Logged \n");
}

/* Logfile oeffnen */
static int open_logfile(char *file){
  int log_fd;

  log_fd = open(file, O_WRONLY | O_APPEND | O_CREAT, 00600);
  if (log_fd < 0) {
    exit_error(ERROR_LOGFILE);
  }
  return log_fd;
}

/* Logfile schließen */
static int close_logfile(int log_fd){
  fsync(log_fd);
  close(log_fd);
}

/* Nachicht schreiben */
static void log_message(char *file, time_t timestamp, char *msg){
  int log_fd;
  char date[27];

  log_fd=open_logfile(file);
  snprintf(date, 27, (char *)asctime(localtime(&timestamp)));
  write(log_fd, date + 4, strlen(date) - 5);
  write(log_fd, " - ", 3);
  write(log_fd, msg, strlen(msg));
  write(log_fd, "\n", 1);
  close_logfile(log_fd);
}

char *get_error_buffer(){
  if(error_buffer == NULL){
    error_buffer = malloc(sizeof(char)*ERR_BUFFERSIZE);
  }
  return error_buffer;
}
#endif

/* Pointer auf letztes 'Clean-Element' bekommen.
 * Mir war grad nach rekrusiv und so viele Elemente sollten
 * es ja nicht sein.*/
static clean_struct_ptr get_last_clean(clean_struct_ptr ptr){
  if(ptr != NULL){
    if (ptr->next != NULL){
      return get_last_clean(ptr->next);
    } else {
      return ptr;
    }
  }
  return NULL;
}

/* Ein neues Clean-Element anfuegen. Ein Clean-Element ist eine Datenstruktur, die
 * einen Pointer auf eine Funktion vom Typ
 * void func(void *data),
 * einen Zeiger auf beliebige Daten und einen Zeiger auf das naechste Element haelt.
 * Die Funktionen werden beim regulaeren beenden des Programmes aufgerufen um zum bsp. 
 * datenbankverbindungen zu schließen, etc. */
void add_clean(void (*func)(void *data), void *data){
  clean_struct_ptr p, temp = (clean_struct_ptr) malloc(sizeof(clean_data));
  temp->data = data;
  temp->func = func;
  temp->next = NULL;
  if(clean_ptr == NULL){
    clean_ptr = temp;
  } else {
    p = get_last_clean(clean_ptr);
    p->next = temp;
  }
}

/* 'Saeuberung' ausfuehren */
static void clean(){
  DEBUGOUT1("\nRaeume auf...\n");
  clean_struct_ptr p = clean_ptr;
  while(p != NULL){
    p->func(p->data);
    p = p->next;
  }
}
