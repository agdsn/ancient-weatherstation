#include <stdio.h>
#include <gd.h>
#include "image_draw.h"
#include "image_data.h"
#include "image_common.h"


typedef int color;


static gdImagePtr create_image();
static gdImagePtr draw_image(gdImagePtr);
static void write_image_png(gdImagePtr, FILE *);


int draw_to_file(FILE *fd){
  gdImagePtr img = create_image();
  draw_image(img);
  write_image_png(img, fd);
}

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

static gdImagePtr draw_image(gdImagePtr img){
  pix_list_ptr pix_list = get_pix_list(300);
  scale_y_coords(pix_list, 100, 40, -10);  
}

static void write_image_png(gdImagePtr img, FILE *fd){
  gdImageAlphaBlending(img, 0);
  gdImageSaveAlpha(img, 1);
  gdImagePng(img, fd);
}


