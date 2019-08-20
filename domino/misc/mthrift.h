#ifndef THRIFT_H_INCLUDED
#define THRIFT_H_INCLUDED

#include <stdint.h>
#include "socket.h"
#include <dolib.h>

#define T_CALL 1
#define T_REPLY 2
#define T_EXCEPTION 3
#define T_ONWAY 4

#define T_STOP 0
#define T_VOID 1
#define T_BOOL 2
#define T_BYTE 3
#define T_I08 3
#define T_I16 6
#define T_I32 8
#define T_U64 9
#define T_I64 10
#define T_DOUBLE 4
#define T_STRING 11
#define T_UTF7 11
#define T_STRUCT 12
#define T_MAP 13
#define T_SET 14
#define T_LIST 15
#define T_UTF8 16
#define T_UTF16 17

#define ntohll(n) ( (((unsigned long long)ntohl(n)) << 32) + ntohl(n >> 32) )
#define htonll(n) ( (((unsigned long long)htonl(n)) << 32) + htonl(n >> 32) )

typedef struct {
    int type;
    char *name;
    int id;
} thrift_message_t;

#ifdef __cplusplus
extern "C"
{
#endif

int thrift_read_header(socket_t sock, const char *name, int32_t *cseqid);

int thrift_read_field_header(socket_t sock, int8_t *type, int16_t *id);
int thrift_skip_field(socket_t sock, int8_t type);

int thrift_read_map_header(socket_t sock, int8_t *key_type, int8_t *val_type, uint32_t *size);
int thrift_read_set_header(socket_t sock, int8_t *type, uint32_t *size);
int thrift_read_list_header(socket_t sock, int8_t *type, uint32_t *size);

#define thrift_read_bool(sock, buf) thrift_read_i8(sock, buf)
int thrift_read_i8(socket_t sock, int8_t *buf);
int thrift_read_i16(socket_t sock, int16_t *buf);
int thrift_read_i32(socket_t sock, int32_t *buf);
int thrift_read_i64(socket_t sock, int64_t *buf);
int thrift_read_str(socket_t sock, char** buf);
int thrift_read_message(socket_t sock, thrift_message_t *msg);

int thrift_send_function(socket_t sock, const char *name, int32_t cseqid, void *params, size_t psize);
int thrift_skip_field(socket_t sock, int8_t type);

int thrift_write_i8(socket_t sock, int8_t value);
int thrift_write_i16(socket_t sock, int16_t value);
int thrift_write_i32(socket_t sock, int32_t value);
int thrift_write_i64(socket_t sock, int64_t value);
int thrift_write_str(socket_t sock, const char* buf);
int thrift_write_message(socket_t sock, thrift_message_t *msg);
int thrift_write_string_list(socket_t sock, do_string_list_t *list);
int thrift_write_field_header(socket_t sock, int8_t type, int16_t id);
int thrift_write_buf(socket_t sock, char* buf, size_t size);

thrift_message_t *thrift_message_new();
void thrift_message_free(thrift_message_t *msg);


#ifdef __cplusplus
}
#endif


#endif // THRIFT_H_INCLUDED
