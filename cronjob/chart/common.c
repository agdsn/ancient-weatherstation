/*

   common.c        -- Part of Chart-generator for the weatherstation

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


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>         	/* STDOUT_FILENO */
#include "common.h"
#include "definitions.h"

static void clean();


clean_struct_ptr clean_ptr  = NULL;

/* Ein neues Clean-Element anfuegen. Ein Clean-Element ist eine Datenstruktur, die
 * einen Pointer auf eine Funktion vom Typ
 * void func(void *data),
 * einen Zeiger auf beliebige Daten und einen Zeiger auf das naechste Element haelt.
 * Die Funktionen werden beim regulaeren beenden des Programmes aufgerufen um zum bsp. 
 * datenbankverbindungen zu schließen, etc. */
void add_clean(clean_func_t func, void *data){
  clean_struct_ptr temp = (clean_struct_ptr) malloc(sizeof(clean_data));

  temp->data = data;
  temp->func = func;
  temp->next = NULL;

  if(clean_ptr == NULL){
    clean_ptr = temp;
  } else {
    temp->next = clean_ptr;
    clean_ptr = temp;
  }
}

/* Zuletzt angefuegtes Clean - Element entfernen */
void remove_clean(){
  clean_struct_ptr temp = clean_ptr;
  if (temp != NULL){
    clean_ptr = temp->next;
    free(temp);
  }
}

/* Clean - Liste leeren */
void clear_clean(){
  while(clean_ptr != NULL){
    remove_clean();
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
void exit_sig_handler(int signr){
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
