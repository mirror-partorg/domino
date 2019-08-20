#include "print.h"
#include <dolib.h>
#include "socket.h"
#include <errno.h>
#include <stdint.h>

#define MAX_TIME_OUT_USEC 120*1000000
#define PRINT_PORT 15000
#define MOK "ok"
#define CH_CR '\r'
#define CH_LF '\n'

static int write_(socket_t sock, void* data, size_t size);
static int writeln(socket_t sock, const char *text);
static int writeln_(socket_t sock, const char *text, void* data, size_t size);
static int readln(socket_t sock, char *buf, size_t len);
static socket_t connect_(const char *host, int port);

typedef struct PACK_STRUCT {
    int32_t copys;
    int32_t start;
    int32_t end;
    int8_t  flag;
} print_info_t;

int print_html(const char *host, const char *text)
{
#ifdef WINSOCK
    WSADATA winsock_data;
    if( WSAStartup(MAKEWORD(2, 2), &winsock_data) ) {
        return 0;
    }
#endif
    int retval = 0;
    socket_t sock;
    sock = connect_(host, PRINT_PORT);
    if (!invalid_socket(sock)) {
        print_info_t info = {1, 0, 0, 0};
        char buf[1024];
        retval =
           writeln_(sock, "PRCP=", &info , sizeof(info)) &&
           readln(sock, buf, sizeof(buf)) &&
           !strcmp(buf, MOK);
        if ( retval ) {
            int len = strlen(text);
            sprintf(buf, "HTML=%d", len);
            retval =
              writeln_(sock, buf, (void*)text, strlen(text)) &&
              readln(sock, buf, sizeof(buf)) &&
              !strcmp(buf, MOK);
        }
        if (!retval)
            do_log(LOG_ERR, buf);

        close_socket(sock);
    }
#ifdef WINSOCK
    WSACleanup();
#endif
    return retval;
}

static socket_t connect_(const char *host, int port)
{
    socket_t sock;
    sock = socket(PF_INET, SOCK_STREAM, 0);
	if (invalid_socket(sock)) {
	    do_log(LOG_ERR, "socket not open " SOCKERROR_SUFFIX);
        return INVALID_SOCKET;
	}
    struct hostent *hp = gethostbyname(host);
    if (!hp) {
        do_log(LOG_ERR, "Unknown host %s", host);
        return INVALID_SOCKET;
    }
    sockaddr_in_t dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    memcpy(&dest.sin_addr, hp->h_addr, sizeof(dest.sin_addr));
    dest.sin_port = htons(port);

    if (connect(sock, (struct sockaddr*) &dest, sizeof(dest)) == -1) {
        do_log(LOG_ERR, "Connect not open " SOCKERROR_SUFFIX);
        close_socket(sock);
        return INVALID_SOCKET;
    }
	char buf[1024];
    if (!readln(sock, buf, sizeof(buf)) ||
        strcmp(buf, MOK)) {
        do_log(LOG_ERR, "Connect error %s", buf);
        close_socket(sock);
        return INVALID_SOCKET;
    }
    if (!writeln(sock, "name=pharma")) {
        close_socket(sock);
        return INVALID_SOCKET;
    }
    if (!readln(sock, buf, sizeof(buf)) ||
        strcmp(buf, MOK)) {
        do_log(LOG_ERR, "Login error %s", buf);
        close_socket(sock);
        return INVALID_SOCKET;
    }
	return sock;
}

static int read_(socket_t sock, void *buffer, size_t buf_len, int usec)
{
    fd_set readset;
    struct timeval tv;
    int n;
    size_t len = 0;
    while (len < buf_len) {
        FD_ZERO(&readset);
        FD_SET(sock, &readset);
        tv.tv_sec =  usec / 1000000;
        tv.tv_usec = usec % 1000000;
        if (select(sock + 1, &readset, NULL, NULL, &tv) > 0 ) {
            if (!FD_ISSET(sock, &readset))
                return -2; // time out
            if ((n = recv(sock, (char*)buffer + len, buf_len - len, 0)) <=0 )
                return n;
            len += n;
        }
        else
            break;
    }
    return len; // time out
}
static int writeln_(socket_t sock, const char *text, void* data, size_t size)
{
    int retval;
    size_t new_size;
    char *buf = do_malloc(strlen(text) + 10 + size);
    if (!buf) {
        do_log(LOG_ERR, "Can't allocate %d byte", strlen(text) + 10 + size);
        return 0;
    }
    sprintf(buf, "%s\r\n", text);
    new_size = strlen(buf)+size;
    memcpy(buf + strlen(buf), data, size);
    retval = write_(sock, buf, new_size);
    do_free(buf);
    return retval;
}
static int writeln(socket_t sock, const char *text)
{
    char *buf = do_malloc(strlen(text) + 10);
    sprintf(buf, "%s\r\n", text);
    return write_(sock, buf, strlen(buf));
}
static int write_(socket_t sock, void* data, size_t size)
{
    if (send(sock, data, size, 0) < 0 ) {
        do_log(LOG_ERR, "error write: " SOCKERROR_SUFFIX);
        return 0;
    }
    return 1;
}
static int readln(socket_t sock, char *cmd, size_t cmd_len)
{
    size_t len = 0;
    int n = 0;
    while ( (len < cmd_len) && ( n = read_(sock, cmd + len, 1 /*cmd_len - len*/, MAX_TIME_OUT_USEC)) > 0 ) {
        len += n;
        if ( (len > 0) && (cmd[len - 1] == CH_LF || cmd[len - 1] == '\0')) {
            if ((len > 1) && (cmd[len - 1] == CH_LF && cmd[len - 2] == CH_CR)) len--;
                cmd[len - 1] = '\0';
            return 1;
        }
    }
    if (n == -2) {
        do_log(LOG_ALERT, "time is up");
#ifdef DEBUG
        cmd[0] = '\0';
        return 1;
#endif

    }
    else
        do_log(LOG_ERR, "error read: " SOCKERROR_SUFFIX);
    return 0;
}
