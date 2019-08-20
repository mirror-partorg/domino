#include "mthrift.h"

#include <errno.h>
#include <dolib.h>

#define MAX_TIME_OUT_USEC 5*1000000
#define BUF_LEN 512
#define READ_ERROR -1
#define READ_TIMEOUT -2

static const int32_t VERSION_1 = 0x80010000;
static const int32_t VERSION_MASK = 0xffff0000;

// static function
int thrift_send_function(socket_t sock, const char *name, int32_t cseqid, void *params, size_t psize)
{
    char buffer[BUF_LEN];
    int32_t i32 = (int32_t)htonl((VERSION_1)|((int32_t)T_CALL));
    char* cp = buffer + sizeof(i32);

    memcpy(cp, &i32, sizeof(i32));
    cp += sizeof(i32);

    i32 = (int32_t)htonl(strlen(name));
    memcpy(cp, &i32, sizeof(i32));
    cp += sizeof(i32);

    strncpy(cp, name, strlen(name));
    cp += strlen(name);

    i32 = (int32_t)htonl(cseqid);
    memcpy(cp, &i32, sizeof(i32));
    cp += sizeof(i32);

    i32 = (int32_t)htonl(cp - buffer - sizeof(i32) + psize + 1/*T_STOP*/);
    memcpy(buffer, &i32, sizeof(i32));
    if (send(sock, buffer, (cp - buffer), 0) < 0) {
        do_log(LOG_ERR, "error write: " SOCKERROR_SUFFIX);
        return 0;
    }
    if (params && psize) {
        if (send(sock, params, psize, 0) < 0) {
            do_log(LOG_ERR, "error write: " SOCKERROR_SUFFIX);
            return 0;
        }
    }
    *cp = T_STOP;
    if (send(sock, cp, 1, 0) < 0) {
        do_log(LOG_ERR, "error write: " SOCKERROR_SUFFIX);
        return 0;
    }
    return 1;
}

int thrift_read_field_header(socket_t sock, int8_t *type, int16_t *id)
{
    if (!thrift_read_i8(sock, type))
        return 0;
    if (*type == T_STOP)
        return 1;
    if (!thrift_read_i16(sock, id))
        return 0;
    return 1;
}

int thrift_read_header(socket_t sock, const char *name, int32_t *cseqid)
{
    int32_t size;
    if (!thrift_read_i32(sock, &size))
        return 0;
    if (size < 0) {
        do_log(LOG_ERR, "Bad header");
        return 0;
    }
    int32_t vs;
    int8_t type;
    char *buf = NULL;
    if (!thrift_read_i32(sock, &vs))
        return 0;
    if (vs < 0) {
        // Check for correct version number
        int32_t version = vs & VERSION_MASK;
        if (version != VERSION_1) {
            do_log(LOG_ERR, "Bad version identifier");
            return 0;
        }
        type = (int8_t)(vs & 0x000000ff);
        if (!thrift_read_str(sock, &buf))
            return 0;

        if (!thrift_read_i32(sock, cseqid)) {
            if (buf) do_free(buf);
            return 0;
        }
    }
    else {
        do_log(LOG_ERR, "No version identifier... old protocol info in strict mode?");
        if (buf) do_free(buf);
        return 0;
    }
    if (type == T_EXCEPTION) {
        do_log(LOG_ERR, "Exception message type");
        if (buf) do_free(buf);
        return 0;
    }
    if (type != T_REPLY) {
        do_log(LOG_ERR, "Invalid message type");
        if (buf) do_free(buf);
        return 0;
    }
    if (!buf || strcmp(buf, name)) {
        do_log(LOG_ERR, "Wrong method name");
        if (buf) do_free(buf);
        return 0;
    }
    if (buf) do_free(buf);
    return 1;
}
static int read_(socket_t sock, void *buffer, size_t buf_len, int usec)
{
    fd_set readset;
    struct timeval tv;
    int n, c;
    size_t len = 0;

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

int thrift_read_i32(socket_t sock, int32_t *buf)
{
    uint32_t i32;
    if (read_(sock, &i32, sizeof(i32), MAX_TIME_OUT_USEC) != sizeof(i32)) {
        do_log(LOG_ERR, "error read: " SOCKERROR_SUFFIX);
        return 0;
    }
    *buf = ntohl(i32);
    return 1;
}
int thrift_read_i16(socket_t sock, int16_t *buf)
{
    uint16_t i16;
    if (read_(sock, &i16, sizeof(i16), MAX_TIME_OUT_USEC) != sizeof(i16)) {
        do_log(LOG_ERR, "error read: " SOCKERROR_SUFFIX);
        return 0;
    }
    *buf = ntohs(i16);
    return 1;
}
int thrift_read_i8(socket_t sock, int8_t *buf)
{
    if (read_(sock, buf, sizeof(int8_t), MAX_TIME_OUT_USEC) != sizeof(int8_t)) {
        do_log(LOG_ERR, "error read: " SOCKERROR_SUFFIX);
        return 0;
    }
    return 1;
}
int thrift_read_i64(socket_t sock, int64_t *buf)
{
    uint64_t i64;
    if (read_(sock, &i64, sizeof(i64), MAX_TIME_OUT_USEC) != sizeof(i64)) {
        do_log(LOG_ERR, "error read: " SOCKERROR_SUFFIX);
        return 0;
    }
    *buf = ntohll(i64);
    return 1;
}
int thrift_read_str(socket_t sock, char** buf)
{
    int32_t len;
    if (!thrift_read_i32(sock, &len))
        return 0;
    (*buf) = (char*)do_malloc(len + 1);
    if (read_(sock, (*buf), len, MAX_TIME_OUT_USEC) != len) {
        do_log(LOG_ERR, "error read: " SOCKERROR_SUFFIX);
        return 0;
    }
    (*buf)[len] = '\0';
    return 1;
}
int thrift_skip_field(socket_t sock, int8_t type)
{
    switch (type) {
        case T_BYTE:
        case T_BOOL: {
            int8_t i8;
            return thrift_read_i8(sock, &i8);
        }
        case T_I16: {
            int16_t i16;
            return thrift_read_i16(sock, &i16);
        }
        case T_I32: {
            int32_t i32;
            return thrift_read_i32(sock, &i32);
        }
        case T_I64: {
            int64_t i64;
            return thrift_read_i64(sock, &i64);
        }
        case T_DOUBLE: {
            int64_t i64;
            return thrift_read_i64(sock, &i64);
        }
        case T_STRING: {
            char *buf;
            int result = thrift_read_str(sock, &buf);
            do_free(buf);
            return result;

        }
        case T_STRUCT:  {
            int16_t field_id;
            int8_t  field_type;
            while (1) {
                if (!thrift_read_field_header(sock, &field_type, &field_id))
                    return 0;
                if (field_type == T_STOP)
                    break;
                if (!thrift_skip_field(sock, field_type))
                    return 0;
            }
            return 1;
        }
        case T_MAP:   {
            int8_t key_type, val_type;
            uint32_t i, size;
            if (!thrift_read_map_header(sock, &key_type, &val_type, &size))
                return 0;
            for (i = 0; i < size; i++) {
                if (!thrift_skip_field(sock, key_type))
                    return 0;
                if (!thrift_skip_field(sock, val_type))
                    return 0;
            }
            return 1;
        }
        case T_SET:      {
            int8_t  type;
            uint32_t i, size;
            if (!thrift_read_set_header(sock, &type, &size))
                return 0;
            for (i = 0; i < size; i++)
                if (!thrift_skip_field(sock, type))
                    return 0;
            return 1;
        }
        case T_LIST:    {
            int8_t  type;
            uint32_t i, size;
            if (!thrift_read_list_header(sock, &type, &size))
                return 0;
            for (i = 0; i < size; i++)
                if (!thrift_skip_field(sock, type))
                    return 0;
            return 1;
        }
        default:
            return 0;
    }
}

int thrift_read_map_header(socket_t sock, int8_t *key_type, int8_t *val_type, uint32_t *size)
{
    int32_t sizei;
    if (!thrift_read_i8(sock, key_type))
        return 0;
    if (!thrift_read_i8(sock, val_type))
        return 0;
    if (!thrift_read_i32(sock, &sizei))
        return 0;
    if (sizei < 0)  {
        do_log(LOG_ERR, "Negative size");
        return 0;
    }
    *size = (uint32_t)sizei;
    return 1;
}
int thrift_read_set_header(socket_t sock, int8_t *type, uint32_t *size)
{
    int32_t sizei;
    if (!thrift_read_i8(sock, type))
        return 0;
    if (!thrift_read_i32(sock, &sizei))
        return 0;
    if (sizei < 0)  {
        do_log(LOG_ERR, "Negative size");
        return 0;
    }
    *size = (uint32_t)sizei;
    return 1;
}
int thrift_read_list_header(socket_t sock, int8_t *type, uint32_t *size)
{
    int32_t sizei;
    if (!thrift_read_i8(sock, type))
        return 0;
    if (!thrift_read_i32(sock, &sizei))
        return 0;
    if (sizei < 0)  {
        do_log(LOG_ERR, "Negative size");
        return 0;
    }
    *size = (uint32_t)sizei;
    return 1;
}
int thrift_read_message(socket_t sock, thrift_message_t *msg)
{
    int32_t i32;
    if (!thrift_read_i32(sock, &i32)) // version
        return 0;
    if (i32 < 0) {
        int32_t version = i32 & VERSION_MASK;
        if (version != VERSION_1) {
            do_log(LOG_ERR, "Bad version identifier from");
            return 0;
        }
        msg->type = (i32 & 0x000000ff);
        if (!thrift_read_str(sock, &msg->name))
            return 0;
        if (!thrift_read_i32(sock, &msg->id))
            return 0;
    }
    else {
        do_log(LOG_ERR, "No version identifier... old protocol client in strict mode?");
        return 0;
    }
    return 1;
}
// write
int thrift_write_i8(socket_t sock, int8_t val)
{
    int8_t i = val;
    if (send(sock, (char*)&i, sizeof(i), 0) < 0) {
        do_log(LOG_ERR, "error write: " SOCKERROR_SUFFIX);
        return 0;
    }
    return 1;
}
int thrift_write_i16(socket_t sock, int16_t val)
{
    int16_t i = htons(val);
    if (send(sock, (char*)&i, sizeof(i), 0) < 0) {
        do_log(LOG_ERR, "error write: " SOCKERROR_SUFFIX);
        return 0;
    }
    return 1;
}
int thrift_write_i32(socket_t sock, int32_t val)
{
    int32_t i = htonl(val);
    if (send(sock, (char*)&i, sizeof(i), 0) < 0) {
        do_log(LOG_ERR, "error write: " SOCKERROR_SUFFIX);
        return 0;
    }
    return 1;
}
int thrift_write_i64(socket_t sock, int64_t val)
{
    int64_t i = htonll(val);
    if (send(sock, (char*)&i, sizeof(i), 0) < 0) {
        do_log(LOG_ERR, "error write: " SOCKERROR_SUFFIX);
        return 0;
    }
    return 1;
}

int thrift_write_str(socket_t sock, const char* buf)
{
    if (!thrift_write_i32(sock, strlen(buf)))
        return 0;

    if (send(sock, buf, strlen(buf), 0) < 0) {
        do_log(LOG_ERR, "error write: " SOCKERROR_SUFFIX);
        return 0;
    }
    return 1;
}

int thrift_write_message(socket_t sock, thrift_message_t *msg)
{
    int32_t version = (VERSION_1) | ((int32_t)msg->type);
    if (!thrift_write_i32(sock, version))
        return 0;
    if (!thrift_write_str(sock, msg->name))
        return 0;
    version = msg->id;
    if (!thrift_write_i32(sock, version))
        return 0;
    return 1;
}
static int thrift_write_list_begin(socket_t sock, int type, uint32_t size)
{
    int8_t t = type;

    if (!thrift_write_i8(sock, t))
        return 0;
    if (!thrift_write_i32(sock, size))
        return 0;
    return 1;
}
int thrift_write_string_list(socket_t sock, do_string_list_t *list)
{
    int i;
    if (!thrift_write_list_begin(sock, T_STRING, list->count))
        return 0;
    for (i = 0; i < list->count; i++)
        if (!thrift_write_str(sock, list->list[i]))
            return 0;
    return 1;
}
int thrift_write_field_header(socket_t sock, int8_t type, int16_t id)
{
    if (!thrift_write_i8(sock, type))
        return 0;
    if (type == T_STOP)
        return 1;
    if (!thrift_write_i16(sock, id))
        return 0;
    return 1;
}
int thrift_write_buf(socket_t sock, char* buf, size_t size)
{
    if (send(sock, buf, size, 0) < 0) {
        do_log(LOG_ERR, "error write: " SOCKERROR_SUFFIX);
        return 0;
    }
    return 1;
}

thrift_message_t *thrift_message_new()
{
    return do_malloc0(sizeof(thrift_message_t));
}

void thrift_message_free(thrift_message_t *msg)
{
    if (msg->name)
        do_free(msg->name);
    do_free(msg);
}
