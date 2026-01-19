#ifndef SERVER_H
#define SERVER_H

typedef signed int i32;
typedef unsigned int u32;
typedef unsigned short int u16;
typedef char i8;
typedef unsigned char u8;

typedef struct {
       u16 packet_id;
       u8  query_response:1;
       u8 opcode:4;
       u8 authoritative_answer:1;
       u8 truncation:1;
       u8 recursion_desired:1;
       u8 recursion_available:1;
       u8 reserved:3;
       u8 response_code:4;
       u16 question_count;
       u16 answer_record_count;
       u16 authority_record_count;
       u16 additional_record_count;

}header;


void server(void);

#endif