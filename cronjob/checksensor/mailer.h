#include <auth-client.h>

#define SSL_ENABLED   0
#define SSL_DISABLED  1
#define SSL_REQUIRED  2

#define SSL_ACCEPT    1
#define SSL_DECLINE   0

#define AUTH_YES      1
#define AUTH_NO	      0

#define EINGHTBIT_Y   1
#define EINGHTBIT_N   0


/* Die Callback-fkt. zum lesen einer Zeile. das Argument ist die momentane Zeilennummer (beginnend bei 0) */
typedef char *(mail_linereader_cb)(int line);

/* Datenstruktur f�r die Addressliste */
typedef struct address_t {
  char *mailbox;                		/* mail-Addresse */
  struct address_t *next;			/* Zeiger auf das n�chste Element */
} address_struct;

/* Zusammenfassung jeglicher Addressen */
typedef struct address_all_t {
  address_struct *from;				/* Absender */
  address_struct *to;				/* Empf�nger */
  address_struct *cc;				/* cc */
  address_struct *bcc;				/* bcc */
} address_all_struct;			
 

/* Datenstruktur f�r die Server-Optionen */
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
 * 5. Argument: Server-Einstellungen */
int mail_message(address_all_struct *,  char *, int, mail_linereader_cb, server_vars *);


/* Gibt ein grundger�st mit default-servereinstellungen
 * zurueck, damit man nicht immer alles selbst angeben muss */
server_vars *get_default_servopts();
