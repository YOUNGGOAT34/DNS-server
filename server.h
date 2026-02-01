#ifndef SERVER_H
#define SERVER_H

#define DNS_HEADER_SIZE 12

typedef signed int i32;
typedef unsigned int u32;
typedef unsigned short int u16;
typedef char i8;
typedef unsigned char u8;


#define DNS_QR(flags)     (((flags) >> 15) & 0x1)
#define DNS_OPCODE(flags) (((flags) >> 11) & 0xF)
#define DNS_AA(flags)     (((flags) >> 10) & 0x1)
#define DNS_TC(flags)     (((flags) >> 9)  & 0x1)
#define DNS_RD(flags)     (((flags) >> 8)  & 0x1)
#define DNS_RA(flags)     (((flags) >> 7)  & 0x1)
#define DNS_RCODE(flags)  ((flags) & 0xF)


typedef struct{
       u8 encoded_name[255];
       u16 type;
       u16 _class;

}question;

typedef struct {
       u16 packet_id;
       u16 flags;
       u16 question_count;
       u16 answer_record_count;
       u16 authority_record_count;
       u16 additional_record_count;

}dns_header;


void server(void);
void encode_qname(u8 *buffer,const i8 *host);

#endif