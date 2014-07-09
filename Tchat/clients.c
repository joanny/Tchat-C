#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

void  tty_to_server( struct sockaddr_in addr, int sockfd ){
  char buffer [4096];
  int lus =0;
  socklen_t addr_size = sizeof(addr);
  printf ("entrer character");
  while (0<1) {
    lus = read(STDIN_FILENO,buffer,4096);
    if (lus<0){
      perror("read_problem");
      exit(1);
    }
  sendto(sockfd,buffer,lus,0,(struct sockaddr *)&addr,addr_size);
  }
}

void  server_to_tty( struct sockaddr_in addr, int sockfd ){
  int  size;
  char buffer [4096];
  socklen_t addr_size = sizeof(addr);
  while (0<1){
	printf("\n");
    size = recvfrom(sockfd,buffer,4096,0,(struct sockaddr *)&addr,&addr_size);
	
    if ( size<0 ){
      perror("error_size_recv");
      exit(1);
    }
  write ( STDOUT_FILENO,buffer,size);
  }

}




int main (int argc, char** argv){
  struct sockaddr_in addr;
  int sclient,rc,fd;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(4200);
  rc = inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr);
  if (rc<0) {
    perror("inet_pton");
    exit(1);
  }
  sclient = socket(AF_INET,SOCK_DGRAM,0);
  if (sclient<0){
    perror("sclient_error");
    exit(1);
  }
  fd= fork();
  if(fd==0){
    server_to_tty(addr,sclient);
  }
  else if (fd>0){
  tty_to_server(addr,sclient);
  }
 return 0;
}
