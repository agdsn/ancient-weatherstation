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
#include "../definitions.h"
#include "../common.h"
#include "image_common.h"
#include "image_file.h"
#include "image_config.h"


image_cfg img_cfg;

void process_image_cfg(char *image_cfg_file){
  printf("%s\n",image_cfg_file);

  

  get_image_cfg(image_cfg_file);

  if (img_cfg.img_name != NULL){
    DEBUGOUT2("Image-Name = %s\n",img_cfg.img_name);
  }

  sleep(3);
}
