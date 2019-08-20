#include "artix.h"
#include <dolib.h>
#include "../../config/config.h"
#include "../../misc/socket.h"
#include "../../misc/mthrift.h"
#include "../../misc/iconv_.h"
#include "../../misc/define.h"
#include <stdint.h>
#include <errno.h>
#include <string.h>

#define GETUPLOADSTATE "getUploadState"
#define SETUPLOADSTATE "setUploadState"
#define GETUPLOADPROGRESS "getUploadProgress"
#define SETUPLOADPROGRESS "setUploadProgress"
#define STARTUPLOAD "startUpload"
#define SUCCESSUPLOAD "successUpload"
#define FAILUPLOAD "failUpload"
#define GETUPLOADERRORDESCRIPTION "getUploadErrorDescription"
#define SETUPLOADERRORDESCRIPTION "setUploadErrorDescription"

#define GETAVAILABLEMODULES "getAvailableModules"
#define GETMODULEDESCRIPTION "getModuleDescription"

#define GETALERT "getAlertUtf8"
#define SETALERT "setAlertWithCodeUtf8"
#define RESETALERT "resetAlert"
#define BUF_LEN 512

#ifndef OLD_ARTIX_INFO
static const char *alert_message[]= {
   "База данных недоступна",
   "Обновление цен не работает",
   "Ошибки в обновлении цен",
   "Ошибки выгрузки текущих продаж",
   "Скидки не работают",
   "Скидки работают с ошибками",
   "Сетевое соединение не работает"
   "Неизвестная проблема, перезагрузите"
};
#endif

struct _artix_info_t{
    socket_t sock;
    int use_utf;
#ifdef WINSOCK
    WSADATA winsock_data;
#endif
};

//static int read_double(artix_info_t *info, double *buf);

artix_info_t *artix_info_new(const char* host, int port, int use_utf)
{
    artix_info_t *info = (artix_info_t *)do_malloc(sizeof(artix_info_t));
#ifdef WINSOCK
    if( WSAStartup(MAKEWORD(2, 2), &info->winsock_data) ) {
        do_free(info);
        return NULL;
    }
#endif
    info->sock = socket( PF_INET, SOCK_STREAM, 0);
	if ( invalid_socket(info->sock) ) {
	    do_log(LOG_ERR, "socket not open " SOCKERROR_SUFFIX);
        do_free(info);
        return NULL;
	}
	do_log_debug(ZONE, "open socket");
    struct hostent *hp = gethostbyname(host);
    if (!hp) {
        do_log(LOG_ERR, "Unknown host %s", host);
        do_free(info);
        return NULL;
    }
    sockaddr_in_t dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    memcpy(&dest.sin_addr, hp->h_addr, sizeof(dest.sin_addr));
    dest.sin_port = htons(port);
//    do_log(LOG_ERR, "S %s:%d", host, port);//!!

    if (connect(info->sock, (struct sockaddr*) &dest, sizeof(dest)) == -1) {
        do_log(LOG_ERR, "Connect not open " SOCKERROR_SUFFIX);
        close_socket(info->sock);
        do_free(info);
        return NULL;
    }
	do_log_debug(ZONE, "connect socket");
	info->use_utf = use_utf;
    return info;
}
DO_EXPORT artix_info_t *artix_info_new_default(const char* host, int use_utf)
{
    return artix_info_new(host, ARTIX_DEFAULT_INFO_SERVICE_PORT, use_utf);
}


DO_EXPORT void artix_info_free(artix_info_t *info)
{
    close_socket(info->sock);
#ifdef WINSOCK
    WSACleanup();
#endif
    do_free(info);
}
DO_EXPORT int artix_info_start_upload(artix_info_t *info)
{
    int32_t rseqid = 0;
    if (!thrift_send_function(info->sock, STARTUPLOAD, rseqid, NULL, 0))
        return 0;

    if (!thrift_read_header(info->sock, STARTUPLOAD, &rseqid))
        return 0;

    int8_t field_type;
    int16_t field_id;
    while (1) {
        if (!thrift_read_field_header(info->sock, &field_type, &field_id))
            return 0;
        if (field_type == T_STOP)
            break;
        thrift_skip_field(info->sock, field_type);
    }
    return 1;
}

DO_EXPORT int artix_info_success_upload(artix_info_t *info)
{
    int32_t rseqid = 0;
    if (!thrift_send_function(info->sock, SUCCESSUPLOAD, rseqid, NULL, 0))
        return 0;

    if (!thrift_read_header(info->sock, SUCCESSUPLOAD, &rseqid))
        return 0;
    int8_t field_type;
    int16_t field_id;
    while (1) {
        if (!thrift_read_field_header(info->sock, &field_type, &field_id))
            return 0;
        if (field_type == T_STOP)
            break;
        thrift_skip_field(info->sock, field_type);
    }
    return 1;
}
DO_EXPORT int artix_info_fail_upload(artix_info_t *info, char** messages, int count_mes)
{
    size_t len = BUF_LEN;
    char* params = (char*) do_malloc(len + 1);
    char *cp = params;
    *cp = T_LIST;
    cp++;
    int16_t id = (int16_t)ntohs(0xFFFF); // just in case
    memcpy(cp, &id, sizeof(id));
    cp += sizeof(id);
    *cp = T_STRING;
    cp++;
    int32_t size = (int32_t)ntohl(count_mes);
    memcpy(cp, &size, sizeof(size));
    cp += sizeof(size);
    int i, crntlen;
    for (i = 0; i < count_mes; i++) {
        if (cp - params + strlen(messages[i]) + sizeof(size) > len) {
            crntlen = cp - params;
            params = (char*) do_realloc(params, len * 2 + 1);
            len *= 2;
            cp = params + crntlen;
        }
        size = (int32_t)ntohl(strlen(messages[i]));
        memcpy(cp, &size, sizeof(size));
        cp += sizeof(size);
        strncpy(cp, messages[i], strlen(messages[i]));
        cp += strlen(messages[i]);
    }
    int psize = cp - params;
    int32_t rseqid = 0;
    if (!thrift_send_function(info->sock, FAILUPLOAD, rseqid, params, psize)) {
        do_free(params);
        return 0;
    }
    do_free(params);

    if (!thrift_read_header(info->sock, FAILUPLOAD, &rseqid))
        return 0;
    int8_t field_type;
    int16_t field_id;
    while (1) {
        if (!thrift_read_field_header(info->sock, &field_type, &field_id))
            return 0;
        if (field_type == T_STOP)
            break;
        thrift_skip_field(info->sock, field_type);
    }
    return 1;
}


DO_EXPORT int artix_info_get_upload_state(artix_info_t *info, artix_info_upload_state_t *state)
{
    int32_t cseqid = 0;

    if (!thrift_send_function(info->sock, GETUPLOADSTATE, cseqid, NULL, 0))
        return 0;

    if (!thrift_read_header(info->sock, GETUPLOADSTATE, &cseqid))
        return 0;

    int8_t field_type;
    int16_t field_id;
    *state = upsUndefine;
    while (1) {
        if (!thrift_read_field_header(info->sock, &field_type, &field_id))
            return 0;
        if (field_type == T_STOP)
            break;
        switch (field_id) {
            case 0:
                if (field_type != T_I32) {
                    do_log(LOG_ERR, "type mismatch");
                    if (!thrift_skip_field(info->sock, field_type))
                        return 0;
                    break;
                }
                int32_t i32;
                if (!thrift_read_i32(info->sock, &i32))
                    return 0;
                *state = (artix_info_upload_state_t) i32;
                break;
            default:
                thrift_skip_field(info->sock, field_type);
        }
    }
    return 1;
}
DO_EXPORT int artix_info_set_upload_state(artix_info_t *info, artix_info_upload_state_t state)
{
    int psize = sizeof(int8_t) + sizeof(int16_t) + sizeof(int32_t);
    char params[psize];
    char *cp = params;
    *cp = T_I32;
    cp++;
    int16_t id = (int16_t)ntohs(1); // just in case
    memcpy(cp, &id, sizeof(id));
    cp += sizeof(id);
    int32_t value = (int32_t)ntohl(state);
    memcpy(cp, &value, sizeof(value));
    cp += sizeof(value);

    int32_t rseqid = 0;
    if (!thrift_send_function(info->sock, SETUPLOADSTATE, rseqid, params, psize))
        return 0;

    if (!thrift_read_header(info->sock, SETUPLOADSTATE, &rseqid))
        return 0;

    int8_t field_type;
    int16_t field_id;
    while (1) {
        if (!thrift_read_field_header(info->sock, &field_type, &field_id))
            return 0;
        if (field_type == T_STOP)
            break;
        thrift_skip_field(info->sock, field_type);
    }
    return 1;
}

DO_EXPORT int artix_info_get_upload_progress(artix_info_t *info, int *percent)
{
    int32_t cseqid = 0;

    if (!thrift_send_function(info->sock, GETUPLOADPROGRESS, cseqid, NULL, 0))
        return 0;

    if (!thrift_read_header(info->sock, GETUPLOADPROGRESS, &cseqid))
        return 0;

    int8_t field_type;
    int16_t field_id;
    *percent = 0;
    while (1) {
        if (!thrift_read_field_header(info->sock, &field_type, &field_id))
            return 0;
        if (field_type == T_STOP)
            break;
        switch (field_id) {
            case 0:
                if (field_type != T_I32) {
                    do_log(LOG_ERR, "type mismatch");
                    if (!thrift_skip_field(info->sock, field_type))
                        return 0;
                    break;
                }
                int32_t i32;
                if (!thrift_read_i32(info->sock, &i32))
                    return 0;
                *percent = i32;
                break;
            default:
                thrift_skip_field(info->sock, field_type);
        }
    }
    return 1;
}
DO_EXPORT int artix_info_set_upload_progress(artix_info_t *info, int percent)
{
    int psize = sizeof(int8_t) + sizeof(int16_t) + sizeof(int32_t);
    char params[psize];
    char *cp = params;
    *cp = T_I32;
    cp++;
    int16_t id = (int16_t)ntohs(1); // just in case
    memcpy(cp, &id, sizeof(id));
    cp += sizeof(id);
    int32_t value = (int32_t)ntohl(percent);
    memcpy(cp, &value, sizeof(value));
    cp += sizeof(value);

    int32_t rseqid = 0;
    if (!thrift_send_function(info->sock, SETUPLOADPROGRESS, rseqid, params, psize))
        return 0;

    if (!thrift_read_header(info->sock, SETUPLOADPROGRESS, &rseqid))
        return 0;

    int8_t field_type;
    int16_t field_id;
    while (1) {
        if (!thrift_read_field_header(info->sock, &field_type, &field_id))
            return 0;
        if (field_type == T_STOP)
            break;
        thrift_skip_field(info->sock, field_type);
    }
    return 1;
}

DO_EXPORT int artix_info_get_upload_error_description(artix_info_t *info, char*** messages, int *count_mes)
{
    int32_t cseqid = 0;
    if (!thrift_send_function(info->sock, GETUPLOADERRORDESCRIPTION, cseqid, NULL, 0))
        return 0;

    if (!thrift_read_header(info->sock, GETUPLOADERRORDESCRIPTION, &cseqid))
        return 0;

    int8_t field_type;
    int16_t field_id;
    while (1) {
        if (!thrift_read_field_header(info->sock, &field_type, &field_id))
            return 0;
        if (field_type == T_STOP)
            break;
        switch (field_id) {
            case 0: {
                if (field_type != T_LIST) {
                    do_log(LOG_ERR, "type mismatch");
                    if (!thrift_skip_field(info->sock, field_type))
                        return 0;
                    break;
                }

                int8_t  type;
                uint32_t i, size;
                char *buf;
                if (!thrift_read_list_header(info->sock, &type, &size))
                    return 0;
                if (type != T_STRING) {
                    do_log(LOG_ERR, "list type mismatch");
                    for (i = 0; i < size; i++)
                        if (!thrift_skip_field(info->sock, type))
                            return 0;
                    break;
                }
                *count_mes = size;
                *messages = (char**) do_malloc (size*sizeof(char*));
                for (i = 0; i < size; i++) {
                    if (!thrift_read_str(info->sock, &buf))
                        return 0;
                    (*messages)[i] = buf;
                }
                break;
            }
            default:
                thrift_skip_field(info->sock, field_type);
        }
    }
    return 1;
}

DO_EXPORT int artix_info_get_available_modules(artix_info_t *info, char*** module_names, int *count_mod)
{
    int32_t cseqid = 0;
    if (!thrift_send_function(info->sock, GETAVAILABLEMODULES, cseqid, NULL, 0))
        return 0;

    if (!thrift_read_header(info->sock, GETAVAILABLEMODULES, &cseqid))
        return 0;

    int8_t field_type;
    int16_t field_id;
    while (1) {
        if (!thrift_read_field_header(info->sock, &field_type, &field_id))
            return 0;
        if (field_type == T_STOP)
            break;
        switch (field_id) {
            case 0: {
                if (field_type != T_LIST) {
                    do_log(LOG_ERR, "type mismatch");
                    if (!thrift_skip_field(info->sock, field_type))
                        return 0;
                    break;
                }

                int8_t  type;
                uint32_t i, size;
                char *buf;
                if (!thrift_read_list_header(info->sock, &type, &size))
                    return 0;
                if (type != T_STRING) {
                    do_log(LOG_ERR, "list type mismatch");
                    for (i = 0; i < size; i++)
                        if (!thrift_skip_field(info->sock, type))
                            return 0;
                    break;
                }
                *count_mod = size;
                *module_names = (char**) do_malloc (size*sizeof(char*));
                for (i = 0; i < size; i++) {
                    if (!thrift_read_str(info->sock, &buf))
                        return 0;
                    (*module_names)[i] = buf;
                }
                break;
            }
            default:
                thrift_skip_field(info->sock, field_type);
        }
    }
    return 1;
}

DO_EXPORT int artix_info_get_module_description(artix_info_t *info, const char *module_name, char*** module_desc, int *count_desc)
{
    int32_t cseqid = 0;
    if (!thrift_send_function(info->sock, GETMODULDESCRIPTION, cseqid, NULL, 0))
        return 0;

    if (!thrift_read_header(info->sock, GETMODULEDESCRIPTION, &cseqid))
        return 0;

    int8_t field_type;
    int16_t field_id;
    while (1) {
        if (!thrift_read_field_header(info->sock, &field_type, &field_id))
            return 0;
        if (field_type == T_STOP)
            break;
        switch (field_id) {
            case 0: {
                if (field_type != T_LIST) {
                    do_log(LOG_ERR, "type mismatch");
                    if (!thrift_skip_field(info->sock, field_type))
                        return 0;
                    break;
                }

                int8_t  type;
                uint32_t i, size;
                char *buf;
                if (!thrift_read_list_header(info->sock, &type, &size))
                    return 0;
                if (type != T_STRING) {
                    do_log(LOG_ERR, "list type mismatch");
                    for (i = 0; i < size; i++)
                        if (!thrift_skip_field(info->sock, type))
                            return 0;
                    break;
                }
                *count_mod = size;
                *module_names = (char**) do_malloc (size*sizeof(char*));
                for (i = 0; i < size; i++) {
                    if (!thrift_read_str(info->sock, &buf))
                        return 0;
                    (*module_names)[i] = buf;
                }
                break;
            }
            default:
                thrift_skip_field(info->sock, field_type);
        }
    }
    return 1;
}


DO_EXPORT int artix_info_set_upload_error_description(artix_info_t *info, char** messages, int count_mes)
{
    size_t len = BUF_LEN;
    char* params = (char*) do_malloc(len + 1);
    char *cp = params;
    *cp = T_LIST;
    cp++;
#ifdef OLD_ARTIX_INFO
    int16_t id = (int16_t)ntohs(0xFFFF); // just in case
#else
    int16_t id = (int16_t)ntohs(1); // just in case
#endif
    memcpy(cp, &id, sizeof(id));
    cp += sizeof(id);
    *cp = T_STRING;
    cp++;
    int32_t size = (int32_t)ntohl(count_mes);
    memcpy(cp, &size, sizeof(size));
    cp += sizeof(size);
    int i, crntlen;
    char *buf;
    for (i = 0; i < count_mes; i++) {
        buf = do_strdup(messages[i]);
        if (cp - params + strlen(buf) + sizeof(size) > len) {
            crntlen = cp - params;
            params = (char*) do_realloc(params, len * 2 + 1);
            len *= 2;
            cp = params + crntlen;
        }
        size = (int32_t)ntohl(strlen(buf));
        memcpy(cp, &size, sizeof(size));
        cp += sizeof(size);
        strncpy(cp, buf, strlen(buf));
        cp += strlen(buf);
        do_free(buf);
    }
    int psize = cp - params;
    int32_t rseqid = 0;
    if (!thrift_send_function(info->sock, SETUPLOADERRORDESCRIPTION, rseqid, params, psize)) {
        do_free(params);
        return 0;
    }
    do_free(params);

    if (!thrift_read_header(info->sock, SETUPLOADERRORDESCRIPTION, &rseqid))
        return 0;
    int8_t field_type;
    int16_t field_id;
    while (1) {
        if (!thrift_read_field_header(info->sock, &field_type, &field_id))
            return 0;
        if (field_type == T_STOP)
            break;
        thrift_skip_field(info->sock, field_type);
    }
    return 1;
}
DO_EXPORT int artix_info_clear_upload_error_description(artix_info_t *info)
{
    return artix_info_set_upload_error_description(info, NULL, 0);
}
DO_EXPORT int artix_info_add_upload_error_description(artix_info_t *info, const char *message)
{
    char **messages = NULL;
    int message_count;
    if (!artix_info_get_upload_error_description(info, &messages, &message_count)) {
        artix_info_list_string_free(messages, message_count);
        return 0;
    }
    message_count++;
    messages = (char**) do_realloc (messages , message_count*sizeof(char*));
    const char *str;
    if (info->use_utf)
        str = UTF_CHARSET;
    else
        str = get_locale_lang();
    iconv_t conv = iconv_open(ARTIX_CHARSET, str);
    if (conv != (iconv_t) -1)
        messages[message_count - 1] = iconv_(conv, message);
    else
        messages[message_count - 1] = do_strdup(message);

    int result = artix_info_set_upload_error_description(info, messages, message_count);

    artix_info_list_string_free(messages, message_count);
    if (conv != (iconv_t) -1)
       iconv_close(conv);


    return result;

}

DO_EXPORT void artix_info_list_string_free(char** messages, int count_mes)
{
    if (messages) {
        int i;
        for (i = 0; i < count_mes; i++)
            if (messages[i]) do_free(messages[i]);
        do_free(messages);
    }
}
/*
static void get_alert_message_code(artix_alert_t *res, const char *message)
{
    const char *ch;
    ch = message + strlen(message) - 1;
    while ( ch > message && ch[0] != ':' ) ch--;
    if ( ch > message && ch[0] == ':' ) {
        res->code = atoi(ch + 1);
        res->message = do_strndup(message, ch - message);
        res->message[ch - message] = '\0';
    }
    else {
        res->message = do_strdup(message);
        res->code = ARTIX_ALERT_OTHER;
    }
}
static char *set_alert_message_code(int code, const char *message)
{
    char *res = do_malloc(10+strlen(message));
    sprintf(res, "%s:%d", message, code);
    return res;
} */
#ifndef OLD_ARTIX_INFO
static int artix_info_set_alert_message(artix_info_t *info, const char *message, int code)
{
    int psize = sizeof(int8_t) + sizeof(int16_t) + sizeof(int32_t) + strlen(message) + sizeof(int8_t) + sizeof(int16_t) + sizeof(int32_t);
    char params[psize];
    char *cp = params;
    *cp = T_STRING;
    cp++;
    int16_t id = (int16_t)ntohs(1); // just in case
    memcpy(cp, &id, sizeof(id));
    cp += sizeof(id);
    int32_t value = (int32_t)ntohl(strlen(message));
    memcpy(cp, &value, sizeof(value));
    cp += sizeof(value);
    memcpy(cp, message, strlen(message));
    cp += strlen(message);
    *cp = T_I32;
    cp++;
    id = (int16_t)ntohs(2);
    memcpy(cp, &id, sizeof(id));
    cp += sizeof(id);
    value = (int32_t)ntohl(code);
    memcpy(cp, &value, sizeof(value));
    cp += sizeof(value);

    int32_t rseqid = 0;
    if (!thrift_send_function(info->sock, SETALERT, rseqid, params, psize))
        return 0;

    if (!thrift_read_header(info->sock, SETALERT, &rseqid))
        return 0;

    int8_t field_type;
    int16_t field_id;
    while (1) {
        if (!thrift_read_field_header(info->sock, &field_type, &field_id))
            return 0;
        if (field_type == T_STOP)
            break;
        thrift_skip_field(info->sock, field_type);
    }
    return 1;
}

DO_EXPORT int artix_info_set_alert(artix_info_t *info, artix_alert_code_t code, const char *message)
{
    artix_alert_t *alert;
    int bcode = 1 << code;
    int res;
    int i;
    const char *text = NULL;
    alert = artix_info_get_alert(info);
    if ( !alert )
        return FALSE;
    
    bcode |= alert->code ;
    for ( i = 0; i <= ARTIX_ALERT_OTHER; i++ )
        if ( (1 << i) & bcode ) {
            text = (i < ARTIX_ALERT_OTHER || !message) ? alert_message[i] : message;
            break;
        }
    artix_alert_free(alert);
    return artix_info_set_alert_message(info, text, bcode);
}
DO_EXPORT int artix_info_clear_alert(artix_info_t *info, artix_alert_code_t code)
{
    artix_alert_t *alert;
    int bcode = 1 << code;
    int i;
    const char *text = NULL;

    alert = artix_info_get_alert(info);
    if ( !alert )
        return FALSE;
    if ( alert->is_set && alert->code & bcode ) {
        alert->code -= bcode;
        if ( !alert->code ) {
            artix_alert_free(alert);
            return artix_info_reset_alert(info);
        }
        for ( i = 0; i <= ARTIX_ALERT_OTHER; i++ )
            if ( (1 << i) & alert->code ) {
                text = alert_message[i];
                break;
            }
        bcode = alert->code;
    }
    artix_alert_free(alert);
    if ( text )
	return artix_info_set_alert_message(info, text, bcode);
    return TRUE;
}
DO_EXPORT int artix_info_reset_alert(artix_info_t *info)
{
    int32_t rseqid = 0;
    if (!thrift_send_function(info->sock, RESETALERT, rseqid, NULL, 0))
        return 0;

    if (!thrift_read_header(info->sock, RESETALERT, &rseqid))
        return 0;

    int8_t field_type;
    int16_t field_id;
    while (1) {
        if (!thrift_read_field_header(info->sock, &field_type, &field_id))
            return 0;
        if (field_type == T_STOP)
            break;
        thrift_skip_field(info->sock, field_type);
    }
    return 1;
}
DO_EXPORT artix_alert_t *artix_info_get_alert(artix_info_t *info)
{
    int32_t cseqid = 0;

    if (!thrift_send_function(info->sock, GETALERT, cseqid, NULL, 0))
        return NULL;

    if (!thrift_read_header(info->sock, GETALERT, &cseqid))
        return NULL;

    int8_t field_type;
    int8_t  buff;
    int16_t field_id;
    int32_t int32;
    char *message;
    artix_alert_t *res = NULL;
    int result;

    while ( TRUE ) {
        if (!thrift_read_field_header(info->sock, &field_type, &field_id)) {
            artix_alert_free(res);
            return NULL;
        }
        if (field_type == T_STOP)
            break;
        switch (field_id) {
            case 0:
                if ( field_type != T_STRUCT ) {
                    do_log(LOG_ERR, "type mismatch");
                    artix_alert_free(res);
                    if (!thrift_skip_field(info->sock, field_type))
                        return NULL;
                    break;
                }
                res = do_malloc0(sizeof(artix_alert_t));
                res->code = -1;
                result = TRUE;
                while ( TRUE ) {
                    if ( !thrift_read_field_header(info->sock, &field_type, &field_id) ) {
                        result=FALSE;break;
                    }
                    if ( field_type == T_STOP )
                        break;
                    switch ( field_id ) {
                        case 1:
                            if ( field_type != T_BOOL ) {
                                do_log(LOG_ERR, "field 1: type mismatch");
                                result=FALSE;
                                break;
                            }
                            if ( !thrift_read_bool(info->sock, &buff) ) {
                                result=FALSE;
                                break;
                            }
                            res->is_set = buff;
                            break;
                        case 2:
                            if ( field_type != T_STRING ) {
                                do_log(LOG_ERR, "field 2: type mismatch");
                                result=FALSE;
                                break;
                            }
                            if ( !thrift_read_str(info->sock, &message) ) {
                                result=FALSE;
                                break;
                            }
                            res->message =  message;
                            break;
                        case 3:
                            if ( field_type != T_I32 ) {
                                do_log(LOG_ERR, "field 3: type mismatch");
                                result=FALSE;
                                break;
                            }
                            
                            if ( !thrift_read_i32(info->sock, &int32) ) {
                                result=FALSE;
                                break;
                            }
                            res->code = int32;
                            break;
                    }
                }
                if ( res->code == -1 )
                    do_log(LOG_ERR, "struct alert is empty");
                if ( !result || res->code == -1 ) {
                    artix_alert_free(res);
                    return NULL;
                }
                break;
            default:
                thrift_skip_field(info->sock, field_type);
        }
    }
    return res;
}
DO_EXPORT void artix_alert_free(artix_alert_t *alert)
{
    if (alert) {
        if (alert->message)
            do_free(alert->message);
        do_free(alert);
    }
}
#endif
