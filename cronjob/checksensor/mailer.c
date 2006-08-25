#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libesmtp.h>
#include "definitions.h"
#include "mailer.h"

#define BUFFSIZE 2048

#define TO_LINE "To: "
#define CC_LINE "Cc: "
#define BCC_LINE "Bcc: "

FILE * ft = NULL;
char *bufft = NULL;

static int build_header( address_struct *,   address_struct *,  address_struct *,  address_struct *, char *, FILE *);
char * gen_from_mailbox();
static char * gen_address_line(char *, const char *, address_struct *);
static const char *read_mail_tmp_file (void **, int *, void *);
static int add_recipients(smtp_message_t , address_struct *);










int mail_message(address_all_struct *addresses,  char *subject, int eightbit, mail_linereader_cb line_read_cb){
  FILE *fd = NULL;
  int i;
  char *buf;
  smtp_session_t session;
  smtp_message_t message;
const smtp_status_t *status;

  if((fd = tmpfile()) == NULL)
    return 0;

  if(addresses->from == NULL){
    addresses->from = malloc(sizeof(address_struct));
    addresses->from->mailbox  = NULL;
    addresses->from->next     = NULL;
  }
  if(addresses->from->mailbox == NULL){
    addresses->from->mailbox = gen_from_mailbox();
  }

  build_header(addresses->from, addresses->to, addresses->cc, addresses->bcc, subject, fd);
  
  i = 0;
  while((buf = line_read_cb(i)) != NULL){
    fputs(buf, fd);
    i++;
  }

  if((session = smtp_create_session()) == NULL)
    return 0;

  if(!smtp_set_server(session, "atlantis.wh2.tu-dresden.de:25"))
    return 0;

  if((message = smtp_add_message(session)) == NULL)
    return 0; 

  if(! smtp_set_reverse_path(message,addresses->from->mailbox))
    return 0;

  if(!smtp_set_messagecb(message, read_mail_tmp_file, fd))
    return 0;

  if(!(add_recipients(message, addresses->to) &&  add_recipients(message, addresses->cc) && add_recipients(message, addresses->bcc) ) )
    return 0;

  if(!smtp_start_session(session))
    return 0;

  status = smtp_message_transfer_status (message);
  DEBUGOUT3( "SMTP sending: %d %s", status->code, status->text);
  
  return 1;
}


/* Baut den Mail-header zusammen */
static int build_header(address_struct * from,  address_struct *to,  address_struct *cc,  address_struct *bcc, char *subject, FILE *fd){
  char *buf;
  if(from == NULL || to == NULL || fd == NULL){
    return 0;
  }
  buf = malloc(sizeof(char)*BUFFSIZE);
  fprintf(fd, "From: %s\r\n", from->mailbox);
  fputs(gen_address_line(buf, TO_LINE, to), fd);
  fputs(gen_address_line(buf, CC_LINE, cc), fd);
  fputs(gen_address_line(buf, BCC_LINE, bcc), fd);
  fprintf(fd, "Subject: %s\r\n", subject);
  fputs("Reply-To:\r\n\r\n", fd);
  free(buf);
  return 1; 
}

/* Baut eine neue Absenderaddresse nach der Form user@host */
#define MAXHOSTNAMELEN 	256
char * gen_from_mailbox(){
  char *host, *user, *new_from;

  host = malloc(sizeof(char)*MAXHOSTNAMELEN);
  user = getlogin();
  gethostname(host, MAXHOSTNAMELEN);

  new_from = malloc(sizeof(char)*(MAXHOSTNAMELEN + strlen(user) + 10));
  snprintf(new_from, (MAXHOSTNAMELEN + strlen(user) + 10), "%s@%s",user,host);

  free(host);
  return new_from;
}

static char *  gen_address_line(char *buf, const char *line_type, address_struct *address){
  address_struct *addr = address;  
  int i = 0;

  strcpy(buf, line_type);
  for(; addr; addr = addr->next){
    if(i){
      buf = strcat(buf, ", ");
    }
    buf = strcat(buf, addr->mailbox);
    i++;
  }
  buf = strcat(buf, "\r\n");
  return buf;
}

static const char *read_mail_tmp_file (void **buf, int *len, void *arg){
  int octets;

  if (*buf == NULL)
    *buf = malloc (BUFFSIZE);

  if (len == NULL) {
    rewind ((FILE *) arg);
    return NULL;
  }

  if (fgets (*buf, BUFFSIZE - 2, (FILE *) arg) == NULL) {
    octets = 0;
  }
  else {
    char *p = strchr (*buf, '\0');

    if (p[-1] == '\n' && p[-2] != '\r') {
      strcpy (p - 1, "\r\n");
      p++;
    }
    octets = p - (char *) *buf;
  }

  *len = octets;
  return *buf;
}


static int add_recipients(smtp_message_t message, address_struct *address){
  address_struct *adr = address;
  for(; adr; adr = adr->next){
    if(adr->mailbox){
      if(!smtp_add_recipient(message, adr->mailbox))
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
  //to->next = NULL;
  to->next = malloc(sizeof(address_struct));
  to->next->mailbox = "jooschi@wh2.tu-dresden.de";
  to->next->next = NULL;
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

