#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <arpa/inet.h>


#define PORT 4200


struct client {
    char name[9];
    struct sockaddr_in addr;
};

#define NUM_CLIENTS 4
struct client clients[NUM_CLIENTS];

int sock;

/*  return 0 on success, -1 if the name is taken, -2 if full.
*/
int
add_client(const char *name, const struct sockaddr_in *addr)
{
    int i;
    for (i = 0; i < NUM_CLIENTS; i++) {
        if (strcmp(clients[i].name, name) == 0) {
            return -1;
        }
    }
    for (i = 0; i < NUM_CLIENTS; i++) {
        if (*clients[i].name == '\0') {
            printf("Adding %s\n", name);
            strncpy(clients[i].name, name, 8);
            clients[i].addr = *addr;
            return 0;
        }
    }
    return -2;
}

void
del_client(const char *name)
{
    int i;
    for (i = 0; i < NUM_CLIENTS; i++) {
        if (strcmp(clients[i].name, name) == 0) {
            printf("Removing %s\n", clients[i].name);
            *clients[i].name = '\0';
            return;
        }
    }
}

void
del_client_by_addr(const struct sockaddr_in *addr)
{
    int i;
    for (i = 0; i < NUM_CLIENTS; i++) {
        if (memcmp(&clients[i].addr, addr, sizeof(*addr)) == 0) {
            printf("Removing %s\n", clients[i].name);
            *clients[i].name = '\0';
            return;
        }
    }
}

void
skip_whitespaces(char **buf)
{
    while (**buf && (**buf == ' ' || **buf == '\n' || **buf == '\r'))
        (*buf) ++;
}

void
skip_non_whitespaces(char **buf)
{
    while (**buf && !(**buf == ' ' || **buf == '\n' || **buf == '\r'))
        ++*buf;
}

/*
  return NULL if there is no more data (only white spaces)
         buf otherwise, and in this case make buf point to following word.  It
             will modify the buffer (insert '\0')
*/
char *
get_next_word(char **buf)
{
    char *result;
    skip_whitespaces(buf);
    result = *buf;
    skip_non_whitespaces(buf);
    **buf = '\0';
    ++*buf;
    skip_whitespaces(buf);
    return result;
}


static inline int
send_to_client(struct client *client, const char *buffer)
{
  return 0;
}

void 
sendbyname(char * name, char * buffer, int size){
  int i=0;
  for (i = 0; i < NUM_CLIENTS; i++) {
  if ( strcmp(name,clients[i].name)==0){
    printf("envoie privée\n");
      sendto(sock,buffer,size,0,(struct sockaddr *)&clients[i].addr,sizeof(clients[i].addr)); 
        
    }
  }
}

void
send_to_all(const char* buffer, int size, int sock, char * name)
{
  int i = 0;
  for (i = 0; i < NUM_CLIENTS; i++) {
    /* printf("sending %s\n", clients[i].name);*/
    sendto(sock,name,8,0,(struct sockaddr *)&clients[i].addr,sizeof(clients[i].addr)); 
    sendto(sock,buffer,size,0,(struct sockaddr *)&clients[i].addr,sizeof(clients[i].addr)); 
    
  }
}
/* test si une addresse est déja client*/
int est_client( struct sockaddr_in addr){
  int comp = 0;
  int i = 0;
  /* l'addresse appartient elle a quelqu'un connecté ? Ici on devrait comparer les addr.sin_addr au lieu 
     des ports mais les clients ont les memes ip  puisqu'ils tournent sur la meme machine*/  
  for (i = 0; i < NUM_CLIENTS; i++) {
    if (ntohs(clients[i].addr.sin_port)== ntohs(addr.sin_port)) {
      comp++;
    }
  }
  /* Si elle n'appartient pas a quelqu'un de connecté */
  if ( comp < 1){
    printf("intrusion_ou_fausse_commande\n");
    return 0;
  }
  return 1;
}

char * get_name( struct sockaddr_in addr){
  int i;
  char * name;
  for (i = 0; i < NUM_CLIENTS; i++) {
    /* Ici aussi on devrait comparer des addr.sin_addr*/
    if (ntohs(clients[i].addr.sin_port)== ntohs(addr.sin_port)) {
      name = clients[i].name;
    }
  }
  return name;
}
/*Recois les messages de tous les clients et les interpréte*/
void go( int sock ) {
   
  int rc;
  int i ;
  char * c;
  char * mot;
  char * name;
  int add;
  socklen_t size;
  char buffer [513];
  struct sockaddr_in addr;
  size = sizeof(addr);
  while (0<1) {
      rc=recvfrom(sock,buffer,513,0,(struct sockaddr *)&addr,&size);
      c = buffer;
      mot = get_next_word(&c);
      /* affiche l'adresse de l'envoyeur */
      printf("Donnée recu de  %s port %d\n",inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
      if (rc<0) {
        perror("Error recvfrom");
      } else {

	/* Login */
	if (strcmp(mot,"#login")==0){
	  name = get_next_word(&c);
	  add = add_client(name,&addr);
	  if(add==0){
	  sendto(sock,"Bienvenue",9,0,(struct sockaddr *)&addr,sizeof(addr));
	  }else if (add < 0){
	   sendto(sock,"Essayer un autre nom",20,0,(struct sockaddr *)&addr,sizeof(addr));
	  }
	  /* Who */
	}else if (strcmp(mot,"#who")==0){
	  if ( est_client(addr)==1){
	    for (i = 0; i < NUM_CLIENTS; i++) {
	      sendto(sock,clients[i].name,8,0,(struct sockaddr *)&addr,sizeof(addr)); 	      
	    }
	  }
	}
	/* To */
	else if (strcmp(mot,"#to")==0){
	   if ( est_client(addr)==1){
	     name = get_next_word(&c);
	     sendbyname(name,c,rc);
	   }
	
	   /* Quit */
	}else if (strcmp(mot,"#quit")==0){
	  if( est_client(addr)==1){
	    del_client_by_addr(&addr);
	  }
	
	  /* Kick */
	}else if (strcmp(mot,"#kick")==0){
	  if( est_client(addr)==1){
	  name = get_next_word(&c);
	  del_client(name);
	  }
	}else{
	  if ( est_client(addr)==1){
	    name = get_name(addr);
	    send_to_all(buffer,rc,sock,name);
	  }
	    
	}
      }
    }
}	 


int
main(int argc, char **argv)
{
    int sock,rc,bd;
    struct sockaddr_in addr;
    socklen_t size;
    size = sizeof(addr);
    sock = socket( PF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
      printf("Error socket");
      exit(1);
    }
    addr.sin_family = AF_INET;;
    addr.sin_port = htons(4200);
    rc = inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr);
    if (rc<0) {
      perror("inet_pton");
      exit(1);
    }
    bd = bind(sock,(struct sockaddr * )&addr,size);
    if (bd<0) {
      printf("Error bind");
      exit(0);
    }
    go(sock);
    return 0;
}
