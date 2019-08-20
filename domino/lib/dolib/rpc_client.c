#include <dolib.h>
#include "socket_io.h"
#include "../../misc/define.h"
#include "../../misc/iconv_.h"

#include <errno.h>
#include <string.h>
#ifdef GCRYPT
#include <stdio.h>
#include <gcrypt.h>
#endif
#ifdef ZLIB
#include <zlib.h>
#endif


struct _do_rpc_client_t{
    socket_t sock;
#ifdef WINSOCK
    WSADATA winsock_data;
#endif
    char *host;
    int   port;
    char *user;
    char *password;
    do_exch_flag_t  support_flags;
    do_exch_flag_t  server_support_flags;
#ifdef GCRYPT
    do_crypt_key_t crypt_key;
    do_sessionIV_t sessionIV;
#endif
    int connect;
    int use_utf;
};

#define MAX_TIME_OUT_U 20*60*1000000

static int negotiation(do_rpc_client_t *do_rpc_client);

#define do_read_(buffer,buf_len) do_read(do_rpc_client->sock,buffer,buf_len, MAX_TIME_OUT_U)
#define do_send_(buffer,buf_len) do_send(do_rpc_client->sock,buffer,buf_len)
#ifdef GCRYPT
#define do_read_data(data, time) do_read_data(do_rpc_client->sock, data, &do_rpc_client->sessionIV, &do_rpc_client->crypt_key, time)
#define do_send_data_(data) do_send_data(do_rpc_client->sock, data, &do_rpc_client->sessionIV, &do_rpc_client->crypt_key, do_rpc_client->support_flags)
#else
#define do_read_data(data, time) do_read_data(do_rpc_client->sock, data, time)
#define do_send_data_(data) do_send_data(do_rpc_client->sock, data, do_rpc_client->support_flags)
#endif

static int login(do_rpc_client_t *do_rpc_client);

DO_EXPORT do_rpc_client_t *do_rpc_client_new(const char *host, int port, const char *user, const char *password, int use_utf)
{
    do_rpc_client_t *do_rpc_client = (do_rpc_client_t *)do_malloc(sizeof(do_rpc_client_t));
    do_rpc_client->use_utf = use_utf;
#ifdef WINSOCK
    if( WSAStartup(MAKEWORD(2, 2), &do_rpc_client->winsock_data) ) {
        do_free(do_rpc_client);
        return NULL;
    }
#endif
#ifdef GCRYPT
    if (!gcry_check_version (GCRYPT_VERSION)) {
        do_log(LOG_ERR, "libgcrypt version mismatch");
        do_free(do_rpc_client);
        return NULL;
    }
    gcry_control (GCRYCTL_DISABLE_SECMEM, 0);
    gcry_control (GCRYCTL_INITIALIZATION_FINISHED, 0);
    if (!gcry_control (GCRYCTL_INITIALIZATION_FINISHED_P)) {
        do_log(LOG_ERR, "libgcrypt has not been initialized");
        do_free(do_rpc_client);
        return NULL;
     }
    do_rpc_client->crypt_key.key = NULL;
    do_rpc_client->sessionIV.IV = NULL;

#endif
    do_rpc_client->support_flags = do_exch_support_flags;
    do_rpc_client->host = do_strdup(host);
    do_rpc_client->port = port;
    do_rpc_client->user = do_strdup(user);
    do_rpc_client->password = do_strdup(password);
    do_rpc_client->connect = 0;


    return do_rpc_client;
}
DO_EXPORT void do_rpc_client_free(do_rpc_client_t *do_rpc_client)
{
    if (do_rpc_client->connect)
        do_rpc_client_disconnect(do_rpc_client);
    close_socket(do_rpc_client->sock);
#ifdef WINSOCK
    WSACleanup();
#endif
    do_free(do_rpc_client->host);
    do_free(do_rpc_client->user);
    do_free(do_rpc_client->password);
#ifdef GCRYPT
    if (do_rpc_client->crypt_key.key)
        do_free(do_rpc_client->crypt_key.key);
    if (do_rpc_client->sessionIV.IV)
        do_free(do_rpc_client->sessionIV.IV);
#endif
    do_free(do_rpc_client);
}
#ifdef GCRYPT
DO_EXPORT int do_rpc_client_crypt_init_key(do_rpc_client_t *do_rpc_client, const char *keyfile)
{
    FILE *fp;
    char buff[1024];
    uint8_t hex_byte[3] = {0, 0, 0};
    if ((fp = fopen(keyfile, "r")) == NULL)  {
        do_log(LOG_ERR, "Error opening key file (%s): %s", keyfile, strerror(errno));
        return 0;
    }
    if (!fgets(buff, 1024, fp)) {
        do_log(LOG_ERR, "Error do_read key file (%s): %s", keyfile, strerror(errno));
        fclose(fp);
        return 0;
    }
    if (!fgets(buff, 1024, fp)) {
        do_log(LOG_ERR, "Error do_read key file (%s): %s", keyfile, strerror(errno));
        fclose(fp);
        return 0;
    }
    do_rpc_client->crypt_key.key_len = gcry_cipher_get_algo_keylen(GCRY_CIPHER_AES);
    do_rpc_client->crypt_key.blk_len = gcry_cipher_get_algo_blklen(GCRY_CIPHER_AES);
    if (strlen(buff) < do_rpc_client->crypt_key.key_len*2)  {
        do_log(LOG_ERR, "Invalid key file (%s): %s", keyfile, strerror(errno));
        fclose(fp);
        return 0;
    }
    fclose(fp);
    if (do_rpc_client->crypt_key.key)
        do_free(do_rpc_client->crypt_key.key);

    do_rpc_client->crypt_key.key = do_malloc(do_rpc_client->crypt_key.key_len);
    size_t i;
    unsigned int u;
    for (i = 0; i < do_rpc_client->crypt_key.key_len; i++) {
        hex_byte[0] = buff[i*2];
        hex_byte[1] = buff[i*2 + 1];
		if (sscanf((const char *)hex_byte, "%x", &u) != 1) {
            do_log(LOG_ERR, "Invalid key file (%s): %s", keyfile, strerror(errno));
            fclose(fp);
            return 0;
		}
        do_rpc_client->crypt_key.key[i] = u;
    }
    return 1;
}
#endif
DO_EXPORT int do_rpc_client_connect(do_rpc_client_t *do_rpc_client)
{
    do_rpc_client->connect = 0;
    do_rpc_client->sock = socket(PF_INET, SOCK_STREAM, 0);
	if (invalid_socket(do_rpc_client->sock)) {
	    do_log(LOG_ERR, "socket not open " SOCKERROR_SUFFIX);
        return 0;
	}
	do_log_debug(ZONE, "open socket");
    struct hostent *hp = gethostbyname(do_rpc_client->host);
    if (!hp) {
        do_log(LOG_ERR, "Unknown host %s", do_rpc_client->host);
        return 0;
    }
    sockaddr_in_t dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    memcpy(&dest.sin_addr, hp->h_addr, sizeof(dest.sin_addr));
    dest.sin_port = htons(do_rpc_client->port);

    if (connect(do_rpc_client->sock, (struct sockaddr*) &dest, sizeof(dest)) == -1) {
        do_log(LOG_ERR, "Connect not open " SOCKERROR_SUFFIX);
        close_socket(do_rpc_client->sock);
        return 0;
    }
	do_log_debug(ZONE, "connect socket");

	if (!negotiation(do_rpc_client)) {
        do_log(LOG_ERR, "Negotiation error");
        close_socket(do_rpc_client->sock);
        return 0;
    }
    if (!login(do_rpc_client)) {
        do_log(LOG_ERR, "Login error");
        close_socket(do_rpc_client->sock);
        return 0;
    }
    do_rpc_client->connect = 1;
	return 1;
}

DO_EXPORT int do_rpc_client_send_command(do_rpc_client_t *do_rpc_client, const char *command, const char *params, do_data_t *data, do_data_t *recv_data)
{
    char buf[strlen(command) + strlen(params) + 4];
    sprintf(buf, "%s %s\n", command, params);
    int retval;

    if (!do_send_(buf, strlen(buf)))
        return 0;

    if (data && !do_send_data_(data))
        return 0;

    if (!recv_data)
        return 1;

    do {
        if ((retval = do_read_data(recv_data, -1)) == 0)
            break;
    } while ( DO_IS_EXCH_DATA_INFO(do_data_type(recv_data)) );

    if (do_data_type(recv_data) == DO_EXCH_DATA_TYPE_ERROR) {
        char *error = do_data_error(recv_data);
        do_log(LOG_ERR, error);
        do_free(error);
        retval = 0;
    }
    return retval;
}
DO_EXPORT int do_rpc_client_disconnect(do_rpc_client_t *do_rpc_client)
{
    const char *buf = "exit\n";

    if (!do_send_((void*)buf, strlen(buf)))
        return 0;
    do_rpc_client->connect = 0;
    return 1;
}

static int negotiation(do_rpc_client_t *do_rpc_client)
{
    if (do_read_(&do_rpc_client->server_support_flags, sizeof(do_rpc_client->server_support_flags)) <= 0)
        return 0;

    if (!do_send_(&do_rpc_client->support_flags, sizeof(do_rpc_client->support_flags)))
        return 0;

    do_rpc_client->support_flags &= do_rpc_client->server_support_flags;
#ifdef GCRYPT
    if (do_rpc_client->server_support_flags & DO_EXCH_FLAG_AES) {
        if (do_rpc_client->sessionIV.IV)
            do_free(do_rpc_client->sessionIV.IV);
        do_rpc_client->sessionIV.len = gcry_cipher_get_algo_blklen(GCRY_CIPHER_AES);
        do_rpc_client->sessionIV.IV = do_malloc(do_rpc_client->sessionIV.len);
        if (do_read_(do_rpc_client->sessionIV.IV, do_rpc_client->sessionIV.len) <= 0)
            return 0;
    }
#endif
    if (do_rpc_client->server_support_flags & DO_EXCH_FLAG_LOCALE) {
        const char *str;
        if (do_rpc_client->use_utf)
            str = UTF_CHARSET;
        else
            str = get_locale_lang();
        if (!do_send_((char*)str, strlen(str) + 1))
            return 0;
    }
    return 1;

}
static int login(do_rpc_client_t *do_rpc_client)
{
    do_data_t *data = do_data_new();
#ifdef GCRYPT
    do_data_set_send_flags(data, DO_EXCH_FLAG_AES);
#endif
    do_data_add_strf(data, "%s:%s", do_rpc_client->user, do_rpc_client->password);
    if (!do_send_data_(data)) {
        do_data_free(data);
        return 0;
    }
    do_data_clear(data);
    if (do_read_data(data, 0) <= 0) {
        if (do_data_type(data) == DO_EXCH_DATA_TYPE_ERROR) {
            char *error = do_data_error(data);
            do_log(LOG_ERR, error);
            do_free(error);
        }
        do_data_free(data);
        return 0;
    }
    if (do_data_type(data) != DO_EXCH_DATA_TYPE_OK) {
        char *error;
        if (do_data_type(data) == DO_EXCH_DATA_TYPE_ERROR)
            error = do_data_error(data);
        else
            error = do_data_str(data);
        do_log(LOG_ERR, "error login: %s", error);
        do_free(error);
        do_data_free(data);
        return 0;
    }
    do_data_free(data);
    return 1;
}
