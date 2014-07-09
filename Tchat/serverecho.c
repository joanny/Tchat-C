
#include <stdio.h>      
#include <stdlib.h>     
#include <unistd.h>     
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>  
#include <netdb.h>      


void echo( int sock ) {
    int rc;
    socklen_t size;
    char buffer [4096];
    struct sockaddr_in addr;
    size = sizeof(addr);
    while (0<1) {
      rc=recvfrom(sock,buffer,4096,0,(struct sockaddr *)&addr,&size);

      /* affiche l'adrese de l'envoyeur */
      printf("Got a datagram from %s port %d\n",inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

      if (rc<0) {
        perror("Error recvfrom");
      } else {
        printf("GOT %d bits\n",rc);
        sendto(sock,buffer,rc,0,(struct sockaddr *)&addr,size);
      }
    }
}


int main() {
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
  echo(sock);
  return(0);
}
