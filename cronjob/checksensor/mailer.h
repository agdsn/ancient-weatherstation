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
 
