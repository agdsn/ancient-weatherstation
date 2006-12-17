/*

   image_file.c  -- Part of Chart-generator for the weatherstation

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
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/stat.h>
#include "../definitions.h"
#include "../common.h"
#include "image_common.h"
#include "image_file.h"
#include "image_config.h"
#include "image_draw.h"

#define BUFFSIZE 512


/* Optionen des Bildes */
image_cfg_t img_cfg;


/* Funktionsdefinitionen */
static void regenerate_image();
static int check_file_interval();
static int check_time_with_stat();
static int check_time_with_fixpoint();


/* Handelt ein Bild */
void process_image_cfg(char *image_cfg_file){
  char *buff;
  int i;

  get_image_cfg(image_cfg_file);

  if (img_cfg.file_name != NULL){

    /* Debugausgabe einiger Config - Optionen */
    DEBUGOUT2("File-Name     = %s\n", img_cfg.file_name);
    DEBUGOUT2("Ueberschr.    = %s\n", img_cfg.headline);
    DEBUGOUT2("Gen.Interval  = %d\n", img_cfg.gen_interval);
    DEBUGOUT2("ShowInterval  = %d\n", img_cfg.show_interval);
    DEBUGOUT2("LabelInterval = %d\n", img_cfg.label_interval);
    DEBUGOUT2("LabelSum      = %d\n", img_cfg.label_sum);
    DEBUGOUT2("Width         = %d\n", img_cfg.width);
    DEBUGOUT2("Height        = %d\n", img_cfg.height);
    DEBUGOUT2("SensorId      = %d\n", img_cfg.sens_id);
    DEBUGOUT2("TabellenFeld  = %s\n", img_cfg.table_field);
    DEBUGOUT2("ManualTabelle = %d\n", img_cfg.manual_table);
    DEBUGOUT2("TabellenName  = %s\n", img_cfg.table_name);
    DEBUGOUT1("\n");

    /* Bild - Namen zusammenbauen */
    if(img_cfg.dflt_dir){
      buff = malloc(sizeof(char)*BUFFSIZE);
      buff = strncpy(buff, global_opts.dflt_image_location, BUFFSIZE - 1);
      i = strlen(buff);
      buff = strncat(buff, img_cfg.file_name, BUFFSIZE - i - 1);
      img_cfg.file_name = strdup(buff);
      free(buff);
    }
  } else {
    return;
  }

  /* Pruefen ob Generierung noetig */
  if(check_file_interval()){
    regenerate_image();		/* generieren */
  }

}


/* Checkt ob es wieder an der Zeit ist ein File 
 * neu zu erstellen */
static int check_file_interval(){

  /* Pruefen ob Bild vorhanden */
  if(access(img_cfg.file_name, F_OK) == -1){
    DEBUGOUT2("Datei '%s' existiert nicht\n", img_cfg.file_name);
    DEBUGOUT1("Sie muss neu generiert werden!\n");
    return 1;
  }

  if(img_cfg.fix_timepoint){
    return check_time_with_fixpoint();
  } else {
    return check_time_with_stat();
  }

}

static int check_time_with_stat(){

  struct stat stat_buff;
  time_t now;
  long diff_sek;

  /* Pruefen ob Bild zu alt */
  if ((stat(img_cfg.file_name, &stat_buff)) != -1){
    
    now = time(NULL);
    diff_sek = difftime(now, stat_buff.st_mtime);

    DEBUGOUT3("Datei '%s' ist %d Sek. alt \n", img_cfg.file_name, diff_sek);
    DEBUGOUT2("Sie soll aller %d Sek. neu generiert werden \n", img_cfg.gen_interval);

    if(diff_sek > img_cfg.gen_interval){
      DEBUGOUT1("Sie muss neu generiert werden!\n");
      return 1;
    }

  } else {
    exit_error(ERROR_STAT);
  }
  DEBUGOUT1("Datei ist aktuell genug!\n");
  return 0;

}

static int check_time_with_fixpoint(){
  
  time_t now;
  int max_diff  =  (CRON_INTERVAL -1) * 60;
  int exact_val = 0;
  int diff      = 0;

  now = time(NULL);
 
  exact_val = (floor( ((double)difftime(now, img_cfg.timepoint)) / ((double)img_cfg.gen_interval) ) * img_cfg.gen_interval) + img_cfg.timepoint;
  diff = difftime(now, exact_val);

  if(diff < max_diff){
    DEBUGOUT3("Bild wird generiert, denn es ist kurz (%d sec) nach %s\n", diff, ctime(&exact_val));
    
    return 1;
  } else {
    DEBUGOUT1("Bild ist aktuell genug\n");
    return 0;
  }


}


/* Bild (neu-) generieren */
static void regenerate_image(){
  FILE *fd = fopen(img_cfg.file_name, "wb");	/* Datei oeffnen */
  draw_to_file(fd);				/* Bild bauen und in Datei schreiben */
  fclose(fd);					/* Datei schließen */
}
