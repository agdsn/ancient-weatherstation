#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libesmtp.h>
#include <auth-client.h>
#include <openssl/ssl.h>
#include "definitions.h"
#include "mailer.h"

#define BUFFSIZE 2048

#define TO_LINE "To: "
#define CC_LINE "Cc: "
#define BCC_LINE "Bcc: "

/* Test */
FILE * ft = NULL;
char *bufft = NULL;


/* Funktionen */
static int build_header( address_struct *,   address_struct *,  address_struct *,  address_struct *, char *, FILE *);
static char * gen_from_mailbox();
static char * gen_address_line(char *, const char *, address_struct *);
static const char *read_mail_tmp_file (void **, int *, void *);
static int add_recipients(smtp_message_t , address_struct *);




/* Funktion zum versenden von Mails */
int mail_message(address_all_struct *addresses,  char *subject, int eightbit, mail_linereader_cb line_read_cb){
  
  FILE *fd = NULL;							/* Zeiger auf Temporaere Datei */
  int i;								/* Laufvariable */
  char *buf;								/* Puffer zum anlegen der temp. Datei */
  smtp_session_t session;						/* SMTP - Session */
  smtp_message_t message;						/* SMTP - Message */
  const smtp_status_t *status;						/* Uebertragungsstatus */

  if((fd = tmpfile()) == NULL)						/* Temporäre Datei anlegen */
    return 0;

  if(addresses->from == NULL){						/* Wenn absenderstruktur nicht gesetzt ist */
    addresses->from = malloc(sizeof(address_struct));			/* Dann speicher allokieren */
    addresses->from->mailbox  = NULL;					/* struktur initialisieren (NULL-Pointer als addresse) */
    addresses->from->next     = NULL;					/* nachfolgezeiger auf NULL setzen (gibt eh nur einen absender) */
  }

  if(addresses->from->mailbox == NULL){					/* Wenn Absenderaddresse nicht gesetzt */
    addresses->from->mailbox = gen_from_mailbox();			/* Dann eine Addresse nach dem schema user@host generieren */
  }

  build_header(addresses->from, addresses->to, addresses->cc, addresses->bcc, subject, fd);	/* Header in die Temporäre Datei schreiben */
  
  i = 0;
  while((buf = line_read_cb(i)) != NULL){		 		/* Solange die Callbackfunktion einen Zeiger auf einen Puffer liefert */
    fputs(buf, fd);							/* Schreibe dessen Inhalt in eine Datei */
    i++;
  }

  if((session = smtp_create_session()) == NULL)				/* SMTP - Session erstellen */
    return 0;

  if(!smtp_set_server(session, "atlantis.wh2.tu-dresden.de:25"))	/* Den Server für die Übertragung setzen */
    return 0;

  if((message = smtp_add_message(session)) == NULL)			/* Eine Nachicht zur Session hinzufügen */
    return 0; 

  if(! smtp_set_reverse_path(message,addresses->from->mailbox))		/* Absenderaddresse setzen */
    return 0;

  if(!smtp_set_messagecb(message, read_mail_tmp_file, fd))		/* Callback - Funktion zum lesen der temp. Datei */
    return 0;

  if(!(add_recipients(message, addresses->to) &&  add_recipients(message, addresses->cc) && add_recipients(message, addresses->bcc) ) )		/* Empfänger setzen */
    return 0;

  if(!smtp_start_session(session))					/* Session Starten (Mail versenden) */
    return 0;

  status = smtp_message_transfer_status (message);			/* uebertragungsstatus holen */
  DEBUGOUT3( "SMTP sending: %d %s", status->code, status->text);	/* und anzeigen */
  
  return 1;
}


/* Baut den Mail-header zusammen 
 * from ist die Liste mit den Absenderaddressen (sollte nur eine sein) 
 * to ist diie Liste der Emfaenger
 * cc ist die Liste der CC - Empfaenger
 * bcc iste die Liste der BCC - Empfaenger 
 * subject ist der Betreff der Mail
 * fd ist der Zeiger auf die temp. Datei */
static int build_header(address_struct * from,  address_struct *to,  address_struct *cc,  address_struct *bcc, char *subject, FILE *fd){
  char *buf;								/* Puffer für die Zeilen */
  if(from == NULL || to == NULL || fd == NULL){				/* wenn absender, Empfaenger oder Dateizeiger NULL */
    return 0;								/* Dann abbrechen */
  }
  buf = malloc(sizeof(char)*BUFFSIZE);					/* Speicher für Puffer holen */

  fprintf(fd, "From: <%s>\r\n", from->mailbox);				/* Absender setzen */
  fputs(gen_address_line(buf, TO_LINE, to), fd);			/* To setzen */
  fputs(gen_address_line(buf, CC_LINE, cc), fd);			/* Cc setzen */
  fputs(gen_address_line(buf, BCC_LINE, bcc), fd);			/* Bcc setzen */
  fprintf(fd, "Subject: %s\r\n", subject);				/* Betreff setzen */
  fputs("Reply-To:\r\n\r\n", fd);					/* Reply-To + Leerzeile anhängen */

  free(buf);								/* Puffer freigeben */
  
  return 1; 
}


/* Baut eine neue Absenderaddresse nach der Form user@host */
#define MAXHOSTNAMELEN 	256
static char * gen_from_mailbox(){
  char *host, *user, *new_from;	

  host = malloc(sizeof(char)*MAXHOSTNAMELEN);				/* Speicher fuer den Hostnamen holen */
  user = getlogin();							/* den Usernamen holen */
  gethostname(host, MAXHOSTNAMELEN);					/* den Hostnamen holen */

  new_from = malloc(sizeof(char)*(MAXHOSTNAMELEN + strlen(user) + 10));		/* Speicher fuer die gesammtaddresse belegen */
  snprintf(new_from, (MAXHOSTNAMELEN + strlen(user) + 10), "%s@%s",user,host);	/* gesammtaddresse zusammenbauen */

  free(host);								/* den speicher der den hostnamen beinhaltete freigeben */
  return new_from;
}


/* Eine Zeile mit addressen generieren 
 * buff ist der Puffer fuer die Zeile 
 * line_type ist eine der Konstanten TO_LINE, CC_LINE oder BCC_LINE
 * address sind die Addressen fuer die Zeile */
static char *  gen_address_line(char *buf, const char *line_type, address_struct *address){
  address_struct *addr = address;			  		/* Temporärer Zeiger */
  int i = 0			;					/* zum unterscheiden, ob die erste Addresse oder nicht */

  strcpy(buf, line_type);						/* Das Keyword ('To: ', 'Cc: ', ...) an den Zeilenanfang setzen */
  for(; addr; addr = addr->next){					/* Addressen durchlaufen */
    if(i)								/* Wenn nicht erste Addresse... */
      strcat(buf, ", ");						/* dann ein ', ' davorsetzen */
    
    strcat(buf, "<");							/* '<' setzen (va Addressen in <> eingeschlossen sind) */
    strcat(buf, addr->mailbox);						/* addresse in den puffer schreiben */
    strcat(buf, ">");							/* '>' dahintersetzen */
    i++;					
  }
  strcat(buf, "\r\n");							/* Wagenrücklauf + Zeilenvorschub anhängen */
  return buf;
}


/* Mail aus temporärer Datei lesen (Callbackfkt.) 
 * buf ist ein Zeiger auf einen Zeichenpuffer
 * len ist NULL (dann rewind des Files) oder nicht NULL (dann Daten lesen)
 * arg ist der Zeiger auf die temp. Datei */
static const char *read_mail_tmp_file (void **buf, int *len, void *arg){
  int octets;								/* Anzahl der Zeichen die gelesen wurden */

  if (*buf == NULL)							/* Wenn Puffer noch nicht initialisiert */
    *buf = malloc (BUFFSIZE);						/* Dann speicher dafuer allokieren */

  if (len == NULL) {							/* Wenn len NULL */
    rewind ((FILE *) arg);						/* Dann die Datei 'zuruechspulen */
    return NULL;							/* und NULL zurueckgeben */
  }

  if (fgets (*buf, BUFFSIZE - 2, (FILE *) arg) == NULL) {		/* Wenn keine Daten mehr im File */
    octets = 0;								/* dann gelesene Zeichen auf 0 setzen */
  } else {								/* Ansonsten */
    char *p = strchr (*buf, '\0');					/* Das Terminierungszeichen suchen */

    if (p[-1] == '\n' && p[-2] != '\r') {				/* Wenn kein Wagenruecklauf */
      strcpy (p - 1, "\r\n");						/* dann einen ergänzen */
      p++;								/* und den String ein Zeichen länger machen */
    }
    octets = p - (char *) *buf;						/* Gelesene Zeichen berechnen */
  }

  *len = octets;
  return *buf;
}


/* Empfaenger zu einer Mail hinzufügen 
 * message ist die SMTP - Message 
 * address ist die Liste mit den Empfaengeraddressen */
static int add_recipients(smtp_message_t message, address_struct *address){
  address_struct *adr = address;					/* Temp. Zeiger auf die Addressen */
  for(; adr; adr = adr->next){						/* Addressen durchgehen */
    if(adr->mailbox){							/* Wenn eine gueltige gefunden */
      if(!smtp_add_recipient(message, adr->mailbox))			/* Dann hinzufügen */
	return 0;
    }
  }
  return 1;
}




static char * test_read(int line){
  if(bufft == NULL)
    bufft = malloc(sizeof(char)*1024);
  return fgets(bufft, 1024, ft);
}

main(char argc[], int arglen){
  printf("%s\n", gen_from_mailbox());

  printf("lege Strukturen an...\n");
  address_struct *to, *cc, *bcc;
  to = malloc(sizeof(address_struct));
  to->mailbox = "losinski@wh2.tu-dresden.de";
  to->next = NULL;
/*  to->next = malloc(sizeof(address_struct));
  to->next->mailbox = "jooschi@wh2.tu-dresden.de";
  to->next->next = NULL;*/
  cc = malloc(sizeof(address_struct));
  cc->mailbox = "losinski@wh2.tu-dresden.de";
  cc->next = NULL;
  bcc = malloc(sizeof(address_struct));
  bcc->mailbox = "losinski@wh2.tu-dresden.de";
  bcc->next = NULL;

  address_all_struct adr;
  adr.from = NULL;
  adr.to = to;
  adr.cc = NULL;//cc;
  adr.bcc = NULL;//bcc;

  ft = fopen("test_data", "r");
  rewind(ft);
printf("rufe mail_message auf...\n");
 mail_message( &adr,  "Powered by libesmtp", 0, test_read);
  

  printf("fertig...\n");
  exit(0);
}

