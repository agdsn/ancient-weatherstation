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
static void event_cb (smtp_session_t , int , void *,...);   

/* Variablen */



/* Funktion zum versenden von Mails */
int mail_message(address_all_struct *addresses,  char *subject, int eightbit, mail_linereader_cb line_read_cb, server_vars *servopts){
  
  FILE *fd = NULL;							/* Zeiger auf Temporaere Datei */
  int i;								/* Laufvariable */
  char *buf;								/* Puffer zum anlegen der temp. Datei */
  smtp_session_t session;						/* SMTP - Session */
  smtp_message_t message;						/* SMTP - Message */
  const smtp_status_t *status;						/* Uebertragungsstatus */

  if(servopts == NULL)
    return 0;

  if(addresses->from == NULL){						/* Wenn absenderstruktur nicht gesetzt ist */
    addresses->from = malloc(sizeof(address_struct));			/* Dann speicher allokieren */
    addresses->from->mailbox  = NULL;					/* struktur initialisieren (NULL-Pointer als addresse) */
    addresses->from->next     = NULL;					/* nachfolgezeiger auf NULL setzen (gibt eh nur einen absender) */
  }

  if(addresses->from->mailbox == NULL){					/* Wenn Absenderaddresse nicht gesetzt */
    addresses->from->mailbox = gen_from_mailbox();			/* Dann eine Addresse nach dem schema user@host generieren */
  }

  if((fd = tmpfile()) == NULL)						/* Temporäre Datei anlegen */
    return 0;

  build_header(addresses->from, addresses->to, addresses->cc, addresses->bcc, subject, fd);	/* Header in die Temporäre Datei schreiben */
  
  i = 0;
  while((buf = line_read_cb(i)) != NULL){		 		/* Solange die Callbackfunktion einen Zeiger auf einen Puffer liefert */
    fputs(buf, fd);							/* Schreibe dessen Inhalt in eine Datei */
    i++;
  }

  if((session = smtp_create_session()) == NULL)				/* SMTP - Session erstellen */
    return 0;

  switch (servopts->ssl_use) {						/* gucken ob SSL benutzt werden soll */
    case SSL_ENABLED:
      smtp_starttls_enable(session, Starttls_ENABLED);			/* SSL soll auf jeden Fall benutzt werden */
      break;
    case SSL_REQUIRED:
      smtp_starttls_enable(session, Starttls_REQUIRED);			/* SSL soll ren benötigt benutzt werden */
      break;
    default:
      smtp_starttls_enable(session, Starttls_DISABLED);			/* SSL soll nicht benutzt werden */
      break;
  }


  if(!smtp_set_server(session, "atlantis.wh2.tu-dresden.de:25"))	/* Den Server für die Übertragung setzen */
    return 0;

  smtp_set_eventcb(session, event_cb, servopts);			/* Callbackfunktion angeben, welche die Events der Verbindung (falsches Zertifikat, ..) hanhelt */

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
  DEBUGOUT3( "SMTP: sending result: %d %s", status->code, status->text);	/* und anzeigen */
  
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

static void event_cb (smtp_session_t session, int event_no, void *arg,...){   
  va_list alist;
  int *ok;
  server_vars *servopt = NULL;

  va_start(alist, arg);
  servopt = (server_vars*)arg;
  switch(event_no) {
    case SMTP_EV_CONNECT:
    case SMTP_EV_MAILSTATUS:
    case SMTP_EV_RCPTSTATUS:
    case SMTP_EV_MESSAGEDATA:
    case SMTP_EV_MESSAGESENT:
    case SMTP_EV_DISCONNECT: 
      break;
    case SMTP_EV_WEAK_CIPHER: {
      int bits;
      bits = va_arg(alist, long); 
      ok = va_arg(alist, int*);
      DEBUGOUT3("SMTP: Weak cipher! bits:%d continue: %d\n", bits, servopt->ssl_ctx_peer_wrong);
      *ok = servopt->ssl_weak_cipher; 
      break;
    }
    case SMTP_EV_STARTTLS_OK: {
      DEBUGOUT1("SMTP: Starting using TLS ... OK\n");
      break;
    }
    case SMTP_EV_INVALID_PEER_CERTIFICATE: {
      long vfy_result;            
      vfy_result = va_arg(alist, long); 
      ok = va_arg(alist, int*);
      DEBUGOUT3("SMTP: Invalid peer certificate! verify-Result: %d continue: %d\n", vfy_result,  servopt->ssl_ctx_peer_invalid);
      *ok = servopt->ssl_ctx_peer_invalid;
      break;
    }
    case SMTP_EV_NO_PEER_CERTIFICATE: {
      ok = va_arg(alist, int*); 
      DEBUGOUT2("SMTP: No peer certificate! continue: %d\n", servopt->ssl_ctx_peer_no);
      *ok = servopt->ssl_ctx_peer_no; 
      break;
    }
    case SMTP_EV_WRONG_PEER_CERTIFICATE: {
      ok = va_arg(alist, int*);
      DEBUGOUT2("SMTP: Wrong peer certificate! continue: %d\n", servopt->ssl_ctx_peer_wrong);
      *ok = servopt->ssl_ctx_peer_wrong; 
      break;
    }
    case SMTP_EV_NO_CLIENT_CERTIFICATE: {
      ok = va_arg(alist, int*);
      DEBUGOUT2("SMTP: No client certificate! continue: %d\n", servopt->ssl_ctx_peer_wrong);
      *ok = servopt->ssl_ctx_client_no; 
      break;       
    }
    case SMTP_EV_EXTNA_DSN: {
      DEBUGOUT1("SMTP: Extension DSN not supportet by MTA\n");
      break;
    }
    case SMTP_EV_EXTNA_STARTTLS: {
      DEBUGOUT1("SMTP: Extension StartTLS not supportet by MTA\n");
      break;
    }
    case SMTP_EV_EXTNA_8BITMIME: {
      DEBUGOUT1("SMTP: Extension 8BITMIME not supportet by MTA\n");
      break;
    }
    default:
      DEBUGOUT2("SMTP: Unhandled event ID: %d - ignored\n", event_no);
  }
  va_end(alist);
}


server_vars *get_default_servopts(){
  server_vars *servopts = malloc(sizeof(server_vars));
  servopts->host 			= "localhost";
  servopts->port 			= 25;
  servopts->ssl_use 			= SSL_REQUIRED;
  servopts->ssl_weak_cipher 		= SSL_ACCEPT;
  servopts->ssl_ctx_client_no 		= SSL_ACCEPT;
  servopts->ssl_ctx_peer_no 		= SSL_ACCEPT;
  servopts->ssl_ctx_peer_wrong 		= SSL_ACCEPT;
  servopts->ssl_ctx_peer_invalid 	= SSL_ACCEPT;
  servopts->auth_use			= AUTH_NO;
  servopts->auth_user			= NULL;
  servopts->auth_pass			= NULL;
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

  server_vars *servo = get_default_servopts(); 
  //servo->ssl_use = SSL_ENABLED;
  //servo->ssl_use = SSL_DISABLED;
  servo->host = "atlantis.wh2.tu-dresden.de";
  servo->port = 465;

  ft = fopen("test_data", "r");
  rewind(ft);
printf("rufe mail_message auf...\n");
 mail_message( &adr,  "Powered by libesmtp", 0, test_read, servo);
  

  printf("fertig...\n");
  exit(0);
}

