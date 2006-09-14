#include <stdio.h>
#include <gd.h>
#include "../definitions.h"
#include "image_draw.h"
#include "image_data.h"
#include "image_common.h"


typedef int color;
typedef struct dimension {
  int width;
  int height;
  int l_t_x;
  int l_t_y;
  int l_b_x;
  int l_b_y;
  int r_b_x;
  int r_b_y;
  int r_t_x;
  int r_t_y;
  int to_base;
} dimension_t;


static gdImagePtr create_image();
static gdImagePtr draw_image(gdImagePtr);
static dimension_t calc_text_dim(char *, double , double );
static color alloc_alpha_color(gdImagePtr , img_color_ptr );
static void write_image_png(gdImagePtr, FILE *);

/* Baut ein Bild und schreibt es in die Datei */
int draw_to_file(FILE *fd){
  gdImagePtr img = create_image();
  draw_image(img);
  write_image_png(img, fd);
  gdImageDestroy(img);
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
  int offset_x_left 	= 60;
  int offset_y_top 	= 5;
  int offset_x_right 	= 20;
  int offset_y_bottom 	= 80;
  int dia_width		= img_cfg.width - offset_x_left - offset_x_right;
  int dia_height 	= 0; 
  int zero_line 	= 0;
  int dia_y_padding	= 10;
  int brect[8];

  dimension_t head_d;
  dimension_t y_label_d;
  dimension_t x_label_d;

  color val_line_c 	= alloc_alpha_color(img, img_cfg.dia_line_color);
  color zero_line_c	= alloc_alpha_color(img, img_cfg.zero_line_color);
  color dia_bg_c	= alloc_alpha_color(img, img_cfg.dia_bg_color);
  color diag_grid_c	= alloc_alpha_color(img, img_cfg.dia_grid_color);
  color dia_border_c	= alloc_alpha_color(img, img_cfg.dia_border_color);
  color headline_c 	= alloc_alpha_color(img, img_cfg.headline_color);


  /* Ueberschrift */
  head_d = calc_text_dim(img_cfg.headline, 16, 0);
  gdImageStringTTF(img, &brect[0], headline_c, IMG_FONT, 16, 0, 10, offset_y_top + head_d.to_base, img_cfg.headline);
  offset_y_top = (offset_y_top * 2) + head_d.height;



  dia_height = img_cfg.height - offset_y_top - offset_y_bottom;

  /* Werte holen */
  pix_list = get_pix_list(dia_width);

  /* Diagramhintergrund */
  gdImageFilledRectangle(img, offset_x_left, offset_y_top, img_cfg.width - offset_x_right, img_cfg.height - offset_y_bottom, dia_bg_c);


  /* horizontale linien + y - Labels */
  y_labels = get_y_label_list(dia_height, dia_y_padding, 0);
  for (; y_labels; y_labels = y_labels->next){
    gdImageLine(img, offset_x_left - 2, offset_y_top + y_labels->pos, img_cfg.width - offset_x_right, offset_y_top + y_labels->pos, diag_grid_c);
  }

  /* y-Werte skalieren */
  zero_line = scale_y_coords(pix_list, dia_height);  

  /* Vertikale linien + x - Labels*/
  x_labels = get_x_label_list(dia_width);
  for(; x_labels; x_labels = x_labels->next){
    gdImageLine(img, offset_x_left + x_labels->pos, offset_y_top, offset_x_left + x_labels->pos, img_cfg.height - offset_y_bottom + 2, diag_grid_c);
  }

  /* Nullinie */
  if (zero_line != -1)
    gdImageLine(img, offset_x_left, zero_line + offset_y_top, img_cfg.width - offset_x_right, zero_line + offset_y_top, zero_line_c); 


  /* Werte Zeichnen */
  for (; pix_list->next; pix_list = pix_list->next){
    gdImageLine(img, (offset_x_left + pix_list->x_pix_coord), (offset_y_top + pix_list->y_pix_coord), (offset_x_left + pix_list->next->x_pix_coord), (offset_y_top + pix_list->next->y_pix_coord), val_line_c);
  }

  gdImageRectangle(img,  offset_x_left, offset_y_top, img_cfg.width - offset_x_right, img_cfg.height - offset_y_bottom, dia_border_c);



}

static dimension_t calc_text_dim(char *text, double size, double angle){
  int brect[8];
  dimension_t dim;
  int x_rt_lb = 0;
  int x_rb_lt = 0;
  int y_rt_lb = 0;
  int y_rb_lt = 0;

  gdImageStringTTF(NULL, &brect[0], 0, IMG_FONT, size, angle, 0,0, text);

  dim.l_t_x = brect[6];
  dim.l_t_y = brect[7];
  dim.l_b_x = brect[0];
  dim.l_b_y = brect[1];
  dim.r_b_x = brect[2];
  dim.r_b_y = brect[3];
  dim.r_t_x = brect[4];
  dim.r_t_y = brect[5];

  dim.to_base = 0 - dim.l_t_y;

  //printf("%d -- %d\n", dim.l_t_y, dim.r_t_y);

  x_rt_lb = dim.r_t_x - dim.l_b_x;
  x_rb_lt = dim.r_b_x - dim.l_t_x;
  y_rt_lb = dim.r_t_y - dim.l_b_y;
  y_rb_lt = dim.r_b_y - dim.l_t_y;

  if(x_rt_lb < x_rb_lt){
    dim.width = x_rb_lt;
  } else {
    dim.width = x_rt_lb;
  }

  if(y_rt_lb < y_rb_lt){
    dim.height = y_rb_lt;
  } else {
    dim.height = y_rt_lb;
  }

  return dim;
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


