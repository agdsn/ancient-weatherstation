/*

   process_image.c -- Part of Chart-generator for the weatherstation

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
#include <time.h>
#include <sys/stat.h>
#include "../definitions.h"
#include "../common.h"
#include "image_common.h"
#include "image_file.h"
#include "image_config.h"


image_cfg img_cfg;

void process_image_cfg(char *image_cfg_file){

  get_image_cfg(image_cfg_file);

  if (img_cfg.file_name != NULL){
    DEBUGOUT2("File-Name     = %s\n", img_cfg.file_name);
    DEBUGOUT2("Ueberschr.    = %s\n", img_cfg.headline);
    DEBUGOUT2("Gen.Interval  = %d\n", img_cfg.gen_interval);
    DEBUGOUT2("ShowInterval  = %d\n", img_cfg.show_interval);
    DEBUGOUT2("LabelInterval = %d\n", img_cfg.label_interval);
    DEBUGOUT2("LabelSum      = %d\n", img_cfg.label_sum);
    DEBUGOUT2("Width         = %d\n", img_cfg.width);
    DEBUGOUT2("Height        = %d\n", img_cfg.height);
    DEBUGOUT2("SensorId      = %d\n", img_cfg.sens_id);
    DEBUGOUT1("\n");
  } else {
    return;
  }
  check_file_interval();
  sleep(3);
}


int check_file_interval(){

  struct stat stat_buff;
  time_t now;
  long diff_sek;

  if(access(img_cfg.file_name, F_OK) == -1){
    DEBUGOUT2("Datei '%s' existiert nicht\n", img_cfg.file_name);
    return 1;
  }
  if ((stat(img_cfg.file_name, &stat_buff)) != -1){
    
    now = time(NULL);
    diff_sek = difftime(now, stat_buff.st_mtime);

    DEBUGOUT3("Datei '%s' ist %d Sek. alt \n", img_cfg.file_name, diff_sek);

    if(diff_sek > (img_cfg.gen_interval * 60))
      return 1;

  } else {
    exit_error(ERROR_STAT);
  }
  return 0;
}
