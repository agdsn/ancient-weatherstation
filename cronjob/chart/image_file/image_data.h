


typedef struct pix_list *pix_list_ptr;
typedef struct pix_list {
  pix_list_ptr 	next;
  int 		x_pix_coord;
  int 		y_pix_coord;
  int 		value_count;
  int 		value_sum;
} pix_list_t;

typedef struct label_list *label_list_ptr;
typedef struct label_list {
  int  		 pos;
  long 		 value;
  char 		 *text;
  label_list_ptr next;
} label_list_t;



/* Holt die Liste mit den Daten fuer das Bild.
 * 1. Argument: die effektive Breite des Bereiches in dem gezeichnet werden soll
 * Rueckgabe: die liste mit den Werten 
 */
pix_list_ptr get_pix_list(int );


/* Max. Wert */
pix_list_ptr get_min_val();


/* Min. Wert */
pix_list_ptr get_max_val();
 

char *get_max_time();
char *get_min_time();


/* Skaliert die X-Koordinaten der Punkte im angegebenem Bereich
 * 1. Argument: die Pix-Liste die skaliert werden soll
 * 2. Argument: die anzahl der Pixel in y-Richtung
 * Rueckgabe:   Position der nullinie. (wenn nicht sichtbar -1)
 */
int scale_y_coords(pix_list_ptr , int );


/* Baut die Liste mit den Labels an der X-Achse 
 * 1. Argument: Breite des bildes
 * Rueckgabe: List mit den Labels
 */
label_list_ptr get_x_label_list(int );

label_list_ptr get_y_label_list(int, int);
