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
      char recv_buffer[512];
      socklen_t clientAddrLen = sizeof(clientAddress);
      
      while (1) {
         
          bytesRead = recvfrom(udpSocket, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr*)&clientAddress, &clientAddrLen);
          if (bytesRead == -1) {
              perror("Error receiving data");
              break;
          }
      
          recv_buffer[bytesRead] = '\0';

          printf("Received %d bytes: %s\n", bytesRead, recv_buffer);
      
          
    

         
          char response[512];

          memset(response,0,sizeof(response));

             //=====header section======

         dns_header *header=(dns_header *)response;

         /* ===PARSE THE DNS HEADER from request===*/
         dns_header *request_header=(dns_header *)recv_buffer;
         u16 request_flags=ntohs(request_header->flags);
         
         u16 opcode=DNS_OPCODE(request_flags);
         u16 RD=DNS_RD(request_flags);


         u16 response_flags=0;

         header->packet_id=request_header->packet_id;

         response_flags |=(1<<15);

         response_flags |=(opcode<<11);

         if(RD){
               response_flags |=(1<<8);
         }


         if(opcode !=0){
             response_flags |=4;
         }

    
         header->flags=htons(response_flags);
          
         header->answer_record_count=htons(0);
         header->authority_record_count=htons(0);
         header->additional_record_count=htons(0);



         
         //=======question section===========

         /*
             NAME(variable)
             RECORD(2 bytes)
             CLASS(2 bytes)

             this will be parsed from the receive buffer
         */

    
         u8 *start_of_question=((u8 *)recv_buffer+DNS_HEADER_SIZE);
         u8 *end_of_question=start_of_question;

         /*
             right now start_of_question ptr is pointing at the beginning of the buffer,we need to find where the 
             encoded qname ends so that we can use the next 2 bytes for QTYPE and the next 2 bytes for QCLASS.
             we can find the end of the qname since in the encoding we added 0 at the end.
         */

         

         while(*end_of_question!=0){
            end_of_question++;
         }

         end_of_question++;

         /*
           record

         */

         u16 qtype=ntohs(*(u16 *)end_of_question);
         end_of_question+=2;

          /*
        class
           
         */

         u16 qclass=ntohs(*(u16 *)end_of_question);

         
         end_of_question+=2;

         u16 question_len=end_of_question-start_of_question;

         header->question_count=htons(1);
      
         
         memcpy(response+DNS_HEADER_SIZE,recv_buffer+DNS_HEADER_SIZE,question_len);

         //=======Answer section=============
         
         //it's structure

         /*
          
            NAME      (variable)
            TYPE      (2 bytes)
            CLASS     (2 bytes)
            TTL       (4 bytes)
            RDLENGTH  (2 bytes)
            RDATA     (4 bytes)


            The values will be parsed from the receive buffer
         
         */

         u8 *answer_start_pos=(u8 *)response+DNS_HEADER_SIZE+question_len;
         u8 *src=(u8 *)response+DNS_HEADER_SIZE;
       
         /*Now the start_answer_pos points to the beginning of the name ,it should be advanced upto end 
         of name+1 so that we can add the next sections of the answer into the response*/

         while(*src!=0) *answer_start_pos++=*src++;
         
         *answer_start_pos++=0;
         
         *(u16 *)answer_start_pos=htons(1);
         answer_start_pos+=2;

         *(u16 *)answer_start_pos=htons(1);
         answer_start_pos+=2;

         *(u32 *)answer_start_pos=htonl(60);
         answer_start_pos+=4;
   

         *(u16 *)answer_start_pos=htons(4);
         answer_start_pos+=2;

         answer_start_pos[0]=8;
         answer_start_pos[1]=8;
         answer_start_pos[2]=8;
         answer_start_pos[3]=8;

         answer_start_pos+=4;

      


         header->answer_record_count=htons(1);

         size_t response_len=answer_start_pos-(u8 *)response;

          if (sendto(udpSocket, response, response_len, 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress)) == -1) {
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