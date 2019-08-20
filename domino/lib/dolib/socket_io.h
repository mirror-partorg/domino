#ifndef DO_IO_H_INCLUDED
#define DO_IO_H_INCLUDED

#include "../../misc/socket.h"
#include <dolib.h>

#define MAX_TIME_OUT_USEC 120*1000000
#define MIN_TIME_OUT_USEC 300


#ifdef GCRYPT
typedef struct {
    unsigned char *key;
    size_t         key_len;
    size_t         blk_len;
} do_crypt_key_t;
typedef struct {
    unsigned char *IV;
    size_t         len;
} do_sessionIV_t;
#endif


#ifdef __cplusplus
extern "C"
{
#endif

int do_read(socket_t sock, void *buffer, size_t buf_len, int wait);
int do_read_data(socket_t sock, do_data_t *data,
#ifdef GCRYPT
                     do_sessionIV_t *iv, do_crypt_key_t *key,
#endif
                    int wait);
int do_send(socket_t sock, void *data, size_t size);
int do_send_ok(socket_t sock);
int do_send_data(socket_t sock, do_data_t *data,
#ifdef GCRYPT
                     do_sessionIV_t *iv, do_crypt_key_t *key,
#endif
                     do_exch_flag_t support_flags);

#ifdef __cplusplus
}
#endif

#endif // DO_IO_H_INCLUDED
