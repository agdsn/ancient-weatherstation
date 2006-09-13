#include <stdio.h>
#include <gd.h>
#include "image_draw.h"
#include "image_data.h"
#include "image_common.h"


typedef int color;


static gdImagePtr create_image();
static gdImagePtr draw_image(gdImagePtr);
static void write_image_png(gdImagePtr, FILE *);

/* Baut ein Bild und schreibt es in die Datei */
int draw_to_file(FILE *fd){
  gdImagePtr img = create_image();
  draw_image(img);
  write_image_png(img, fd);
}

/* Erstellt ein Bild mit Hintergrundfarbe */
static gdImagePtr create_image(){
  gdImagePtr new_img   = NULL;
  color back;
  img_color_ptr back_p = img_cfg.bg_color;

  new_img = gdImageCreateTrueColor(img_cfg.width, img_cfg.height);
  gdImageAlphaBlending(new_img, 0);
  back = gdImageColorAllocateAlpha( new_img, back_p->r, back_p->g, back_p->b, back_p->alpha );
  gdImageFilledRectangle(new_img, 0,0,img_cfg.width, img_cfg.height, back);
  gdImageAlphaBlending(new_img, 1);
  
  return new_img;
}

/* Baut das Bild */
static gdImagePtr draw_image(gdImagePtr img){
  pix_list_ptr pix_list = NULL;
  int max_val 		= 0;
  int min_val 		= 0;
  int offset_x_left 	= 0;
  int offset_y_top 	= 0;
  int offset_x_right 	= 0;
  int offset_y_bottom 	= 0;
  int dia_width		= img_cfg.width - offset_x_left - offset_x_right;
  int dia_height 	= img_cfg.height - offset_y_top - offset_y_bottom;
  int zero_line 	= 0;

  color val_line_c 	= gdImageColorAllocateAlpha(img, 0,0,0,0);
  color zero_line_c	= gdImageColorAllocateAlpha(img, 0,0,0,0);

  pix_list = get_pix_list(dia_width);

  /* y-Werte skalieren */
  zero_line = scale_y_coords(pix_list, dia_height, 40, -10);  

  /* Nullinie */
  gdImageLine(img, offset_x_left, zero_line + offset_y_top, img_cfg.width - offset_x_right, zero_line + offset_y_top, zero_line_c); 
  
  /* Werte Zeichnen */
  for (; pix_list->next; pix_list = pix_list->next){
    gdImageLine(img, (offset_x_left + pix_list->x_pix_coord), (zero_line + offset_y_top + pix_list->y_pix_coord), (offset_x_left + pix_list->next->x_pix_coord), (zero_line + offset_y_top + pix_list->next->y_pix_coord), val_line_c);
  }


}




/* Schreibt das Bild in eine Datei */
static void write_image_png(gdImagePtr img, FILE *fd){
  gdImageAlphaBlending(img, 0);
  gdImageSaveAlpha(img, 1);
  gdImagePng(img, fd);
}


