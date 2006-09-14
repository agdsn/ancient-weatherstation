typedef struct img_color *img_color_ptr; 
typedef struct img_color {
  int r;
  int g;
  int b;
  int alpha;
} img_color_t;

typedef struct image_cfg {
  char	 *file_name;
  char	 *headline;
  char   *table_field;
  int 	 manual_table;
  char   *table_name;
  long 	 gen_interval;
  long 	 show_interval;
  long 	 label_interval;
  int 	 label_sum;
  int 	 width;
  int 	 height;
  int  	 sens_id;
  double val_koeff;

  img_color_ptr bg_color;
  img_color_ptr dia_bg_color;
  img_color_ptr dia_line_color;
  img_color_ptr dia_grid_color;
  img_color_ptr dia_border_color;
  img_color_ptr zero_line_color;
  img_color_ptr label_color;
  img_color_ptr headline_color;




} image_cfg_t;

extern image_cfg_t img_cfg;
