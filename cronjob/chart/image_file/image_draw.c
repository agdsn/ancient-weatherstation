#include <stdio.h>
#include <gd.h>
#include "image_draw.h"
#include "image_data.h"
#include "image_common.h"


typedef int color;


static gdImagePtr create_image();
static gdImagePtr draw_image(gdImagePtr);
static color alloc_alpha_color(gdImagePtr , img_color_ptr );
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

  new_img = gdImageCreateTrueColor(img_cfg.width, img_cfg.height);
  gdImageAlphaBlending(new_img, 0);
  back = alloc_alpha_color(new_img, img_cfg.bg_color); 
  gdImageFilledRectangle(new_img, 0,0,img_cfg.width, img_cfg.height, back);
  gdImageAlphaBlending(new_img, 1);
  
  return new_img;
}

/* Baut das Bild */
static gdImagePtr draw_image(gdImagePtr img){
  pix_list_ptr pix_list   = NULL;
  label_list_ptr x_labels = NULL;
  label_list_ptr y_labels = NULL;

  int max_val 		= 0;
  int min_val 		= 0;
  int offset_x_left 	= 20;
  int offset_y_top 	= 20;
  int offset_x_right 	= 20;
  int offset_y_bottom 	= 20;
  int dia_width		= img_cfg.width - offset_x_left - offset_x_right;
  int dia_height 	= img_cfg.height - offset_y_top - offset_y_bottom;
  int zero_line 	= 0;
  int dia_y_padding	= 10;

  color val_line_c 	= alloc_alpha_color(img, img_cfg.dia_line_color);
  color zero_line_c	= alloc_alpha_color(img, img_cfg.zero_line_color);
  color dia_bg_c	= alloc_alpha_color(img, img_cfg.dia_bg_color);
  color diag_grid_c	= alloc_alpha_color(img, img_cfg.dia_grid_color);

  /* Werte holen */
  pix_list = get_pix_list(dia_width);

  


  




  /* Diagramhintergrund */
  gdImageFilledRectangle(img, offset_x_left, offset_y_top, img_cfg.width - offset_x_right, img_cfg.height - offset_y_bottom, dia_bg_c);


  y_labels = get_y_label_list(dia_height, dia_y_padding, 0);
  for (; y_labels; y_labels = y_labels->next){
    gdImageLine(img, offset_x_left, offset_y_top + y_labels->pos, img_cfg.width - offset_x_right, offset_y_top + y_labels->pos, diag_grid_c);
  }

  /* y-Werte skalieren */
  zero_line = scale_y_coords(pix_list, dia_height);  

  /* Vertikale linien + x - Labels*/
  x_labels = get_x_label_list(dia_width);
  for(; x_labels; x_labels = x_labels->next){
    gdImageLine(img, offset_x_left + x_labels->pos, offset_y_top, offset_x_left + x_labels->pos, img_cfg.height - offset_y_bottom, diag_grid_c);
  }

  /* Nullinie */
  if (zero_line != -1)
    gdImageLine(img, offset_x_left, zero_line + offset_y_top, img_cfg.width - offset_x_right, zero_line + offset_y_top, zero_line_c); 


  /* Werte Zeichnen */
  for (; pix_list->next; pix_list = pix_list->next){
    gdImageLine(img, (offset_x_left + pix_list->x_pix_coord), (zero_line + offset_y_top + pix_list->y_pix_coord), (offset_x_left + pix_list->next->x_pix_coord), (zero_line + offset_y_top + pix_list->next->y_pix_coord), val_line_c);
  }


}




/* Eine Farbe mit alphawert holen */
static color alloc_alpha_color(gdImagePtr img, img_color_ptr ptr){
  return  gdImageColorAllocateAlpha(img, ptr->r, ptr->g, ptr->b, ptr->alpha);
}

/* Schreibt das Bild in eine Datei */
static void write_image_png(gdImagePtr img, FILE *fd){
  gdImageAlphaBlending(img, 0);
  gdImageSaveAlpha(img, 1);
  gdImagePng(img, fd);
}


