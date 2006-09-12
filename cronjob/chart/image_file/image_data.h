


typedef struct pix_list *pix_list_ptr;
typedef struct pix_list {
  pix_list_ptr 	next;
  int 		x_pix_coord;
  int 		value_count;
  int 		value_sum;
} pix_list_t;



/* Holt die Liste mit den Daten fuer das Bild.
 * 1. Argument: die effektive Breite des Bereiches in dem gezeichnet werden soll
 */
pix_list_ptr get_pix_list(int );
