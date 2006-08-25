#define SSL_ENABLED  0
#define SSL_DISABLED 1
#define SSL_REQUIRED 2

#define SSL_ACCEPT   1
#define SSL_DECLINE  0

#define AUTH_YES     1
#define AUTH_NO	     0

typedef char *(mail_linereader_cb)(int line);

typedef struct address_t {
  char *mailbox;                /* mailbox and host address */
  struct address_t *next;
} address_struct;

typedef struct address_all_t {
  address_struct *from;
  address_struct *to;
  address_struct *cc;
  address_struct *bcc;
} address_all_struct;
 
typedef struct server_vars_t {
  char  *host;
  int   port;
  char  ssl_use;
  char	ssl_weak_cipher;
  char	ssl_ctx_client_no;
  char 	ssl_ctx_peer_no;
  char	ssl_ctx_peer_wrong;
  char  ssl_ctx_peer_invalid;
  char  auth_use;
  char  *auth_user;
  char	*auth_pass;
}server_vars;
