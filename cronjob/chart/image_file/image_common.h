typedef struct image_cfg_t {
  char	*file_name;
  char	*headline;
  long 	gen_interval;
  long 	show_interval;
  long 	label_interval;
  int 	label_sum;
  int 	width;
  int	height;
  int 	sens_id;
} image_cfg;


extern image_cfg img_cfg;