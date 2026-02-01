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
      if (setsockopt(udpSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
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
      
          
          char response[DNS_HEADER_SIZE];

          memset(response,0,sizeof(response));

         dns_header *header=(dns_header *)response;
         header->packet_id=htons(1234);
         u16 flags=0;


         flags |= (1 << 15); 
         flags |= (0 << 11);  
         flags |= (0 << 10);  
         flags |= (0 << 9);    
         flags |= (0 << 8);  
         flags |= (0 << 7);   
         flags |= (0 << 0); 
         
         header->flags=htons(flags);
          
         header->answer_record_count=htons(0);
         header->authority_record_count=htons(0);
         header->additional_record_count=htons(0);


      
         
          if (sendto(udpSocket, response, sizeof(response), 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress)) == -1) {
              perror("Failed to send response");
          }
      }
      
      close(udpSocket);
}



void encode_qname(u8 *buffer,const i8 *host){

   /*
      
      The format for the DNS encoding is [the number of bytes of a label][label characters]....and we repeat the same thing for .com ,
      ignoring the . 
      for example :example.com will be 
      [7]['e']['x']['a']['m']['p']['l']['e'][3]['c']['o']['m'][0]
      the zero at the end is the terminator

   */
       
   const i8 *ptr=host;//pointer to the current character
   u8 *len=buffer;//position to add the current label's length
   u8 *dest=buffer+1;//position to add the current character
   i32 count=0;//number of characters of the current label

   while(*ptr){
       if(*ptr=='.'){
           *len=count;
           len=dest++;
           count=0;
       }else{
           *dest++=*ptr;
           count++;
       }

       ptr++;

   }

   *len=count;
   *dest++=0;


}