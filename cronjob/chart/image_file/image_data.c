/*

   image_data.c        -- Part of Chart-generator for the weatherstation

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

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <postgresql/libpq-fe.h>
#include "image_data.h"
#include "image_common.h"
#include "../common.h"
#include "../definitions.h"

#define BUFFSIZE 512


/* Variablen */
static pix_list_ptr min = NULL;			/* Pointer auf min - Element */
static pix_list_ptr max = NULL;			/* Pointer auf Max - Element */
double graph_min = 0;				/* graphen-maximum */
double graph_max = 0;				/* graphen-minimum */
static long base_time;				/* Zeit an der 0-Koordinate (lt. Datenbank!) */
int max_diff = 0;				/* Maximaler Abstand, den 2 Werte haben duerfen, ohne das die Linie unterbrochen wird */
int real_min = 0;
int real_max = 0;


/* Funktionsdefinitionen */
static inline pix_list_ptr add_pix_value(pix_list_ptr , int , int, int );
static char *get_conn_string();
static PGconn *pg_check_connect(char *);
static PGresult *pg_check_exec(PGconn *, char *, int);
static char *get_type_table_by_id(PGconn *, int );
static inline double * build_koeff_array(int, int , int);


/* Gibt die Liste mit den y-Labels zurueck */
label_list_ptr get_y_label_list(int c_hight, int padding){
  int max_val      = 0;				/* Maximaler Wert */
  int min_val	   = 0;				/* Minimaler Wert */
  double factor    = 0;				/* Pixel / Werte */
  int diff         = 0;				/* Wertebereich */
  double real_diff = 0;				/* Realer Wertebereich */
  int temp         = 0; 
  double koeff     = 1;
  double interval  = 0;
  int num          = 0;
  int max_num      = 0;
  int i;
  int new_val      = 0;
  char * buff      = NULL;
  double padd_val  = 0;				/* Wert, der oben und unten auf das 'reale' Max u. min aufgerechnet wird, damit die Linie nicht 'anstoesst' */

  label_list_ptr ptr      = NULL;
  label_list_ptr new_ptr  = NULL;
  label_list_ptr temp_ptr = NULL;


  DEBUGOUT1("\nBaue y-Labels...\n");
  DEBUGOUT4(" Max. Wert: %d, Min. Wert: %d (inkl. Koeffizient: %3.3f)\n", max_val, min_val, img_cfg.val_koeff);

  /* Max und min festlegen */
  if (!img_cfg.label_sum){
    max_val = ceil( ( ((double)max->value_sum) / ((double)max->value_count) ) * img_cfg.val_koeff);
    min_val = floor( ( ((double)min->value_sum) / ((double)min->value_count) ) * img_cfg.val_koeff);
  } else {
    max_val = ceil( ( ((double)max->value_sum) ) * img_cfg.val_koeff);
    min_val = floor( ( ((double)min->value_sum) ) * img_cfg.val_koeff);
  }
  /* Min 0 setzen wenn zero_min gesetzt */
  if (img_cfg.zero_min){
    min_val = 0;
  } 

  /* Rechnen ... */
  diff     = max_val - min_val;
  factor   =  ( ((double)c_hight - (2 * padding)) / ((double)diff) );
  padd_val = (1 / factor) * ((double)padding); 

  graph_min  = min_val - padd_val;
  graph_max  = max_val + padd_val; 
  real_diff = graph_max - graph_min;  

  DEBUGOUT5(" Realer Max. Wert: %3.3f, Realer Min. Wert: %3.3f Differenz: %3.3f (inkl. Koeffizient: %3.3f)\n", graph_max, graph_min, real_diff, img_cfg.val_koeff);


  /* Interval der Labels berechnen */
  temp     = floor( real_diff );
  interval = 1;
  max_num  = floor(c_hight / 20);
  while ((num = floor( temp / interval )) > max_num){
    interval++;
  }
  
  DEBUGOUT2(" Interval: %f \n", interval);

  /* An Labels auf die 0 'eichen' */
  temp = ceil(graph_min);
  while (fmod(((double)temp), ((double)interval)) != 0){
    temp++;
  }

  /* Bugfix: manchmal wird das obeste Label nicht angezeigt,
   * daher den Zaehler um eins erhoehen und wenn dabei ueber 
   * den Bereich hinaus, dann wieder eins runter */
  num++;
  if ((temp + ((num - 1) * interval)) > (graph_max - 0.5))
    num--;

  /* Puffer fuer die Labels */
  buff = malloc(sizeof(char)*BUFFSIZE);

  /* Labels generieren */
  for (i = 0; i < num; i++){
    
    /* Momentaner wert */
    new_val = temp + (i * interval);    
    
    /* Labels zusammenbauen */
    if(img_cfg.unit != NULL){
      snprintf(buff, BUFFSIZE, "%d%s", new_val, img_cfg.unit);
    } else {
      snprintf(buff, BUFFSIZE, "%d", new_val);
    }

    /* Neues Label - Element */
    new_ptr            = malloc(sizeof(label_list_t));
    new_ptr->pos       = floor( (graph_max -  ((double)new_val) ) * factor);
    new_ptr->value     = new_val;
    new_ptr->text      = strdup(buff);
    new_ptr->next      = NULL;

    /* Rueckgabe- und Temp-pointer zuweisen */
    if (ptr != NULL){
      temp_ptr->next = new_ptr;
      temp_ptr       = temp_ptr->next;
    } else {
      ptr      = new_ptr;
      temp_ptr = new_ptr;
    }

    DEBUGOUT3("  Label '%s' an Position %d\n", new_ptr->text, new_ptr->pos);
  }

  free(buff);

  DEBUGOUT2(" %d Labels generiert\n", num);

  return ptr;
}


/* Baut die Liste mit den Labels an der X-Achse */
label_list_ptr get_x_label_list(int c_width){
  double factor = ((double)img_cfg.label_interval) * ( ((double)c_width) / ((double)img_cfg.show_interval) );
  int num       = floor( ((double)img_cfg.show_interval) /  ((double)img_cfg.label_interval) );
  char *buff    = malloc(sizeof(char)*BUFFSIZE);
  int i;
  time_t timestamp;
  label_list_ptr ptr      = NULL;
  label_list_ptr new_ptr  = NULL;
  label_list_ptr temp_ptr = NULL;

  DEBUGOUT1("\nBaue x-Labels...\n");
 
  /* Labels generieren */
  for ( i = 1; i < num; i++ ) {
    
    /* aktueller Timestamp */
    timestamp = base_time + (i * img_cfg.label_interval);

    /* Text fuer das Label bauen */
    strftime(buff, BUFFSIZE, img_cfg.x_fmt, localtime(&timestamp) );

    /* Neues Label - Element */
    new_ptr            = malloc(sizeof(label_list_t));
    new_ptr->pos       = floor( ((double)i) * factor);
    new_ptr->value     = base_time + (i * img_cfg.label_interval);
    new_ptr->text      = strdup(buff);
    new_ptr->next      = NULL;

    /* Rueckgabe- und Temp-pointer zuweisen */
    if (ptr != NULL){
      temp_ptr->next = new_ptr;
      temp_ptr       = temp_ptr->next;
    } else {
      ptr      = new_ptr;
      temp_ptr = new_ptr;
    }

    DEBUGOUT3("  Label '%s' an Position %d\n", new_ptr->text, new_ptr->pos);
  }

  DEBUGOUT2(" %d Labels generiert\n", num); 
  
  free(buff);

  return ptr;
}


/* Skaliert die X-Koordinaten der Punkte im angegebenem Bereich
 * ausfuehrliche Beschreibung im header-file */
int scale_y_coords(pix_list_ptr ptr, int c_height){
  double range         = (((graph_max - graph_min) / img_cfg.val_koeff ) + 1) ;				/* Anzahl von 0,1-Schritten */
  double pix_per_scale = ((double)c_height) / ((double)range); 						/* Pixel pro 0,1 */
  int zero_line        = floor( ((double)((graph_max  / img_cfg.val_koeff) + 1)) * pix_per_scale);	/* Nullinie */
  pix_list_ptr temp    = ptr;										/* Temporaerer Pointer zum durchgehen der Liste */
  
  DEBUGOUT1("\nBerechne y-Koordinaten:\n");

  /* Liste duchgehen */
  for (; temp; temp = temp->next){
    
    /* unterscheidung ob summiert oder gemittelt werden soll */
    if(!img_cfg.label_sum){
      temp->y_pix_coord = (-1 * floor( ( ((double)temp->value_sum) / ((double)temp->value_count) ) * pix_per_scale)) + zero_line;
    } else {
      temp->y_pix_coord = (-1 * floor(  ((double)temp->value_sum) * pix_per_scale)) + zero_line;
    }

    DEBUGOUT3("  neue y-Koordinate: %d bei x: %d\n",temp->y_pix_coord, temp->x_pix_coord);
  }

  DEBUGOUT2(" Nullinie bei: %d\n", zero_line);
  
  /* Wenn Nullinie zu sehen, dann die Position zurueckgeben */
  if ((graph_max - graph_min + 1) >= graph_max){
    return zero_line;
  } else {
    return -1;
  }
}


/* Maximaler wert */
pix_list_ptr get_max_elem(){
  return min;
}
double get_max_val(){
  return real_max * img_cfg.val_koeff ;
}



/* Minimaler Wert */
pix_list_ptr get_min_elem(){
  return max;
}
double get_min_val(){
  return real_min * img_cfg.val_koeff ;
}


/* Maximale Zeit */
char *get_max_time(){
  char *buff = malloc(sizeof(char)*BUFFSIZE);
  time_t timestamp = base_time + img_cfg.show_interval;

  strftime(buff, BUFFSIZE, img_cfg.x_fmt_extra, localtime(&timestamp) );

  return buff;
}


/* Minimale Zeit */
char *get_min_time(){
  char *buff = malloc(sizeof(char)*BUFFSIZE);
  time_t timestamp = base_time ;

  strftime(buff, BUFFSIZE, img_cfg.x_fmt_extra, localtime(&timestamp) );

  return buff;
}


/* Holt eine Liste mit den Werten und den jeweiligen x-koordinaten */
pix_list_ptr get_pix_list(int c_width){
  double seconds_per_pix = ((double)c_width)/((double)img_cfg.show_interval);		/* Pixel pro Sekunde */
  char *conn_string 	 = get_conn_string();						/* Verbindungs-String */
  PGconn *conn 		 = pg_check_connect(conn_string);				/* Datenbank - Verbindung */
  char *table 		 = NULL;							/* Tabellen - Name */
  char *query		 = malloc(sizeof(char)*BUFFSIZE);				/* Query */
  PGresult *res		 = NULL;							/* Darenbank - Result */
  int time_field;									/* Id ses Timestamp-Feldes */
  int val_field;									/* Id des Wert - Feldes */
  long time_temp;									/* Hilfsvariable */
  int pix_coord;									/* x - Koordinate, an die der Wert gehoert */
  int i, s, t, u;									/* Laufvariable zum durchlaufen des Datenbank-resuls */
  long timestamp;
  int temp_value;
  pix_list_ptr list_ptr = NULL;								/* Zeiger auf den Anfang der Wertliste */
  pix_list_ptr temp_ptr = NULL;								/* Zeiger zum durchlaufen der Wertliste */

  DEBUGOUT1("\nHole Daten...\n");
  DEBUGOUT2("  Ein Pixel entspricht %f sekunden\n", seconds_per_pix);

  /* Tabellenname holen */
  if (img_cfg.manual_table) {
    table = strdup(img_cfg.table_name);
  } else {
    table = get_type_table_by_id(conn, img_cfg.sens_id);
  }

  /* Anfrage zusammenbauen */
  snprintf(query, BUFFSIZE, "SELECT round(date_part('epoch', current_timestamp))::int4 AS now, round(date_part('epoch', timestamp))::int4 AS times, %s AS val FROM %s WHERE sens_id=%d AND  timestamp > (current_timestamp - INTERVAL '%d seconds') ORDER BY times ASC", img_cfg.table_field, table, img_cfg.sens_id, img_cfg.show_interval );

  /* Anfrage stellen */
  res = pg_check_exec(conn, query, 1);

  /* ID's der Felder ermitteln */
  time_field = PQfnumber(res, "times");
  val_field  = PQfnumber(res, "val");

  /* Momentane Zeit */
  if(PQntuples(res) > 0){
    base_time = timestamp = (ntohl( *((uint32_t*)PQgetvalue(res, 0, PQfnumber(res, "now") ) )) ) - img_cfg.show_interval;
  } else {
    base_time = time(NULL);
  }

  max_diff = ceil(img_cfg.label_interval * seconds_per_pix);

  /* Ergebnisse durchlaufen */
  for (i = 0; i < PQntuples(res); i++){
    timestamp = ntohl( *((uint32_t*)PQgetvalue(res, i, time_field)));
    time_temp = timestamp - base_time;
    temp_value = ntohl( *((uint32_t*)PQgetvalue(res, i, val_field)) );

    /* Wenn Balken gezeichnet werden sollen */
    if(img_cfg.bars){
      time_temp --;
      if(time_temp < 1)
        time_temp++;
      time_temp = floor( ((double)time_temp) / ((double)img_cfg.label_interval) ) * img_cfg.label_interval;
    } else {
      if(temp_value > real_max){
        real_max = temp_value;
      } 
      if (temp_value < real_min){
        real_min = temp_value;
      }
    }

    /* Koordinate berechnen */
    pix_coord = floor( ((double)time_temp) * seconds_per_pix) ;
    
    /* Listenelement generieren */
    temp_ptr = add_pix_value(temp_ptr, pix_coord, ntohl(*((uint32_t*)PQgetvalue(res, i, val_field) )) , max_diff);

    /* Rueckgabe- und Max/Min-pointer zuweisen */
    if (list_ptr == NULL){
      list_ptr = temp_ptr;
      
      /* Globale Variablen neu initialisieren um 
       * mehtete Bilder in einem Thread bauen zu
       * koennen */
      min = temp_ptr;
      max = temp_ptr;
      real_max = temp_value;
      real_min = temp_value;
    }

  }

  /* Wenn keine Daten vorhanden, dann Fake - Element generieren */
  if(i == 0){
    timestamp = timestamp - base_time +1;
    if(img_cfg.bars){
      time_temp = floor( ((double)timestamp) / ((double)img_cfg.label_interval) ) * img_cfg.label_interval;
    } else {
      time_temp = timestamp;
    }
    pix_coord = floor( ((double)time_temp) * seconds_per_pix) ;
    temp_ptr  = add_pix_value(temp_ptr, pix_coord, 0 , max_diff);
    list_ptr = temp_ptr;
    min = temp_ptr;
    max = temp_ptr;
  }

  /* Min / Max ermitteln */
  temp_ptr = list_ptr;
  u = 1;
  s = 1;
  t = 1;
  for (; temp_ptr; temp_ptr = temp_ptr->next){
    if(!img_cfg.label_sum){
      t = temp_ptr->value_count;
      s = min->value_count;
      u = max->value_count;
    }
    if (min != NULL){
      if ( (temp_ptr->value_sum / t) < (min->value_sum / s) )
        min = temp_ptr;
    } else {
      min = temp_ptr;
    }

    if (max != NULL){
      if ( (temp_ptr->value_sum / t) > (max->value_sum / u) )
        max = temp_ptr;
    } else {
      max = temp_ptr;
    }

  }
  
  DEBUGOUT2(" %d Werte geholt \n", i);
  DEBUGOUT3(" Min: x-pos.: %d, Wert: %d\n", min->x_pix_coord, (min->value_sum / min->value_count) );
  DEBUGOUT3(" Max: x-pos.: %d, Wert: %d\n", max->x_pix_coord, (max->value_sum / max->value_count) );

  /* Aufraemen */
  PQclear(res);
  PQfinish(conn);
  free(query);
  free(table);
  free(conn_string);

  return list_ptr;
}

/* Speichert einen geholten Wert ab */
static inline pix_list_ptr add_pix_value(pix_list_ptr ptr, int coord, int value, int max_diff){ 
  int old_coord;

  /* Erstes Element */
  if(ptr == NULL){
    DEBUGOUT1("\nLese Daten ein:\n");
    ptr  		= malloc(sizeof(pix_list_t));
    ptr->next 		= NULL;
    ptr->x_pix_coord 	= coord;
    ptr->y_pix_coord 	= 0;
    ptr->value_count    = 1;
    ptr->value_sum	= value;
    DEBUGOUT3("  Erstes Element generiert...x-pos.: %d Wert: %d\n",ptr->x_pix_coord, ptr->value_sum);
  } else {

    /* Wenn schon ein Wert fuer X - Koordinate vorhanden,
     * dann aufsummieren, sonst neues Element anfuegen */
    if(coord == ptr->x_pix_coord){
      ptr->value_sum += value;
      ptr->value_count++;

      DEBUGOUT5("  Zu x-pos. %d %d. Wert (%d) hinzugefuegt. Durchschn.: %d\n", ptr->x_pix_coord, ptr->value_count, value, (ptr->value_sum/ptr->value_count) );
    } else {
      old_coord		= ptr->x_pix_coord;
      ptr->next		= malloc(sizeof(pix_list_t));
      ptr 		= ptr->next;
      ptr->x_pix_coord	= coord;
      ptr->y_pix_coord	= 0;
      ptr->value_sum 	= value;
      ptr->value_count 	= 1;
      ptr->next           = NULL;

      if ((coord - old_coord) > max_diff){
        ptr->no_line = 1;
      } else {
        ptr->no_line = 0;
      }

      DEBUGOUT3("  An x-pos. %d Wert %d eingefuegt\n", ptr->x_pix_coord, ptr->value_sum);
    }
  }
  return ptr;

}

/* Durchschnittslinie 
 * reale Werte, eff. Zeichenbreite */
pix_list_ptr build_average_line(pix_list_ptr real_list, int c_width){

  int list_count  = 0;			/* Anzahl Elemente im Array */
  int list_offset = 0;			/* Erste koordinate in der Liste (Koordinate an Array-Pos. 0) */
  int field_size  = c_width + 10;	/* Array-Groeße (sollte nicht groeßer sein als die Breite in Pixel) */
  int i,j,m;				/* Zaehlvariablen */
  double sum    = 0;			/* Summe der berechneten Werte */
  double *koeff = 0;			/* Momentan zu bearbeitener Koeffizient */
  double count  = 0;			/* Summe der Koeffizienten */

  pix_list_ptr old_temp       = real_list;					/* Liste mit den 'echten' Werten */
  pix_list_ptr *old_ptr_field = malloc(sizeof(pix_list_ptr) * (field_size));	/* Array, in das die Liste geschrieben wird */
  pix_list_ptr new_list       = NULL;						/* Neue 'Durchschnittsliste' */
  pix_list_ptr new_list_temp  = NULL;						/* Temp. Pointer zum 'durchwandern' der Liste */

  list_offset = old_temp->x_pix_coord;
  DEBUGOUT2("list_offset %d\n", list_offset);


  /* Array mit den Werten der 'echten' Liste fuellen */
  for(; old_temp; old_temp = old_temp->next){
    
    /* Wenn zu klein (sollte nicht passieren), dann Array vergroeßern */
    if(list_count >= field_size){
      field_size += c_width;
      realloc(old_ptr_field, sizeof(pix_list_ptr) * (field_size));
    }

    DEBUGOUT2("list_count: %d -- ", list_count);
    DEBUGOUT2("old_temp->x_pix_coord: %d -- ",old_temp->x_pix_coord);

    /* Wenn keine Werte vorhanden, dann Array mit NULL fuellen */
    while( (list_count + list_offset) > old_temp->x_pix_coord){
      old_ptr_field[list_count] = NULL;
      list_count++;
      DEBUGOUT1("Fuege Null hinzu\n");
    }
    
    /* Listenelement in Array schreiben */
    old_ptr_field[list_count] = old_temp;
    DEBUGOUT1("Fuege Wert hinzu\n");

    list_count++;
  }

  /* Koeffizienten bauen */
  koeff = build_koeff_array(img_cfg.use_gauss_average, img_cfg.average_look_width, img_cfg.gauss_width);

  /* Array durchgehen und neue Werte berechnen.
   * Es werden immer 30 Werte Link und 30 Werte rechts mit
   * sich jeweils um 0.03 veringernden koeffizienten 
   * multipliziert und zur Summe addiert.
   * Das ganze wird dann durch die Summe der koeffizienten geteilt */
  for(i = 0; i < list_count; i++){
    if(old_ptr_field[i] != NULL){

      /* Aktueller Wert aus der 'realen' Liste */
      sum = ((double)old_ptr_field[i]->value_sum) / ((double)old_ptr_field[i]->value_count) * koeff[0];

      /* Summe der Koeffizienten. 
       * Aktueller Wert hat die Wichtung 1, 
       * daher mit 1 initialisiert */
      count = koeff[0];

      /* 30 (29) Links ind rechts durchgehen */
      for(j = 1; j < img_cfg.average_look_width; j++){

	/* Anzahl, wie viele Werte mit dem Aktuellem 
	 * Koeffizienten zur Summe addiert wurden */
	m = 0;

	/* j Werte 'links' vom aktuellem Wert */
	if( (i - j) >= 0){
	  if(old_ptr_field[i-j] != NULL){
	    sum += (((double)old_ptr_field[i-j]->value_sum) / ((double)old_ptr_field[i-j]->value_count)) * koeff[j];
	    m++;
	  }
	}

	/* j Werte rechts vom aktuellem Wert */
	if( (i + j) < list_count){
	  if(old_ptr_field[i+j] != NULL){
	    sum += (((double)old_ptr_field[i+j]->value_sum) / ((double)old_ptr_field[i+j]->value_count)) * koeff[j];
	    m++;
	  }
	}

       /* Hier wir die anzahl der Werte, 
        * die zur Summe hinzuaddiert wurden 
        * multipliziert mit dem aktuellem 
        * koeffizienten und das ganze auf 
        * den gesammtkoeffizienten addiert */
        count += m * koeff[j];


      }

      /* Neues Element an die Durchschnittsliste anfuegen */
      new_list_temp = add_pix_value(new_list_temp, old_ptr_field[i]->x_pix_coord, floor(sum / count), max_diff);

      /* Beim ersten Durchlauf den Kopf der Liste merken */
      if(new_list == NULL){
        new_list = new_list_temp;
      }

    }
  }

  /* Koeffizienten freigeben */
  free(koeff);

  /* Array freigeben */
  free(old_ptr_field);

  /* Durchschnittsliste zurueckgeen */
  return new_list;
}


/* Array mit koeffizienten bauen 
 * gibt als rueckgabewert die koeffizienten
 * Typ: 0=linear, 1=gauss*/
static inline double* build_koeff_array(int type, int look_width, int gauss_width){
  int j;								/* Laufvariable */
  double div;
  int o                = gauss_width;
  double * koeff_array = malloc((sizeof(double) * (look_width+1)));

  for(j = 0; j < look_width; j++){
    if(type){
      /* GAUSS!!!*/
      koeff_array[j] = (1/ (sqrt(2*PI) * o))*exp(-(j*j)/(o*o));
    } else {
      /* LINEAR*/
      if (j == 0){
        koeff_array[0] = 1;
      } else {
        div = 0.9 / ((double)look_width);      
        koeff_array[j] = 0.9 - (div * j);
      }
    }
  }
  return koeff_array;
}


/* baut den String fuer die Postgres-Verbindung zusammen */
static char *get_conn_string(){
    char *conn_string = malloc(sizeof(char)*BUFFSIZE);
    snprintf(conn_string, BUFFSIZE, "host=%s dbname=%s user=%s password=%s connect_timeout=%s", global_opts.pg_host, global_opts.pg_database, global_opts.pg_user, global_opts.pg_pass, global_opts.pg_timeout);
    return conn_string;
}


/* Baut eine Vebindung zur postgres auf, bricht mit fehler ab wenn nicht moeglich */
static PGconn *pg_check_connect(char *conn_string){
  PGconn *conn = PQconnectdb(conn_string);			/* Connection aufbauen */
  if(PQstatus(conn) != CONNECTION_OK){
    DEBUGOUT2("\nFehler beim Aufbau der Datenbankverbindung\n%s\n", PQerrorMessage(conn));
    exit_error(ERROR_MAKECONN);
  }
  DEBUGOUT1("Verbindung hergestellt \n");
  return conn;
}

/* Fuehrt ein SQL-Statement aus. Bricht mit fehler ab wenn nicht moeglich */
static PGresult *pg_check_exec(PGconn *conn, char *query, int in_binary){
  PGresult *res;
  //res = PQexec(conn, query);
  res = PQexecParams(conn, query,0,NULL,NULL,NULL,NULL,in_binary);
  if(!res || PQresultStatus(res) != PGRES_TUPLES_OK){
    DEBUGOUT2("Fehler beim exec: %s\n", query);
    exit_error(ERROR_QUERY);
  } else {
    DEBUGOUT2("Query: '%s' ausgefuehrt\n", query);
  }
  return res;
}


/* Tabellenname des typs aus der Datenbank holen */
static char *get_type_table_by_id(PGconn *connection, int sens_id){
  char *table;
  int table_field;
  PGresult *table_res;
  char *query_buff 	= malloc(sizeof(char)*BUFFSIZE);

  snprintf(query_buff, BUFFSIZE, "select typen.tabelle as tbl FROM typen, sensoren WHERE sensoren.id=%d AND typen.typ=sensoren.typ", sens_id);
  table_res = pg_check_exec(connection, query_buff, 0);
  
  if(PQntuples(table_res) < 1)
    return NULL;
  
  table_field = PQfnumber(table_res, "tbl");
  table       = strdup(PQgetvalue(table_res, 0, table_field));
  
  DEBUGOUT2("\tTabelle: %s \n", table);

  PQclear(table_res);
  free(query_buff);

  return table;
}
