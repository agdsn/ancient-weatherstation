#include <stdio.h>
#include "image_draw.h"
#include "image_data.h"


static void create_image();
static void draw_image();
static void write_image();


int draw_to_file(FILE *fd){
  draw_image();
}

static void create_image(){

}

static void draw_image(){
  pix_list_ptr pix_list = get_pix_list(300);

}

static void write_image(){

}
