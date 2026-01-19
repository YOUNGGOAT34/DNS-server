#include "server.h"

#include <stdio.h>
#include<stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>



void server(void){

   
   setbuf(stdout, NULL);
       setbuf(stderr, NULL);
   
   
    i32 udpSocket;
    struct sockaddr_in clientAddress;
      
      udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
      if (udpSocket == -1) {
         fprintf(stderr,"Socket creation failed: %s...\n", strerror(errno));
         exit(EXIT_FAILURE);
      }
   
      int reuse = 1;
      if (setsockopt(udpSocket, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
         printf("SO_REUSEPORT failed: %s \n", strerror(errno));
         exit(EXIT_FAILURE);
      }
      
      struct sockaddr_in serv_addr = { .sin_family = AF_INET ,
                               .sin_port = htons(2053),
                               .sin_addr = { htonl(INADDR_ANY) },
                              };
      
      if (bind(udpSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
         printf("Bind failed: %s \n", strerror(errno));
            exit(EXIT_FAILURE);
      }
   
      int bytesRead;
      char buffer[512];
      socklen_t clientAddrLen = sizeof(clientAddress);
      
      while (1) {
         
          bytesRead = recvfrom(udpSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddress, &clientAddrLen);
          if (bytesRead == -1) {
              perror("Error receiving data");
              break;
          }
      
          buffer[bytesRead] = '\0';
          printf("Received %d bytes: %s\n", bytesRead, buffer);
      
          
          char response[1] = { '\0' };
      
         
          if (sendto(udpSocket, response, sizeof(response), 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress)) == -1) {
              perror("Failed to send response");
          }
      }
      
      close(udpSocket);
}