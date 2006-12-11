/*
   mailer.h      -- Wrapper for libesmtp

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


#include <auth-client.h>

/* SSL-Benutzung */
#define SSL_ENABLED   0 			/* auf jeden Fall eingschaltet */ 
#define SSL_DISABLED  1				/* Ausgeschaltet */
#define SSL_REQUIRED  2				/* Wenn benoetigt */

/* Zertifikat akzeptieren */
#define SSL_ACCEPT    1				/* Ja */
#define SSL_DECLINE   0				/* Nein */

/* Authentifizieren */
#define AUTH_YES      1				/* Ja */
#define AUTH_NO	      0				/* Nein */

/* eightbit setzen */
#define EINGHTBIT_Y   1				/* ja */
#define EINGHTBIT_N   0				/*nein */

/* Rueckgabestatus */
#define MAILER_STATUS_OK			0	/* Erfolgreich versandt */
#define MAILER_STATUS_FAILTURE_SERVOPTS_MISSING	1	/* Server-einstellungen fehlen */
#define MAILER_STATUS_FAILTURE_TEMPFILE_CREATE	2	/* Tempfile kann nicht angelegt werden */
#define MAILER_STATUS_FAILTURE_SESSION_CREATE	3	/* Session kann nicht erzeugt werden */
#define MAILER_STATUS_FAILTURE_SETSERVER	4	/* Server kann nicht gesetzt werden */
#define MAILER_STATUS_FAILTURE_CREATEAUTHCTX	5	/* Authentifizierungskontext kann nicht angelegt werden */
#define MAILER_STATUS_FAILTURE_SETAUTHCTX	6	/* Authentifizierungskontext kann nicht gesetzt werden */
#define MAILER_STATUS_FAILTURE_ADDMESSAGE	7	/* Message kann nicht hinzugefuegt werden */
#define MAILER_STATUS_FAILTURE_SETREVPATH	8	/* Serveraddresse kann nicht gesetzt werden */
#define MAILER_STATUS_FAILTURE_SETMESSAGECB	9	/* Message-Callback-fkt kann nicht gesetzt werden */
#define MAILER_STATUS_FAILTURE_ADDRECIPIENTS	10	/* Empfaenger kkoennen nicht hinzugefuegt werden */
#define MAILER_STATUS_FAILTURE_STARTSESSION	11	/* Session kann nicht gestartet werden */
#define MAILER_STATUS_FAILTURE_SENDING		12 	/* Fehler beim senden */
#define MAILER_STATUS_FAILTURE_CREATE_HEADER	13	/* Header kann nicht gebaut werden */


/* Die Callback-fkt. zum lesen einer Zeile. 
 * das 1. Argument ist die momentane Zeilennummer (beginnend bei 0) 
 * das 2. Argument ist das Argument, welches der mail_message-fkt. uebergeben wurde */
typedef char *(*mail_linereader_cb)(int line, void *arg);

/* Datenstruktur fuer die Addressliste */
typedef struct address_t {
  char 		   *mailbox;          		/* mail-Addresse */
  struct address_t *next;			/* Zeiger auf das naechste Element */
} address_struct;

/* Zusammenfassung jeglicher Addressen */
typedef struct address_all_t {
  address_struct *from;				/* Absender */
  address_struct *to;				/* Empfaenger */
  address_struct *cc;				/* cc */
  address_struct *bcc;				/* bcc */
} address_all_struct;			
 

/* Datenstruktur fuer die Server-Optionen */
typedef struct server_vars_t {
  char  		*host;			/* host */
  int   		port;			/* port */
  char  		ssl_use;		/* soll ssl benutzt werden ? */
  char			ssl_weak_cipher;	/* was bei schwacher Verschluesselung */
  char			ssl_ctx_client_no;	/* was bei fehlendem client-zertifikat */
  char 			ssl_ctx_peer_no;	/* was bei fehlendem server-zertifikat */
  char			ssl_ctx_peer_wrong;	/* was bei falschem server-zertifikat */
  char  		ssl_ctx_peer_invalid;	/* was bei ungueltigem server-zertifikat */
  char  		auth_use;		/* soll authentifiziert werden */
  auth_interact_t 	auth_cb;		/* Callbackfkt. zum authentifizieren. wenn NULL, dan werden die werte drunter benutzt */  
  char  		*auth_user;		/* default-user */
  char			*auth_pass;		/* default-password */
}server_vars;

/* Funktion zum versenden der Mails.
 * 1. Argument: jegliche Mail-Addressen
 * 2. Argument: Betreff der Nachicht
 * 3. Argument: eightbit-flag setzen oder nicht 
 * 4. Argument: Callback-Fkt. zum lesen einer Zeile
 * 5. Argument: Argument fuer die Callback-Fkt.
 * 6. Argument: Server-Einstellungen 
 * gibt einen Statusnummer zurueck */
int mail_message(address_all_struct *,  char *, int, mail_linereader_cb, void *, server_vars *);


/* Gibt ein grundgeruest mit default-servereinstellungen
 * zurueck, damit man nicht immer alles selbst angeben muss */
server_vars *get_default_servopts();


/* Liefert einen Text zur Statusmeldung */
const char *get_mail_status_text(int error_no);

