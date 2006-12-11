/*
   mailer.c      -- Wrapper for libesmtp

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

/* Compiling:
 * it uses libesmtp and libssl, so you must add
 * -lesmtp and -lssl for linking.
 * Debug-output can be activated with -DDEBUG as 
 * compile-option. */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libesmtp.h>
#include <auth-client.h>
#include <openssl/ssl.h>
#include "mailer.h"

#define BUFFSIZE 2048

#ifndef DEFAULT_MAIL_USER
//  #define DEFAULT_MAIL_USER "mailer.c"
  #define DEFAULT_MAIL_USER	"weatherstation"
#endif

#define TO_LINE "To: "
#define CC_LINE "Cc: "
#define BCC_LINE "Bcc: "

/* Debug --------------------------------------------------------------- */
#ifdef DEBUG
  #define DEBUGOUT1(x)         fprintf(stderr,x)
  #define DEBUGOUT2(x,y)       fprintf(stderr,x,y)
  #define DEBUGOUT3(x,y,z)     fprintf(stderr,x,y,z)
  #define DEBUGOUT4(x,y,z,a)   fprintf(stderr,x,y,z,a)
  #define DEBUGOUT5(x,y,z,a,b) fprintf(stderr,x,y,z,a,b)
  #define FOR(x)               for(x)
#else
  #define DEBUGOUT1(x)
  #define DEBUGOUT2(x,y)
  #define DEBUGOUT3(x,y,z)
  #define DEBUGOUT4(x,y,z,a)
  #define DEBUGOUT5(x,y,z,a,b)    
  #define FOR(x) 
#endif  


/* Funktionen */
static int build_header( address_struct *,   address_struct *,  address_struct *,  address_struct *, char *, FILE *);
static char * gen_from_mailbox();
static char * gen_address_line(char *, const char *, address_struct *);
static const char *read_mail_tmp_file (void **, int *, void *);
static int add_recipients(smtp_message_t , address_struct *);
static void event_cb (smtp_session_t , int , void *,...);   
static int default_auth_cb (auth_client_request_t , char **, int , void *);


/* Funktion zum versenden von Mails */
int mail_message(address_all_struct *addresses,  char *subject, int eightbit, mail_linereader_cb line_read_cb, void *line_read_cb_arg, server_vars *servopts){
  
  FILE *fd 			= NULL;					/* Zeiger auf Temporaere Datei */
  int i				= 0;					/* Laufvariable */
  char *buf			= NULL;					/* Puffer zum anlegen der temp. Datei */
  char *hostportstr		= NULL;					/* host:port */
  int hostportlen		= 0;					/* strlen(host:port) */
  smtp_session_t session 	= NULL;					/* SMTP - Session */
  smtp_message_t message 	= NULL;					/* SMTP - Message */
  auth_context_t authctx	= NULL;					/* Kontext zum authentifizieren */
  const smtp_status_t *status	= NULL;					/* Uebertragungsstatus */

  if(servopts == NULL)
    return MAILER_STATUS_FAILTURE_SERVOPTS_MISSING;

  if(addresses->from == NULL){						/* Wenn absenderstruktur nicht gesetzt ist */
    addresses->from = malloc(sizeof(address_struct));			/* Dann speicher allokieren */
    addresses->from->mailbox  = NULL;					/* struktur initialisieren (NULL-Pointer als addresse) */
    addresses->from->next     = NULL;					/* nachfolgezeiger auf NULL setzen (gibt eh nur einen absender) */
  }

  if(addresses->from->mailbox == NULL){					/* Wenn Absenderaddresse nicht gesetzt */
    addresses->from->mailbox = gen_from_mailbox();			/* Dann eine Addresse nach dem schema user@host generieren */
  }

  if((fd = tmpfile()) == NULL)						/* Temporaere Datei anlegen */
    return MAILER_STATUS_FAILTURE_TEMPFILE_CREATE;

  if(!build_header(addresses->from, addresses->to, addresses->cc, addresses->bcc, subject, fd))	/* Header in die Temporaere Datei schreiben */
    return MAILER_STATUS_FAILTURE_CREATE_HEADER;  

  i = 0;
  while((buf = line_read_cb(i, line_read_cb_arg)) != NULL){ 		/* Solange die Callbackfunktion einen Zeiger auf einen Puffer liefert */
    fputs(buf, fd);							/* Schreibe dessen Inhalt in eine Datei */
    i++;
  }

  if((session = smtp_create_session()) == NULL)				/* SMTP - Session erstellen */
    return MAILER_STATUS_FAILTURE_SESSION_CREATE;

  switch (servopts->ssl_use) {						/* gucken ob SSL benutzt werden soll */
    case SSL_ENABLED:
      smtp_starttls_enable(session, Starttls_ENABLED);			/* SSL soll auf jeden Fall benutzt werden */
      break;
    case SSL_REQUIRED:
      smtp_starttls_enable(session, Starttls_REQUIRED);			/* SSL soll ren benoetigt benutzt werden */
      break;
    default:
      smtp_starttls_enable(session, Starttls_DISABLED);			/* SSL soll nicht benutzt werden */
      break;
  }

  hostportlen = strlen(servopts->host)+7;				/* (hostnamlen + doppelpkt + max-portlen (65536 => 5 zeichen) + Nullterm.szeichen */
  hostportstr = malloc(sizeof(char)*hostportlen);			/* Speicher fuer host:port belegen */
  snprintf(hostportstr, hostportlen, "%s:%d", servopts->host, servopts->port); 	/* host:port zusammenbauen */
  DEBUGOUT2("SMTP: Send Mail over: \"%s\"\n",hostportstr);

  if(!smtp_set_server(session, hostportstr))				/* Den Server fuer die Uebertragung setzen */
    return MAILER_STATUS_FAILTURE_SETSERVER;

  if (servopts->auth_use) {						/* Wenn Authentifizierung eingeschaltet */
    if ((authctx = auth_create_context ()) == NULL)			/* Authentifizierungskontext erstellen */
      return MAILER_STATUS_FAILTURE_CREATEAUTHCTX;
    auth_set_mechanism_flags (authctx, AUTH_PLUGIN_PLAIN, 0);		/* Plain-Text-Plugin auswaehlen */
    if(servopts->auth_cb == NULL){					/* wenn keine Callback-fkt. gesetzt */
      auth_set_interact_cb (authctx, default_auth_cb, servopts);	/* dann die standart-fkt nehmen */
    } else {								/* sonst */
      auth_set_interact_cb (authctx, servopts->auth_cb, servopts);	/* die gegebene verwenden */
    }
    if (!smtp_auth_set_context (session, authctx))			/* Kontext der session uebergeben */
      return MAILER_STATUS_FAILTURE_SETAUTHCTX; 
  } 

  smtp_set_eventcb(session, event_cb, servopts);			/* Callbackfunktion angeben, welche die Events der Verbindung (falsches Zertifikat, ..) hanhelt */

  if((message = smtp_add_message(session)) == NULL)			/* Eine Nachicht zur Session hinzufuegen */
    return MAILER_STATUS_FAILTURE_ADDMESSAGE; 

  if(! smtp_set_reverse_path(message,addresses->from->mailbox))		/* Absenderaddresse setzen */
    return MAILER_STATUS_FAILTURE_SETREVPATH;

  if(eightbit)								/* Weinn eightbit-Flag gesetzt, */
    smtp_8bitmime_set_body(message, E8bitmime_8BITMIME);		/* dann das auch in der libesmtp setzen */

  if(!smtp_set_messagecb(message, read_mail_tmp_file, fd))		/* Callback - Funktion zum lesen der temp. Datei */
    return MAILER_STATUS_FAILTURE_SETMESSAGECB;

  if(!(add_recipients(message, addresses->to) &&  add_recipients(message, addresses->cc) && add_recipients(message, addresses->bcc) ) )		/* Empfaenger setzen */
    return MAILER_STATUS_FAILTURE_ADDRECIPIENTS;

  if(!smtp_start_session(session))					/* Session Starten (Mail versenden) */
    return MAILER_STATUS_FAILTURE_STARTSESSION;

  status = smtp_message_transfer_status (message);			/* uebertragungsstatus holen */
  DEBUGOUT3( "SMTP: sending result: %d %s", status->code, status->text);	/* und anzeigen */

  if (status->code < 200 || status->code > 299) {
    return MAILER_STATUS_FAILTURE_SENDING;
  }
  
  /* aufraeumen */
  if(fd != NULL)
    fclose(fd);
  if(hostportstr != NULL)
    free(hostportstr);
  if(session != NULL)
    smtp_destroy_session(session);
  if(authctx != NULL)
    auth_destroy_context(authctx);

  return MAILER_STATUS_OK;
}


/* Baut den Mail-header zusammen 
 * from ist die Liste mit den Absenderaddressen (sollte nur eine sein) 
 * to ist diie Liste der Emfaenger
 * cc ist die Liste der CC - Empfaenger
 * bcc iste die Liste der BCC - Empfaenger 
 * subject ist der Betreff der Mail
 * fd ist der Zeiger auf die temp. Datei */
static int build_header(address_struct * from,  address_struct *to,  address_struct *cc,  address_struct *bcc, char *subject, FILE *fd){
  char *buf;								/* Puffer fuer die Zeilen */
  if(from == NULL || to == NULL || fd == NULL){				/* wenn absender, Empfaenger oder Dateizeiger NULL */
    return 0;								/* Dann abbrechen */
  }
  buf = malloc(sizeof(char)*BUFFSIZE);					/* Speicher fuer Puffer holen */

  fprintf(fd, "From: <%s>\r\n", from->mailbox);				/* Absender setzen */
  fputs(gen_address_line(buf, TO_LINE, to), fd);			/* To setzen */
  fputs(gen_address_line(buf, CC_LINE, cc), fd);			/* Cc setzen */
  fputs(gen_address_line(buf, BCC_LINE, bcc), fd);			/* Bcc setzen */
  fprintf(fd, "Subject: %s\r\n", subject);				/* Betreff setzen */
  fputs("Reply-To:\r\n\r\n", fd);					/* Reply-To + Leerzeile anhaengen */

  free(buf);								/* Puffer freigeben */
  
  return 1; 
}


/* Baut eine neue Absenderaddresse nach der Form user@host */
#define MAXHOSTNAMELEN 	256
static char * gen_from_mailbox(){
  char *host, *user, *new_from;	

  host = malloc(sizeof(char)*MAXHOSTNAMELEN);				/* Speicher fuer den Hostnamen holen */
  user = getlogin();							/* den Usernamen holen */
  if(user == NULL)
    user = DEFAULT_MAIL_USER;
  
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
  address_struct *addr = address;			  		/* Temporaerer Zeiger */
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
  strcat(buf, "\r\n");							/* Wagenruecklauf + Zeilenvorschub anhaengen */
  return buf;
}


/* Mail aus temporaerer Datei lesen (Callbackfkt.) 
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
      strcpy (p - 1, "\r\n");						/* dann einen ergaenzen */
      p++;								/* und den String ein Zeichen laenger machen */
    }
    octets = p - (char *) *buf;						/* Gelesene Zeichen berechnen */
  }

  *len = octets;
  return *buf;
}


/* Empfaenger zu einer Mail hinzufuegen 
 * message ist die SMTP - Message 
 * address ist die Liste mit den Empfaengeraddressen */
static int add_recipients(smtp_message_t message, address_struct *address){
  address_struct *adr = address;					/* Temp. Zeiger auf die Addressen */
  for(; adr; adr = adr->next){						/* Addressen durchgehen */
    if(adr->mailbox){							/* Wenn eine gueltige gefunden */
      if(!smtp_add_recipient(message, adr->mailbox))			/* Dann hinzufuegen */
	return 0;
    }
  }
  return 1;
}


/* Event-Callback-Funktion.
 * Die wird von der Libsmtp aufgerufen wenn waehrend der Session irgend etwas
 * (unabhoengig ob gut oder nicht gut) passiert. 
 * dadurch hat man die Moeglichkeit auf solche Ereignisse zu reagieren */
static void event_cb (smtp_session_t session, int event_no, void *arg,...){   
  va_list alist;							/* Argumentliste */
  int *ok;								/* Rueckgabewert an die lib */
  server_vars *servopt = NULL;						/* Server-Optionen, um zu entscheiden was bei einem Event getan werden soll */

  va_start(alist, arg);							/* Argumentliste initialisieren */
  servopt = (server_vars*)arg;						/* Server-Optionen zuweisen */
  switch(event_no) {							/* Eventnummer feststellen */
    case SMTP_EV_CONNECT:						/* ignorieren */
    case SMTP_EV_MAILSTATUS:						/* ignorieren */
    case SMTP_EV_RCPTSTATUS:						/* ignorieren */
    case SMTP_EV_MESSAGEDATA:						/* ignorieren */
    case SMTP_EV_MESSAGESENT:						/* ignorieren */
    case SMTP_EV_DISCONNECT: 						/* ignorieren */
      break;
    case SMTP_EV_WEAK_CIPHER: {						/* Schwache Verschluesslung */
      int bits;								/* Anzahl der Verschluesslungsbits */
      bits = va_arg(alist, long); 					/* aus atgliste holen und zuweisen */
      ok = va_arg(alist, int*);						/* rueckgabezeiger bekommen */
      DEBUGOUT3("SMTP: Weak cipher! bits:%d continue: %d\n", bits, servopt->ssl_ctx_peer_wrong);
      *ok = servopt->ssl_weak_cipher; 					/* rueckgabewert setzen */
      break;
    }
    case SMTP_EV_STARTTLS_OK: {						/* TLS wird benutzt */
      DEBUGOUT1("SMTP: Starting using TLS ... OK\n");
      break;
    }
    case SMTP_EV_INVALID_PEER_CERTIFICATE: {				/* Ungueltiges Server-Zertifikat */
      long vfy_result;            					/* pruefungs-resultat */
      vfy_result = va_arg(alist, long); 
      ok = va_arg(alist, int*);
      DEBUGOUT3("SMTP: Invalid peer certificate! verify-Result: %d continue: %d\n", vfy_result,  servopt->ssl_ctx_peer_invalid);
      *ok = servopt->ssl_ctx_peer_invalid;				/* rueckgabewert setzen */
      break;
    }
    case SMTP_EV_NO_PEER_CERTIFICATE: {					/* kein Server-Zertifikat */
      ok = va_arg(alist, int*); 
      DEBUGOUT2("SMTP: No peer certificate! continue: %d\n", servopt->ssl_ctx_peer_no);
      *ok = servopt->ssl_ctx_peer_no; 					/* rueckgabewert setzen */
      break;
    }
    case SMTP_EV_WRONG_PEER_CERTIFICATE: {				/* falsches Server-Zertifikat */
      ok = va_arg(alist, int*);
      DEBUGOUT2("SMTP: Wrong peer certificate! continue: %d\n", servopt->ssl_ctx_peer_wrong);
      *ok = servopt->ssl_ctx_peer_wrong; 				/* rueckgabewert setzen */
      break;
    }
    case SMTP_EV_NO_CLIENT_CERTIFICATE: {				/* kein Client-Zertifikat */
      ok = va_arg(alist, int*);
      DEBUGOUT2("SMTP: No client certificate! continue: %d\n", servopt->ssl_ctx_peer_wrong);
      *ok = servopt->ssl_ctx_client_no; 				/* rueckgabewert setzen */
      break;       
    }
    case SMTP_EV_EXTNA_DSN: {						/* DSN ist nicht verfuegbar */
      DEBUGOUT1("SMTP: Extension DSN not supportet by MTA\n");
      break;
    }
    case SMTP_EV_EXTNA_STARTTLS: {					/* StartTLS ist nicht verfuegbar */
      DEBUGOUT1("SMTP: Extension StartTLS not supportet by MTA\n");
      break;
    }
    case SMTP_EV_EXTNA_8BITMIME: {					/* 8BITMIME ist nicht verfuegbar */
      DEBUGOUT1("SMTP: Extension 8BITMIME not supportet by MTA\n");
      break;
    }
    default:								/* was unbekanntes ist angekommen */
      DEBUGOUT2("SMTP: Unhandled event ID: %d - ignored\n", event_no);
  }
  va_end(alist);
}


/* Wird genutzt, wenn keine andere Callback-fkt definiert ist.
 * Setzt die in den serveropts liegenden daten (user + passwd) */
static int default_auth_cb (auth_client_request_t request, char **result, int fields, void *arg){
  int i;								/* Laufvariable */

  DEBUGOUT1("SMTP: Authenticating...");

  for (i = 0; i < fields; i++) {					/* Felder durchlaufen */
    if (request[i].flags & AUTH_USER) {					/* Wenn username erfragt wird */
      result[i] = ((server_vars *)arg)->auth_user;			/* dann den uebergeben */
    } 
    else if (request[i].flags & AUTH_PASS) {				/* wenn nach Passwort gefragt wird */
      result[i] = ((server_vars *)arg)->auth_pass;			/* dann das uebergeben */
    }
  }

  DEBUGOUT1("OK\n");

  return 1;
}


/* Liefert ein Standart-options-Objekt,
 * damit nicht immer jede Option einzeln gesetzt werden muss */
server_vars *get_default_servopts(){
  server_vars *servopts = malloc(sizeof(server_vars));			/* Speicher belegen */
  servopts->host 			= "localhost";			/* default-host */
  servopts->port 			= 25;				/* default-Port */
  servopts->ssl_use 			= SSL_REQUIRED;			/* soll ssl benutzt werden ? */
  servopts->ssl_weak_cipher 		= SSL_ACCEPT;			/* was bei schwacher Verschluesselung */
  servopts->ssl_ctx_client_no 		= SSL_ACCEPT;			/* was bei fehlendem client-zertifikat */
  servopts->ssl_ctx_peer_no 		= SSL_ACCEPT;			/* was bei fehlendem server-zertifikat */
  servopts->ssl_ctx_peer_wrong 		= SSL_ACCEPT;			/* was bei falschem server-zertifikat */
  servopts->ssl_ctx_peer_invalid 	= SSL_ACCEPT;			/* was bei ungueltigem server-zertifikat */
  servopts->auth_use			= AUTH_NO;			/* soll authentifiziert werden */
  servopts->auth_cb			= NULL;				/* Auth-Callback-fkt */
  servopts->auth_user			= NULL;				/* default-user */
  servopts->auth_pass			= NULL;				/* default-password */
}


/* Statusmeldungen zurueckgeben */
const char *get_mail_status_text(int error_no){
  switch (error_no){
    case MAILER_STATUS_OK:
      return "SMTP: Sending Ok";
    case MAILER_STATUS_FAILTURE_SERVOPTS_MISSING:
      return "SMTP: Server-options missing";
    case MAILER_STATUS_FAILTURE_TEMPFILE_CREATE:	
      return "SMTP: Cannot create tempfile";
    case MAILER_STATUS_FAILTURE_SESSION_CREATE:	
      return "SMTP: Cannot create Session";
    case MAILER_STATUS_FAILTURE_SETSERVER:
      return "SMTP: Cannot set server";
    case MAILER_STATUS_FAILTURE_CREATEAUTHCTX:
      return "SMTP: Cannot create authentification context";
    case MAILER_STATUS_FAILTURE_SETAUTHCTX:
      return "SMTP: Cannot set authentification context";
    case MAILER_STATUS_FAILTURE_ADDMESSAGE:	
      return "SMTP: Cannot add message";
    case MAILER_STATUS_FAILTURE_SETREVPATH:	
      return "SMTP: Cannot set from-address";
    case MAILER_STATUS_FAILTURE_SETMESSAGECB:	
      return "SMTP: Cannot add message-callback";
    case MAILER_STATUS_FAILTURE_ADDRECIPIENTS:	
      return "SMTP: Cannot add recipients";
    case MAILER_STATUS_FAILTURE_STARTSESSION:	
      return "SMTP: Cannot start session";
    case MAILER_STATUS_FAILTURE_SENDING:
      return "SMTP: Failture while sending";
    case MAILER_STATUS_FAILTURE_CREATE_HEADER:
      return "SMTP: Cannot create header";
  }	
}
