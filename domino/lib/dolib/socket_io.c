#include "socket_io.h"
#ifdef GCRYPT
#include <gcrypt.h>
#endif
#ifdef ZLIB
#include <zlib.h>
#endif
#include <dolib.h>
#include <errno.h>

#define READ_ERROR -1
#define READ_TIMEOUT -2
#define READ_MAX_RETRIES 5
#ifdef _WIN32
#include <sys/timeb.h>

#define MAX_TIME_OUT_USEC 120*1000000
#define MIN_TIME_OUT_USEC 300


int gettimeofday(struct timeval *tp, void *tzp)
{
    struct _timeb timebuffer;

    _ftime(&timebuffer);
    tp->tv_sec = timebuffer.time;
    tp->tv_usec = timebuffer.millitm * 1000;

    return 0;
}
#endif

int do_read(socket_t sock, void *buffer, size_t buf_len, int wait)
{
    fd_set readset;
    struct timeval tv;
    int n, c;
    size_t len = 0;
    int usec;
    if (wait)
        usec = MAX_TIME_OUT_USEC;
    else
        usec = MIN_TIME_OUT_USEC;

    if (!buf_len)
        return 0;

    while (len < buf_len) {

        FD_ZERO(&readset);
        FD_SET(sock, &readset);
        tv.tv_sec =  usec / 1000000;
        tv.tv_usec = usec % 1000000;
        c = select(sock + 1, &readset, NULL, NULL, &tv);
        switch (c) {
            case 0:
                return READ_TIMEOUT;
            case 1:
                do {
                    n = recv(sock, buffer + len, buf_len - len, 0);
                } while (n == -1 && errno == EINTR);
                if (n == -1) {
                    do_log(LOG_ERR, "error read: " SOCKERROR_SUFFIX);
                    return READ_ERROR;
                }
                if (n == 0) {
                    do_log(LOG_ERR, "remote host has closed the socket");
                    return READ_ERROR;
                }
                len += n;
                break;
            case -1:
            default:
                do_log(LOG_ERR, "error read: " SOCKERROR_SUFFIX);
                return READ_ERROR;
        }
    }
    return len;
}

/*
int do_read(socket_t sock, void *buffer, size_t buf_len, int usec)
{
    fd_set readset;
    struct timeval tv;
    int n, c;
    size_t len = 0;
    struct timeval begin;
    struct timeval end;
    int retries = 0;
    if (!buf_len)
        return 0;
    gettimeofday(&begin, NULL);
    while (len < buf_len) {

        FD_ZERO(&readset);
        FD_SET(sock, &readset);
        tv.tv_sec =  usec / 1000000;
        tv.tv_usec = usec % 1000000;
        c = select(sock + 1, &readset, NULL, NULL, &tv);
        if (c > 0) {
            if (!FD_ISSET(sock, &readset))
                return READ_TIMEOUT;
            if ((n = recv(sock, (char*)buffer + len, buf_len - len, 0)) <= 0) {
                if (!n) {
                    do_log(LOG_ERR, "error read: " SOCKERROR_SUFFIX);
                    if (retries < READ_MAX_RETRIES) {
                        retries++;
                        gettimeofday(&end, NULL);
                        if ( (((end.tv_sec - begin.tv_sec) * 1000 * 1000)
                                 + (((uint64_t)(end.tv_usec - begin.tv_usec)))) < usec) {
                            usleep(50);
                            continue;
                        }
                    }
                    gettimeofday(&end, NULL);
                    return READ_TIMEOUT;
                }
                do_log(LOG_ERR, "error read: " SOCKERROR_SUFFIX);
                return READ_ERROR;
            }
            len += n;
        }
        else {
            if (!c) {
                if (!len)
                    return READ_TIMEOUT;
                else
                    return READ_ERROR;
            }
            do_log(LOG_ERR, "error read: " SOCKERROR_SUFFIX);
            return READ_ERROR;
        }
    }
    if (!len && buf_len)
        return READ_ERROR;
    return len; // time out
}
*/
/*

int do_read(socket_t sock, void *buffer, size_t buf_len, int usec)
{
    if (!buf_len)
        return 0;

    if (sock < 0) {
        do_log(LOG_ERR, "Called read on non-open socket");
        return READ_ERROR;
    }

    int32_t retries = 0;
    uint32_t eagainThresholdMicros = 0;
    struct timeval begin;
    int got;
    int errno_copy ;
    struct timeval end;
    struct timeval tv;
    uint32_t readElapsedMicros;

    tv.tv_sec =  usec / 1000000;
    tv.tv_usec = usec % 1000000;
    int ret = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    do_log(LOG_ERR, "error read: " SOCKERROR_SUFFIX);
    if (usec)
        eagainThresholdMicros = (usec*1000000) / READ_MAX_RETRIES;

try_again:

    gettimeofday(&begin, NULL);

    got = recv(sock, buffer, buf_len, 0);
    errno_copy = errno;
    gettimeofday(&end, NULL);
    readElapsedMicros =  (((end.tv_sec - begin.tv_sec) * 1000 * 1000)
                                 + (((uint64_t)(end.tv_usec - begin.tv_usec))));
    // Check for error on read
    if (got < 0) {
        if (errno_copy == EAGAIN) {
        // check if this is the lack of resources or timeout case
            if (!eagainThresholdMicros || (readElapsedMicros < eagainThresholdMicros)) {
                if (retries++ < READ_MAX_RETRIES) {
                    usleep(50);
                    goto try_again;
                }
                else
                    do_log(LOG_ERR, "EAGAIN (unavailable resources)");
            }
            else
                do_log(LOG_ERR, "EAGAIN (timed out)");
        }
        // If interrupted, try again
        if (errno_copy == EINTR && retries++ < READ_MAX_RETRIES)
            goto try_again;
#if defined __FreeBSD
        if (errno_copy == ECONNRESET)
                // shigin: freebsd doesn't follow POSIX semantic of recv and fails with
                // ECONNRESET if peer performed shutdown
                //
            return READ_ERROR;

#endif

        do_log(LOG_ERR, "error read: " SOCKERROR_SUFFIX);
        return READ_ERROR;
    }
    return got;
}
*/
/*
int do_read(socket_t sock, void *buffer, size_t buf_len, int usec)
{
    fd_set readset;
    struct timeval tv;
    int n, c;
    size_t len = 0;
    while (len < buf_len) {
        FD_ZERO(&readset);
        FD_SET(sock, &readset);
        tv.tv_sec =  usec / 1000000;
        tv.tv_usec = usec % 1000000;
        c = select(sock + 1, &readset, NULL, NULL, &tv);
        if (c > 0 ) {
            if (!FD_ISSET(sock, &readset))
                return -2; // time out
            if ((n = recv(sock, (char*)buffer + len, buf_len - len, 0)) <= 0 ) {
                if (!n)
                    return 0;
                return n;
            }
            len += n;
        }
        else {
            if (!c) {
                if (!len)
                    return -2;
                else
                    return len;
            }
            do_log(LOG_ERR, "error read: " SOCKERROR_SUFFIX);
            return -1;
        }
    }
    if (!len && buf_len)
        return 0;
    return len; // time out
}
*/
int do_read_data(socket_t sock, do_data_t *data,
#ifdef GCRYPT
                     do_sessionIV_t *iv, do_crypt_key_t *key,
#endif
                    int wait_time_sec)
{
    do_exch_header_t header;
    int n, wait_data;
#ifdef ZLIB
    UNPACK_SIZE_TYPE unpack_size = 0;
#endif
#ifdef GCRYPT
    int mdlen;
    unsigned char md5hash[500];
#endif
    wait_data = wait_time_sec * 1000000;
    while (1) {
        n = do_read(sock, &header, sizeof(header), MAX_TIME_OUT_USEC);
        if (n == -2) {
            if (wait_time_sec == -1)
                continue;
            wait_data -= MAX_TIME_OUT_USEC;
            if (wait_data > 0)
                continue;
            do_data_set_err(data, "Can't read data for timeout");
            return 0;
        }
        if (n == -1) {
            do_data_set_err(data, "Error read data");
            return 0;
        }
        if (n != sizeof(header) || !DO_EXCH_HEADER_CHECK(header)) {
            do_data_set_err(data, "Input data was corrupted");
            return 0;
        }
        do_data_set_type(data, header.type);
#ifdef ZLIB
        if (header.flags & DO_EXCH_FLAG_ZIP)
            if (do_read(sock, &unpack_size, sizeof(unpack_size), MAX_TIME_OUT_USEC) != sizeof(unpack_size)) {
                do_data_set_err(data, "Can't read unpack size info");
                return 0;
            }
#endif
#ifdef GCRYPT
        if (header.flags & DO_EXCH_FLAG_MD5) {
            mdlen = gcry_md_get_algo_dlen (GCRY_MD_MD5);
            if (do_read(sock, md5hash, mdlen, MAX_TIME_OUT_USEC) != mdlen) {
                do_data_set_err(data, "Can't read hash");
                return 0;
            }
        }
#endif
        if (!do_data_alloc(data, header.size)) {
            do_data_set_errf(data, "Could not allocate memory for data %dByte", header.size);
            return 0;
        }
        n = do_read(sock, do_data_get(data), do_data_size(data), MAX_TIME_OUT_USEC);
        if ((n < 0) || ((size_t)n != do_data_size(data))) {
            do_data_set_err(data, "Can't read data");
            return 0;
        }
        break;
    }
#ifdef GCRYPT
    if (header.flags & DO_EXCH_FLAG_MD5) {
        gcry_md_hd_t hd;
        unsigned char *p;
        gcry_error_t err = 0;
        mdlen = gcry_md_get_algo_dlen (GCRY_MD_MD5);
        if (mdlen < 1 || mdlen > 500)   {
            do_data_set_err(data, "hash len is invalid");
            return 0;
        }
        err = gcry_md_open (&hd, GCRY_MD_MD5, 0);
        if (err) {
            do_data_set_err(data, "Can't read data");
            gcry_md_close(hd);
            return 0;
        }
        gcry_md_write(hd, do_data_get(data), do_data_size(data));
        p = gcry_md_read (hd, GCRY_MD_MD5);
        if (memcmp(md5hash, p, mdlen)) {
            do_data_set_err(data, "Data invalid");
            gcry_md_close(hd);
            return 0;
        }
        gcry_md_close(hd);
    }
    if (header.flags & DO_EXCH_FLAG_AES) {
        void *buf;
        if ((buf = do_malloc(do_data_size(data))) == NULL) {
            do_data_set_errf(data, "Could not allocate memory for data %dByte", do_data_size(data));
            return 0;
        }
        gcry_cipher_hd_t hd;

        if (gcry_cipher_open (&hd, GCRY_CIPHER_AES, GCRY_CIPHER_MODE_CFB, GCRY_CIPHER_CBC_CTS)) {
            do_log(LOG_ERR, "GCrypt aes-cbc-cts, grcy_open_cipher failed");
            do_free(buf);
            return 0;
        }

        if ( gcry_cipher_setkey (hd, key->key, key->key_len)) {
            do_log(LOG_ERR, "GCrypt aes-cbc-cts, gcry_cipher_setkey failed");
            gcry_cipher_close (hd);
            do_free(buf);
            return 0;
        }

        if ( gcry_cipher_setiv (hd, iv->IV, key->blk_len) ) {
            do_log(LOG_ERR, "GCrypt aes-cbc-cts, gcry_cipher_setiv failed");
            gcry_cipher_close (hd);
            do_free(buf);
            return 0;
        }

        if ( gcry_cipher_decrypt (hd, buf, do_data_size(data), do_data_get(data), do_data_size(data)) ) {
          do_log(LOG_ERR, "GCrypt aes-cbc-cts, gcry_cipher_decrypt failed");
          gcry_cipher_close (hd);
          do_free(buf);
          return 0;
        }
        gcry_cipher_close (hd);
        do_data_set_data(data, buf, do_data_size(data));
    }
#endif
//!!
#ifdef ZLIB
    if (header.flags & DO_EXCH_FLAG_ZIP) {
        void *buf;
        if ((buf = do_malloc(unpack_size)) == NULL) {
            do_data_set_errf(data, "Could not allocate memory for data %dByte", unpack_size);
            return 0;
        }
        uLongf size_f = unpack_size;
        int error = uncompress(buf, &size_f, do_data_get(data), header.size);
        if (error != Z_OK) {
            do_free(buf);
            switch (error) {
                case Z_MEM_ERROR:
                    do_data_set_err(data, "Not enough memory");
                    do_log(LOG_ERR, "ZLib: Not enough memory");
                    break;
                case Z_BUF_ERROR:
                    do_data_set_err(data, "Not enough room in the output buffer");
                    do_log(LOG_ERR, "ZLib: Not enough room in the output buffer");
                    break;
                case Z_DATA_ERROR:
                    do_data_set_err(data, "Input data was corrupted");
                    do_log(LOG_ERR, "ZLib: Input data was corrupted");
                    break;
                default :
                    do_data_set_errf(data, "Undefined zlib error %d", error);
                    do_log(LOG_ERR, "ZLib: Undefined zlib error %d", error);
            }
            return 0;
        }
        do_data_set_data(data, buf, size_f);
    }
#endif
    return 1;
}
#define do_log_debug(...)

int do_send_ok(socket_t sock)
{
    int retval;
    do_data_t *data = do_data_new();
    do_data_set_type(data, DO_EXCH_DATA_TYPE_OK);
    retval = do_send_data(sock, data,
#ifdef GCRYPT
                            NULL, NULL,
#endif
                            0);
    do_data_free(data);
    return retval;
}

int do_send_data(socket_t sock, do_data_t *data,
#ifdef GCRYPT
                     do_sessionIV_t *iv, do_crypt_key_t *key,
#endif
                    do_exch_flag_t support_flags)
{
    do_exch_header_t header;
    header.size = 0;
    header.flags = 0;
    header.type = do_data_type(data);
    int n;
    size_t all_size;
    char *data_out, *crnt;

    void *out = NULL;
#if defined(ZLIB) || defined(GCRYPT)
    void *buf = NULL;
#endif
#ifdef ZLIB
    uint32_t unpack_size;
#endif
#ifdef GCRYPT
    int mdlen = 0;
    unsigned char md5hash[500];
#endif
    if (data) {
        if (!DO_IS_EXCH_DATA_INFO(do_data_type(data))) {
            header.size = do_data_size(data);
            out = do_data_get(data);
#ifdef ZLIB
            if ((do_data_send_flags(data) & DO_EXCH_FLAG_ZIP) && (support_flags & DO_EXCH_FLAG_ZIP)) {
                unpack_size = do_data_size(data);
                header.size = unpack_size * 1.1 + 12;
                if ((buf = do_malloc(header.size)) == NULL) {
                    do_log(LOG_ERR, "could not allocate memory for data");
                    return 0;
                }
                uLongf size_f = header.size;
                int error = compress2(buf, &size_f, do_data_get(data), unpack_size, 9);
                if (error != Z_OK) {
                    do_free(buf);
                    switch (error) {
                        case Z_MEM_ERROR:
                            do_log(LOG_ERR, "ZLib: not enough memory");
                            break;
                        case Z_BUF_ERROR:
                            do_log(LOG_ERR, "ZLib: not enough room in the output buffer");
                            break;
                        case Z_STREAM_ERROR:
                            do_log(LOG_ERR, "ZLib: input level parameter is invalid");
                            break;
                        default :
                            do_log(LOG_ERR, "ZLib: undefined zlib error %d", error);
                    }
                    do_free(buf);
                    return 0;
                }
                out = buf;
                header.size = size_f;
                header.flags += DO_EXCH_FLAG_ZIP;
            }
#endif
#ifdef GCRYPT
            if ((do_data_send_flags(data) & DO_EXCH_FLAG_AES) && (support_flags & DO_EXCH_FLAG_AES)) {
                header.flags += DO_EXCH_FLAG_AES;
                if ((buf = do_malloc(do_data_size(data))) == NULL) {
                    do_data_set_errf(out, "Could not allocate memory for data %dByte", do_data_size(data));
                    if (out != do_data_get(data)) do_free(out);
                    return 0;
                }
                gcry_cipher_hd_t hd;

                if (gcry_cipher_open (&hd, GCRY_CIPHER_AES, GCRY_CIPHER_MODE_CFB, GCRY_CIPHER_CBC_CTS)) {
                    do_log(LOG_ERR, "GCrypt aes-cbc-cts, grcy_open_cipher failed");
                    if (buf) do_free(buf);
                    if (out != do_data_get(data)) do_free(out);
                    return 0;
                }

                if ( gcry_cipher_setkey (hd, key->key, key->key_len)) {
                    do_log(LOG_ERR, "GCrypt aes-cbc-cts, gcry_cipher_setkey failed");
                    gcry_cipher_close (hd);
                    if (buf) do_free(buf);
                    if (out != do_data_get(data)) do_free(out);
                    return 0;
                }

                if ( gcry_cipher_setiv (hd, iv->IV, key->blk_len) ) {
                    do_log(LOG_ERR, "GCrypt aes-cbc-cts, gcry_cipher_setiv failed");
                    gcry_cipher_close (hd);
                    if (buf) do_free(buf);
                    if (out != do_data_get(data)) do_free(out);
                    return 0;
                }

                if ( gcry_cipher_encrypt (hd, buf, do_data_size(data), do_data_get(data), do_data_size(data)) ) {
                    do_log(LOG_ERR, "GCrypt aes-cbc-cts, gcry_cipher_encrypt failed");
                    gcry_cipher_close (hd);
                    if (buf) do_free(buf);
                    if (out != do_data_get(data)) do_free(out);
                    return 0;
                }
                gcry_cipher_close (hd);
                if (out != do_data_get(data)) do_free(out);
                out = buf;
            }
            if ((do_data_send_flags(data) & DO_EXCH_FLAG_MD5) && (support_flags & DO_EXCH_FLAG_MD5)) {
                gcry_md_hd_t hd;
                gcry_error_t err = 0;
                mdlen = gcry_md_get_algo_dlen (GCRY_MD_MD5);
                unsigned char *p;
                if (mdlen < 1 || mdlen > 500)   {
                    do_data_set_err(data, "hash len is invalid");
                    if (out != do_data_get(data)) do_free(out);
                    return 0;
                }
                err = gcry_md_open (&hd, GCRY_MD_MD5, 0);
                if (err) {
                    do_data_set_err(data, "Can't read data");
                    gcry_md_close(hd);
                    if (out != do_data_get(data)) do_free(out);
                    return 0;
                }
                gcry_md_write(hd, out, header.size);
                p = gcry_md_read (hd, GCRY_MD_MD5);
                memcpy(md5hash, p, mdlen);
                gcry_md_close(hd);
            }
#endif

        }
        else {
            header.size = do_data_size(data);
            out = do_data_get(data);
        }
    }
    do_log_debug(ZONE, "start header");
    all_size = sizeof(header);
    /*n = send(sock, (char*)&header, sizeof(header), 0 );
    if ( n != sizeof(header) ) {
        do_log(LOG_ERR, "error write: " SOCKERROR_SUFFIX);
        if (out != do_data_get(data)) do_free(out);
        return 0;
    }*/
#ifdef ZLIB
    if (header.flags & DO_EXCH_FLAG_ZIP) {
        all_size += sizeof(unpack_size);
        /*do_log_debug(ZONE, "start unpack size");
        if ( send(sock, (char*)&unpack_size, sizeof(unpack_size), 0 ) != sizeof(unpack_size) ) {
            do_log(LOG_ERR, "error write: " SOCKERROR_SUFFIX);
            if (out != do_data_get(data)) do_free(out);
            return 0;
        }*/
    }
#endif
#ifdef GCRYPT
    if (header.flags & DO_EXCH_FLAG_MD5) {
        all_size += mdlen;
        /*do_log_debug(ZONE, "start hash");
        if ( send(sock, (char*)md5hash, mdlen, 0 ) != mdlen ) {
            do_log(LOG_ERR, "error write: " SOCKERROR_SUFFIX);
            if (out != do_data_get(data)) do_free(out);
            return 0;
        }*/
    }
#endif
    all_size += header.size;
    data_out = do_malloc(all_size);
    if ( !data_out ) {
        do_log(LOG_ERR, "can't alloc %d bytes", all_size);
        if (out != do_data_get(data)) do_free(out);
        return 0;
    }
    crnt = data_out;
    memcpy(crnt, &header, sizeof(header));
    crnt += sizeof(header);
#ifdef ZLIB
    if (header.flags & DO_EXCH_FLAG_ZIP) {
        memcpy(crnt,(char*)&unpack_size, sizeof(unpack_size));
        crnt+=sizeof(unpack_size);
    }
#endif
#ifdef GCRYPT
    if (header.flags & DO_EXCH_FLAG_MD5) {
        memcpy(crnt,(char*)md5hash, mdlen);
        crnt += mdlen;
    }
#endif
    if ( out )
        memcpy(crnt, out, header.size);
    do_log_debug(ZONE, "start data");
    //if (out)
    if ( ((n = send(sock, data_out, all_size, 0 )) < 0) || ((size_t)n != all_size)) {
        do_log(LOG_ERR, "error write: " SOCKERROR_SUFFIX);
        do_free(data_out);
        if (out != do_data_get(data)) do_free(out);
        return 0;
    }
    do_free(data_out);
    do_log_debug(ZONE, "end data %d", header.size);
    if (out && data && out != do_data_get(data)) do_free(out);
    return 1;
}
int do_send(socket_t sock, void *data, size_t size)
{
    if (send(sock, data, size, 0) < 0 ) {
        do_log(LOG_ERR, "error write: " SOCKERROR_SUFFIX);
        return 0;
    }
    return 1;
}
