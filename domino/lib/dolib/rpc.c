
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <locale.h>
#include <stdarg.h>
#ifdef ZLIB
#include <zlib.h>
#endif
#ifdef GCRYPT
#include <gcrypt.h>
#endif

#include <sys/stat.h>
#include <dolib.h>
#include "../../misc/define.h"
#include "thread.h"
#include "socket_io.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include "../../misc/iconv_.h"

#ifdef PSEUDO_TELNET
#include "telnet.h"
#endif

#ifdef PSEUDO_TELNET
#define MAX_TRY_PASSWORD 3
#define WAIT_FOR_READ_SEC 3
#endif
#define MAX_CLIENTS 500
#define MAX_STEP_WAIT 3
#define MAX_BUFFER_LEN 2047

#define COMMAND_EXIT 0
#define COMMAND_HELP 1
#define COMMAND_STAT 2
#define ADMIN_LEVEL 1


#define MAX_IDENT 100

typedef struct {
    do_rpc_t *ctrl;
    int id;
    thread_t *thread;
    socket_t sock;
    do_exch_flag_t support_flags;
    char   addr[80];
    char   username[MAX_IDENT + 1];
    time_t connect_time;
    int    userlevel;
    iconv_t conv;
#ifdef  PSEUDO_TELNET
    int pseudo_telnet_run;
    do_string_list_t *history;
    int history_pos;
#endif
#ifdef GCRYPT
    do_sessionIV_t iv;
#endif
} do_rpc_user_t;

struct _do_rpc_t {
    thread_t *thread;
    int       tcpport;
#ifdef PSEUDO_TELNET
    thread_t *telnet_thread;
    int       telnet_running;
    int       telnet_tcpport;
#endif
#ifdef WINSOCK
    WSADATA winsock_data;
    int init_winsock;
#endif
    do_rpc_user_t clients[MAX_CLIENTS];
    char     *host;
#ifdef GCRYPT
    do_crypt_key_t key;
#endif
    struct {
        do_proc_state_t *proc_state;
        char *name;
        char *ver;
    }info;
    struct {
        void *owner;
        do_rpc_extended_func *func;
        do_rpc_command_t *commands;
        int command_count;
    } extended;
};
typedef struct {
    int id;
    int helponly;
    do_rpc_command_t *cmd;
    int external;
    do_list_t *param_list;
} do_rpc_command_info_t;

static void do_rpc_run(do_rpc_t *ctrl
#ifdef PSEUDO_TELNET
    , int pseudo_telnet
#endif
       );
static int do_rpc_terminate(do_rpc_t *ctrl, int wait);

static void do_rpc_client_run(do_rpc_user_t *client);
static int  do_rpc_client_init(do_rpc_user_t *client);
static int  do_rpc_exec_command(do_rpc_user_t *client, char *cmd);

// thread methods

static void exec(void* arg);
#ifdef PSEUDO_TELNET
static void exec_telnet(void* arg);
#endif
static void exec_client(void *arg);

// sub methods
static int read_str(do_rpc_user_t *client, char *cmd, size_t cmd_len
#ifdef PSEUDO_TELNET
                   , int echo, int history
#endif
                 );

#ifdef PSEUDO_TELNET
static int send_str(do_rpc_user_t *client, const char* cformat, ...);
#endif

static int login(do_rpc_user_t *client);
static int user_verification(do_rpc_user_t *client, const char *name, const char *password);

static int negotiations(do_rpc_user_t *client);

static do_rpc_user_t *get_crnt_client(do_rpc_t *ctrl, int handle);

#ifdef  PSEUDO_TELNET
static int send_text(do_rpc_user_t *client, void *data, size_t size);
static int user_pref(do_rpc_user_t *client);
static int telnet_init(do_rpc_user_t *client);
static int telnet_comm_skip(do_rpc_user_t *client, char *buf, int len, char *echo_buf, int* wait, int start, int echo);
#endif

#ifdef GCRYPT
static int  crypt_init_key(do_rpc_t *ctrl, const char *key_file);
static void *crypt_init_iv(do_rpc_t *ctrl);
#endif

static void do_rpc_client_clear(do_rpc_user_t *client);

static void do_rpc_cmd_exec(do_rpc_t *ctrl, int handle, int command, do_list_t *param_list, do_data_t *in, do_data_t *out);
static int  do_rpc_cmd_parser(do_rpc_user_t *client, const char *cmd, do_rpc_command_info_t *command, do_data_t *out);
static void do_rpc_cmd_get_version(do_rpc_t *ctrl, do_data_t *out);
static void do_rpc_cmd_get_doc(do_rpc_t *ctrl, do_data_t *out);
static void do_rpc_cmd_get_help(do_rpc_t *ctrl, do_rpc_command_info_t* cmd, do_data_t *out
#ifdef PSEUDO_TELNET
                 , int color
#endif
);
static void do_rpc_set_state(do_rpc_t *ctrl, do_state_t newstate);


DO_EXPORT do_rpc_t *do_rpc_new(const char* name, char* ver,
                     int tcpport, int telnet_tcpport,
                     do_proc_state_t *proc_state,
                     const char* crypt_key_file)
{
    do_rpc_t *ctrl = (do_rpc_t*)do_malloc0(sizeof(do_rpc_t));
#ifdef WINSOCK
    if( WSAStartup(MAKEWORD(2, 2), &ctrl->winsock_data) ) {
        do_free(ctrl);
        return NULL;
    }
    ctrl->init_winsock = 1;
#endif
    char buf[256];
    size_t buflen = sizeof(buf) -1;
    int n;
    if ( (n = gethostname(buf, buflen)) == 0)
        ctrl->host = do_strdup(buf);
    else {
        ctrl->host = do_strdup("?");
        do_log(LOG_ERR, "gethostname %d %s", n, strerror(n));
    }
#ifdef GCRYPT
    if (!gcry_check_version (GCRYPT_VERSION)) {
        do_log(LOG_ERR, "libgcrypt version mismatch");
        do_free(ctrl);
        return NULL;
    }
//    gcry_control (GCRYCTL_SET_THREAD_CBS, &gcry_threads_other);
    gcry_control (GCRYCTL_DISABLE_SECMEM, 0);
    gcry_control (GCRYCTL_INITIALIZATION_FINISHED, 0);
    if (!gcry_control (GCRYCTL_INITIALIZATION_FINISHED_P)) {
        do_log(LOG_ERR, "libgcrypt has not been initialized");
        do_free(ctrl);
        return NULL;
    }
    if (!crypt_init_key(ctrl, crypt_key_file))
        return 0;
#endif
    ctrl->info.proc_state = proc_state;
#ifdef _WIN32
    do_service_set_proc_state(ctrl->info.proc_state);
#endif
    ctrl->info.name = do_strdup(name);
    ctrl->info.ver = do_strdup(ver);
    ctrl->tcpport = tcpport;
    if (ctrl->tcpport)
        thread_new(exec, ctrl, &ctrl->thread);
    else
        ctrl->thread = NULL;
#ifdef PSEUDO_TELNET
    ctrl->telnet_tcpport = telnet_tcpport;
    if (telnet_tcpport)
        thread_new(exec_telnet, ctrl, &ctrl->telnet_thread);
    else
        ctrl->telnet_thread = NULL;
#endif
    return ctrl;
}
DO_EXPORT void do_rpc_set_extended(do_rpc_t *ctrl, void* owner, do_rpc_extended_func *func, do_rpc_command_t *commands, int command_count)
{
    ctrl->extended.owner = owner;
    ctrl->extended.func = func;
    ctrl->extended.commands = commands;
    ctrl->extended.command_count = command_count;
}
DO_EXPORT void do_rpc_free(do_rpc_t *ctrl)
{
    do_rpc_terminate(ctrl, 0);
#ifdef PSEUDO_TELNET
    int i;
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (ctrl->clients[i].history)
            do_string_list_free(ctrl->clients[i].history);
    }
#endif
#ifdef WINSOCK
    //!!if (ctrl->init_winsock)
        //WSACleanup();
#endif
    do_free(ctrl->host);
    do_free(ctrl->info.name);
    do_free(ctrl->info.ver);
#ifdef GCRYPT
    do_free(ctrl->key.key);
#endif
    do_free(ctrl);
}

DO_EXPORT void do_send_percent(do_rpc_t *ctrl, int handle, int perc)
{
    do_rpc_user_t *client;
    client = get_crnt_client(ctrl, handle);
    if (!client || !client->sock)
        return;
#ifdef PSEUDO_TELNET
    if (client->pseudo_telnet_run) {
        send_str(client, "%d percent ", perc);
        return;
    }
#endif
    do_data_t *out = do_data_new();
    int32_t buf = perc;
    do_data_set(out, &buf, sizeof(int32_t));
    do_data_set_type(out, DO_EXCH_DATA_TYPE_INFO_PERCENT);
    if (!do_send_data(client->sock, out,
#ifdef GCRYPT
                   &client->iv, &ctrl->key,
#endif
                   client->support_flags)) {
        close_socket(client->sock);
        client->sock = 0;
    }
    do_data_free(out);
}

DO_EXPORT void do_rpc_send_number(do_rpc_t *ctrl, int handle, int quant)
{
    do_rpc_user_t *client;
    client = get_crnt_client(ctrl, handle);
    if (!client || !client->sock)
        return;
#ifdef PSEUDO_TELNET
    if (client->pseudo_telnet_run) {
        send_str(client, "%d ", quant);
        return;
    }
#endif
    int32_t buf = quant;
    do_data_t *out = do_data_new_from_src(&buf, sizeof(int32_t));
    do_data_set_type(out, DO_EXCH_DATA_TYPE_INFO_NUMBER);
    if (!do_send_data(client->sock, out,
#ifdef GCRYPT
                   &client->iv, &ctrl->key,
#endif
                    client->support_flags)) {
        close_socket(client->sock);
        client->sock = 0;
    }
    do_data_free(out);
}

DO_EXPORT void do_rpc_send_msg(do_rpc_t *ctrl, int handle, const char* cformat, ...)
{
    if (!cformat || !strlen(cformat))
        return;
    do_rpc_user_t *client;
    client = get_crnt_client(ctrl, handle);
    if (!client || !client->sock)
        return;
    va_list ap;
    int sz = 1024, n = -1;
    char *buf = NULL;
    while (n == -1) {
        buf = (char*) do_realloc(buf, sz + 1);
        if (!buf) return;
        va_start(ap, cformat);
        n = vsnprintf(buf, sz , cformat, ap);
        va_end(ap);
        sz *=2;
    }
#ifdef PSEUDO_TELNET
    if (client->pseudo_telnet_run) {
        send_text(client, buf, strlen(buf));
        do_free(buf);
        return;
    }
#endif
    do_data_t *out = do_data_new();
    do_data_set_str(out, buf);
    do_data_set_type(out, DO_EXCH_DATA_TYPE_INFO_MESSAGE);
    if (!do_send_data(client->sock, out,
#ifdef GCRYPT
                   &client->iv, &ctrl->key,
#endif

                    client->support_flags)) {
        close_socket(client->sock);
        client->sock = 0;
    }
    do_data_free(out);
    do_free(buf);
}
DO_EXPORT int do_rpc_close(do_rpc_t *ctrl, int wait)
{
    return do_rpc_terminate(ctrl, wait);
}

static char* do_rpc_get_client_info(do_rpc_t *ctrl,  int handle)
{
    if (handle >= MAX_CLIENTS || !ctrl->clients[handle].sock)
        return NULL;
    char *buf, *info;
    info = do_malloc(1024);
    buf = ctime(&ctrl->clients[handle].connect_time);
    buf[strlen(buf) - 1] = '\0';
    sprintf(info, " id: %d user: %s level: %d from: %s at: %s",
                               handle,
                               ctrl->clients[handle].username,
                               ctrl->clients[handle].userlevel,
                               ctrl->clients[handle].addr,
                               buf);
    return info;
}
static int do_rpc_client_count(do_rpc_t *ctrl)
{
    return MAX_CLIENTS;
}

static int do_rpc_client_live(do_rpc_t *ctrl, int handle)
{
    return (handle < MAX_CLIENTS && ctrl->clients[handle].sock);
}

static int do_rpc_client_kill(do_rpc_t *ctrl, int handle, int id, int kill)
{
    if (ctrl->clients[id].thread) {
        thread_free(ctrl->clients[id].thread);
        do_rpc_client_clear(&ctrl->clients[id]);
        ctrl->clients[id].thread = NULL;
    }
    return 1;
}
/*
static char* do_rpc_get_client_name(do_rpc_t *ctrl, int handle)
{
    if (handle < MAX_CLIENTS)
        return ctrl->clients[handle].username;
    else
        return NULL;
}
*/
static do_rpc_user_t *do_rpc_get_client_current(do_rpc_t *ctrl, int handle)
{
    if (handle < MAX_CLIENTS)
        return &ctrl->clients[handle];
    else
        return NULL;
}
/*
static int do_rpc_get_client_level(do_rpc_t *ctrl, int handle)
{
    if (handle < MAX_CLIENTS)
        return ctrl->clients[handle].userlevel;
    else
        return 0;
}
*/
// local function
static int do_rpc_terminate(do_rpc_t *ctrl, int nowait)
{
    int i, j, flag;
    flag = 0;
    for (j = 0; !nowait && j < MAX_STEP_WAIT; j++) {
        flag = 0;
        for (i = 0; i < MAX_CLIENTS; i++)
            if (ctrl->clients[i].thread && ctrl->clients[i].thread->running) {
                flag = 1;
                break;
            }
        if (!flag) break;
        DO_SEC_SLEEP(1);
    }

    for (i = 0; i < MAX_CLIENTS; i++)
        if (ctrl->clients[i].thread)  {
            thread_free(ctrl->clients[i].thread);
            ctrl->clients[i].thread= NULL;
        }

    if (ctrl->thread) {
        thread_free(ctrl->thread);
        ctrl->thread = NULL;
    }
#ifdef PSEUDO_TELNET
    if (ctrl->telnet_thread) {
        thread_free(ctrl->telnet_thread);
        ctrl->telnet_thread = NULL;
    }
#endif
    return !flag;
}

#define do_log_debug(...)
static void do_rpc_run(do_rpc_t *ctrl
#ifdef PSEUDO_TELNET
    , int pseudo_telnet
#endif
       )
{
    socket_t sock, srvsock;
    sock = socket( PF_INET, SOCK_STREAM, 0);
	if (invalid_socket(sock)) {
	    do_log(LOG_ERR, "socket not open " SOCKERROR_SUFFIX);
        return;
	}
	do_log_debug(ZONE, "open socket");
    int  opt = 1;
    int  opt_len = sizeof(opt);

    if( setsockopt(
        sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, opt_len) )
    {
	    do_log(LOG_ERR, "setsockopt(SO_REUSEADDR) failed with error: " SOCKERROR_SUFFIX);
        return;
    }

#ifdef DEBUG        
    do_log(LOG_INFO,"setsockopt(SO_REUSEADDR)");
#endif    
    sockaddr_in_t srvaddr, cltaddr;
    socklen_t clt_len;

    srvaddr.sin_family      = AF_INET;
    srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
#ifdef PSEUDO_TELNET
    if (pseudo_telnet)
        srvaddr.sin_port        = htons(ctrl->telnet_tcpport);
    else
#endif
    srvaddr.sin_port        = htons(ctrl->tcpport);

    if( bind(sock, (struct sockaddr *)&srvaddr, sizeof(srvaddr)))
    {
	    do_log(LOG_ERR, "bind() failed with error: " SOCKERROR_SUFFIX);
        return;
    }
#ifdef DEBUG    
    do_log(LOG_INFO, "bind()");
#endif    
    if( listen(sock, MAX_CLIENTS) )
    {
	do_log(LOG_ERR, "listen() failed with error: " SOCKERROR_SUFFIX);
        return;
    }
#ifdef DEBUG    
    do_log(LOG_INFO, "listen()");
#endif    
    while ( 1 )
    {
        if (do_proc_state_get_state(ctrl->info.proc_state) == DO_STATE_STOP ||
            do_proc_state_get_state(ctrl->info.proc_state) == DO_STATE_STOPING)  break;

        clt_len = sizeof(cltaddr);
#ifdef DEBUG    
       do_log(LOG_INFO, "accept()");
#endif    
        srvsock = accept(sock, (struct sockaddr *) &cltaddr, &clt_len);
        if (do_proc_state_get_state(ctrl->info.proc_state) == DO_STATE_STOP || do_proc_state_get_state(ctrl->info.proc_state) == DO_STATE_STOPING)  break;

        if ( invalid_socket(srvsock) )  {
            do_log(LOG_ERR, "accept failed: " SOCKERROR_SUFFIX);
            continue;
        }
        sockaddr_t  peer;
        socklen_t peerlen;
        peerlen = sizeof(peer);
#ifdef DEBUG    
       do_log(LOG_INFO, "accepted");
#endif    
        if (0 == getpeername(srvsock, &peer, &peerlen))    {
//!!#ifdef DEBUG        
            do_log(LOG_INFO, "Try connect from %s:%i",
                inet_ntoa(((sockaddr_in_t *)&peer)->sin_addr),
                ntohs(((sockaddr_in_t *)&peer)->sin_port));
//#endif                
        }
        int indx, i;
        indx = -1;
        for (i = 0; i < MAX_CLIENTS; i++)
            if (!ctrl->clients[i].thread || !ctrl->clients[i].thread->running) {
                if (ctrl->clients[i].thread) {
                    thread_free(ctrl->clients[i].thread);
                    ctrl->clients[i].thread = NULL; // just in case
		}
                indx = i;
                break;
            }
        if (indx == -1) {
            close_socket(srvsock);
            continue;
        }
        ctrl->clients[indx].id = indx;
#ifdef PSEUDO_TELNET
        ctrl->clients[indx].pseudo_telnet_run = pseudo_telnet;
#endif
        ctrl->clients[indx].sock = srvsock;
        ctrl->clients[indx].ctrl = ctrl;
        thread_new(exec_client, &ctrl->clients[indx], &ctrl->clients[indx].thread);
    }
    close_socket(sock);
}
static void do_rpc_client_run(do_rpc_user_t *client)
{
    int n;
    if (!do_rpc_client_init(client) || !login(client)) {
        close_socket(client->sock);
        do_log_debug(ZONE, "disconnect from %s", client->addr);
        do_rpc_client_clear(client);
        return;
    }
    char msgbuff[MAX_BUFFER_LEN + 1];
    while (1) {
        if (do_proc_state_get_state(client->ctrl->info.proc_state) == DO_STATE_STOP ||
            do_proc_state_get_state(client->ctrl->info.proc_state) == DO_STATE_STOPING)  {
	    break;
	}
        if (!client->thread || !client->thread->running) {
    	    break;
	}
#ifdef PSEUDO_TELNET
        if (client->pseudo_telnet_run)
            if (!user_pref(client))
                return;
        client->history_pos = 0;
#endif
	n = read_str(client, msgbuff, MAX_BUFFER_LEN
#ifdef PSEUDO_TELNET
            , 1, 1
#endif
            );
	if (n == -1)
	    continue;
        if (!n) {
            close_socket(client->sock);
            do_log(LOG_NOTICE, "connection break  %s from %s", client->username,  client->addr);
            do_log_debug(ZONE, "disconnect from %s", client->addr);
            return;
        }
        if (do_proc_state_get_state(client->ctrl->info.proc_state) == DO_STATE_STOP ||
            do_proc_state_get_state(client->ctrl->info.proc_state) == DO_STATE_STOPING)  break;
        if (!client->thread || !client->thread->running) break;
        if (!strlen(msgbuff)) break;
        n = do_rpc_exec_command(client, msgbuff);
        if (!n) {
            close_socket(client->sock);
            do_log_debug(ZONE, "disconnect from %s", client->addr);
            return;
        }
        if (n < 0) break;
    }
    close_socket(client->sock);
    do_log_debug(ZONE, "disconnect from %s", client->addr);
}

static void exec(void* arg)
{
    do_rpc_run(arg
#ifdef PSEUDO_TELNET
        , 0
#endif
       );
    thread_exit();
}
#ifdef PSEUDO_TELNET
void exec_telnet(void* arg)
{
    do_rpc_run(arg, 1);
}
#endif

void exec_client(void *arg)
{
    do_rpc_user_t *client = arg;
#ifdef GCRYPT
    client->iv.IV = crypt_init_iv(client->ctrl);
#endif
    do_rpc_client_run(client);
    thread_free(client->thread);
    do_rpc_client_clear(client);
    client->thread = NULL;
    thread_exit();
}
static int read_str(do_rpc_user_t *client, char *cmd, size_t cmd_len
#ifdef PSEUDO_TELNET
                   , int echo, int history
#endif
                   )
{
    size_t len = 0;
    int n = 0;
#ifdef PSEUDO_TELNET
    int wait = 0;
#endif
    while ( (len < cmd_len) && ( n = do_read(client->sock, cmd + len, 1 /*cmd_len - len*/, 1)) > 0 ) {
        len += n;
#ifdef PSEUDO_TELNET
        char echo_buf[MAX_BUFFER_LEN + 1];
        if (client->pseudo_telnet_run) {
            len = telnet_comm_skip(client, cmd, len, echo_buf, &wait, len - n - wait, echo);
            if (wait)
                continue;
            if ( echo && strlen(echo_buf) > 0 && ( send(client->sock, echo_buf, strlen(echo_buf), 0) < 0 ) ) {
                do_log(LOG_ERR, "error write: " SOCKERROR_SUFFIX);
                return 0;
            }
        }
#endif

#ifdef PSEUDO_TELNET
// CR LF || LF || Null
        if ( (len > 0) && (cmd[len - 1] == CH_LF || cmd[len - 1] == '\0')) {
            if ((len > 1) && (cmd[len - 1] == CH_LF && cmd[len - 2] == CH_CR)) len--;
#else
        if ( (len > 0) && (cmd[len - 1] == '\n' || cmd[len - 1] == '\0')) {

#endif
                cmd[len - 1] = '\0';
#ifdef PSEUDO_TELNET
            if (client->pseudo_telnet_run && echo && history && strlen(cmd))
                do_string_list_add_alloc(client->history, cmd);
#endif
            return 1;
        }
    }
    if (n == -2){
        do_log(LOG_ALERT, "time is up");
	return -1;
    }
    else
        do_log(LOG_ERR, "error read: " SOCKERROR_SUFFIX);
    return 0;
}
#ifdef PSEUDO_TELNET
static int telnet_comm_skip(do_rpc_user_t *client, char *buf, int len, char *echo_buf, int *wait, int start, int echo )
{
    int echo_len = 0, i, j, k;//, up, down;
    *wait = 0;
    for (i = start; i < len ; i++) {
        if (buf[i] == CH_ESCAPE) { //
            if (len < i + 3) {
                *wait = len - i;
                return len;
            }
            if (echo && (buf[i+1] = '[') && (
                   (buf[i+2] == 'A') ||
                   (buf[i+2] == 'B')
                    )) {
                echo_len = 0;
                for (k = 0; k < i; k++) {
                    strcpy(echo_buf + echo_len, "\x1b[D \x1b[D");
                    echo_len += strlen("\x1b[D \x1b[D");
                }
                echo_buf[echo_len] = '\0';
                if (client->pseudo_telnet_run) {
                    if (buf[i+2] == 'A') {
                        if (client->history->count - client->history_pos > 0) {
                            strcpy(buf, client->history->list[client->history->count - client->history_pos - 1]);
                            strcpy(echo_buf + echo_len, client->history->list[client->history->count - client->history_pos - 1]);
                            client->history_pos++;
                            return strlen(buf);
                        }
                    }
                    if (buf[i+2] == 'B') {
                        if (client->history_pos  > 0) {
                            client->history_pos --;
                            strcpy(buf, client->history->list[client->history->count - client->history_pos - 1]);
                            strcpy(echo_buf + echo_len, client->history->list[client->history->count - client->history_pos - 1]);
                            return strlen(buf);
                        }
                    }
                }
                return 0;
            }
            len -= 3;
            memmove(buf + i, buf + i + 3, len - i);
            i--;
        }
        else
        if ((unsigned char)buf[i] == IAC) {
            if (len < i + 2) {
                *wait = len - i;
                return len;
            }
            if (buf[i + 1] == CH_ESCAPE) {
                if (len < i + 3) {
                    *wait = len - i;
                    return len;
                }
                len -= 3;
            }
            else
                if ((unsigned char)buf[i] == IAC) {
                    if ((unsigned char)buf[i + 1] == DONT ||
                        (unsigned char)buf[i + 1] == DO ||
                        (unsigned char)buf[i + 1] == WONT ||
                        (unsigned char)buf[i + 1] == WILL) {
                            if (len < i + 3) {
                                *wait = len - i;
                                return len;
                            }
                            len -= 3;
                    }
                    else
                        if ((unsigned char)buf[i + 1] == SB) { // suboption skip
                            int j = i + 2;
                            for (; j < len && ((unsigned char)buf[j] != IAC || (unsigned char)buf[j + 1] == IAC) ; j++);
                            if (j >= len) {
                                *wait = len - i;
                                return len;
                            }
                            len -= j - i + 1;
                        }
                }
            memmove(buf + i, buf + i + 3, len - i);
            i--;
        }
        else
        if (buf[i] == CH_BACKSPACE || buf[i] == CH_F16) {
            len--; j = i;
            if (len > 0) {
                len--; j--;
                strcpy(echo_buf + echo_len, "\x1b[D \x1b[D");
                echo_len += strlen("\x1b[D \x1b[D");
            }
            memmove(buf + j, buf + i + 1, len - j);
            i = j;
        }
        else
        if (buf[i] == '\0') {//linux telnet
            buf[i] = '\n';
            echo_buf[echo_len++] = '\n';
        }
        else
            echo_buf[echo_len++] = buf[i];
    }
    echo_buf[echo_len] = '\0';
    return len;
};
int telnet_read(socket_t sock, void *buffer, size_t buf_len, int usec)
{
    fd_set readset;
    struct timeval tv;
    int n, c;
    size_t len = 0;

    if (!buf_len)
        return 0;


    FD_ZERO(&readset);
    FD_SET(sock, &readset);
    tv.tv_sec =  usec / 1000000;
    tv.tv_usec = usec % 1000000;
    c = select(sock + 1, &readset, NULL, NULL, &tv);

    switch (c) {
        case 0:
            return -2;
        case 1:
            do {
                n = recv(sock, buffer + len, buf_len - len, 0);
            } while (n == -1 && errno == EINTR);
            if (n == -1) {
                do_log(LOG_ERR, "error read: " SOCKERROR_SUFFIX);
                return -1;
            }
            len += n;
            break;
        case -1:
        default:
            do_log(LOG_ERR, "error read: " SOCKERROR_SUFFIX);
            return -1;
    }
    return len;
}

static int telnet_init(do_rpc_user_t *client)
{
    unsigned char buf[MAX_BUFFER_LEN + 1];
    int n;
    if ( (n = do_read(client->sock, buf, MAX_BUFFER_LEN, 0)) == -1 ) { // read from client (maybe empty)
        do_log(LOG_ERR, "error read: " SOCKERROR_SUFFIX);
        return 0;
    }
    client->pseudo_telnet_run = 1;
    do_log_debug(ZONE, "client connect for telnet");
    // echo off
    buf[0] = IAC; buf[1] = WILL; buf[2] = TELOPT_ECHO;
    buf[3] = IAC; buf[4] = WILL; buf[5] = TELOPT_SGA;
    do_log_debug(ZONE, "send to telnet client init");
    if ( send(client->sock, (char*) buf, 6, 0) < 0 ) {
        do_log(LOG_ERR, "error write: " SOCKERROR_SUFFIX);
        return 0;
    }
    do_log_debug(ZONE, "read from telnet client init");
    if ( n <= 0 ) { // server send param first, read from client
        if ( (n = telnet_read(client->sock, buf, MAX_BUFFER_LEN, 10000000)) < 0 ) { // read answer
            do_log(LOG_ERR, "error read : " SOCKERROR_SUFFIX);
            return 0;
        }
        if ( buf[0] != IAC ) {
            do_log(LOG_ERR, "error stream from client: ");
            return 0;
        }
    }
    return 1;
}
static int send_text(do_rpc_user_t *client, void *data, size_t size)
{
    if (size == 0)
        return 1;

    char buf[MAX_BUFFER_LEN + 2];
    buf[0] = IAC; buf[1] = WILL; buf[2] = TELOPT_BINARY;
    if ( send(client->sock, (char*) buf, 3, 0) < 0 ) {
        do_log(LOG_ERR, "error write: " SOCKERROR_SUFFIX);
        return 0;
    }
    unsigned char *head;
    size_t i = 0, k;
    while ( i < size ) {
        head = (unsigned char*)data + i;
        for (k = 0; (k + i < size) && head[k] != '\n' && head[k] != IAC && k < MAX_BUFFER_LEN; k++);
        if (
            ( (k >= MAX_BUFFER_LEN || k + i >= size) && head[k] != '\n' && head[k] != IAC) ||
            (head[k] == IAC && k + i < size && head[k + 1] == IAC) ||
            (head[k] == '\n' && k > 0 && head[k - 1] == '\r')
           )  {
            if ( send(client->sock, (char*) head, k , 0) < 0 ) {
                do_log(LOG_ERR, "error write: " SOCKERROR_SUFFIX);
                return 0;
            }
        }
        else {
            if (head[k] == IAC) {
                strncpy(buf, (char*) head, k + 1);
                buf[k + 1] = IAC;
            }
            else { // '\n'
                strncpy(buf, (char*) head, k);
                buf[k] = '\r';
                buf[k + 1] = '\n';
            }
            if ( send(client->sock, buf, k + 2, 0) < 0 ) {
                do_log(LOG_ERR, "error write: " SOCKERROR_SUFFIX);
                return 0;
            }
        }
        i += k + 1;
    }
    if (((char*)data)[size - 1] != '\n') {
        if ( send (client->sock, "\n\r", 2, 0) < 0) {
                do_log(LOG_ERR, "error write: " SOCKERROR_SUFFIX);
                return 0;
            }
    }
    buf[0] = IAC; buf[1] = DONT; buf[2] = TELOPT_BINARY;
    if ( send(client->sock, (char*) buf, 3, 0) < 0 ) {
        do_log(LOG_ERR, "error write: " SOCKERROR_SUFFIX);
        return 0;
    }
    return 1;
}
#endif


static int login(do_rpc_user_t *client)
{
    char password[MAX_IDENT + 1], username[MAX_IDENT + 1];
#ifdef PSEUDO_TELNET
    int i;
    if (client->pseudo_telnet_run) {
        for (i = 0; i < MAX_TRY_PASSWORD; i++ ) {
            if (!send_str(client, "username:"))  return 0;
            if (read_str(client, username, MAX_IDENT, 1, 0) != 1) return 0;
            if (!strlen(username)) { // empty username
                send_str(client, "get out !\n\r");
                do_log(LOG_NOTICE, "authentication failer from %s", client->addr);
                DO_SEC_SLEEP(WAIT_FOR_READ_SEC);
                return 0;
            }
            do_log_debug(ZONE, "username :%s", username);
            if (!send_str(client, "password:")) return 0;
            if (read_str(client, password, MAX_IDENT, 0, 0) != 1) return 0;
            if (user_verification(client, username, password))  {
                //do_log(LOG_NOTICE, "succes authentication for %s from %s", username, client->addr);
                if (!send_str(client, "\n\rsucces authentication for %s from %s\n\r", username, client->addr)) return 0;
                strcpy(client->username, username);
                return 1;
            }
            if (!send_str(client, "\n\rlogin incorrect\n\r")) return 0;
            DO_SEC_SLEEP(1);
        }
        do_log(LOG_NOTICE, "authentication failer for %s from %s", username,  client->addr);
        if (!send_str(client, "get out !\n\r")) return 0;
        DO_SEC_SLEEP(WAIT_FOR_READ_SEC);
        return 0;
    }
#endif
    do_data_t *data = do_data_new();

    if (!do_read_data(client->sock, data,
#ifdef GCRYPT
                   &client->iv, &((do_rpc_t*)client->ctrl)->key,
#endif
                    0)) {
        do_log(LOG_ERR, "Error read authentication data from %s", client->addr);
        do_data_free(data);
        return 0;
    }
    int ret;
    char buf[MAX_BUFFER_LEN + 1];
    strncpy(buf, do_data_get(data), (MAX_BUFFER_LEN < do_data_size(data)) ? MAX_BUFFER_LEN : do_data_size(data));
    buf[(MAX_BUFFER_LEN < do_data_size(data)) ? MAX_BUFFER_LEN : do_data_size(data)] = '\0';
    do_scanner_t scan;
    do_scanner_init(&scan, (char*)buf, client->conv);
    if (do_scanner_step(&scan) != do_lx_ident) return 0;
    strcpy(username, scan.lexem);
    if (do_scanner_step(&scan) != do_lx_colon) return 0;
    if (do_scanner_step(&scan) != do_lx_ident && scan.lex != do_lx_cinteger) return 0;
    strcpy(password, scan.lexem);
    ret = user_verification(client, username, password);
    if (ret)  {
        //do_log(LOG_NOTICE, "succes authentication for %s from %s", username, client->addr);
        strcpy(client->username, username);
        do_data_free(data);
        do_send_ok(client->sock);
        return 1;
    }
    do_log(LOG_NOTICE, "authentication failer for %s from %s", username,  client->addr);
    do_data_free(data);
    return 0;
}


#ifdef PSEUDO_TELNET
static int user_pref(do_rpc_user_t *client)
{
    if ( client->userlevel > 0 )
        return send_str(client, COLOR_RED "%s@%s " COLOR_BLUE "~ # " FONT_DEFAULT, client->ctrl->host, client->ctrl->info.name);
    else
        return send_str(client, COLOR_GREEN "%s@%s " COLOR_BLUE "~ S " FONT_DEFAULT, client->ctrl->host, client->ctrl->info.name);
}
#endif

static int user_verification(do_rpc_user_t *client, const char *name, const char *password)
{
    int ret;

    ret = do_authenticate(name, password, &client->userlevel);
    if (!ret)
        return 0;
    return ret;
}
static void rpc_params_free(do_rpc_command_info_t *cmd)
{
    if (!cmd->param_list)
        return;
    int i;
    for (i = 0; i < cmd->param_list->count; i++)
        switch (((do_rpc_command_param_t*)cmd->param_list->list[i])->type) {
            case do_lx_cstring:
                if (((do_rpc_command_param_t*)cmd->param_list->list[i])->value.string)
                    do_free(((do_rpc_command_param_t*)cmd->param_list->list[i])->value.string);
                break;
            default:
                break;
        }
    do_list_free(cmd->param_list);
    cmd->param_list = NULL;
}
static int do_rpc_exec_command(do_rpc_user_t *client, char *cmd)
{
    if (!cmd || !strlen(cmd))
        return 1;

    do_rpc_command_info_t command;
    do_data_t *do_data_in  = do_data_new(),
                *do_data_out = do_data_new();
    command.param_list = NULL;

    while (1) {
        if (!do_rpc_cmd_parser(client, cmd, &command, do_data_out))
            break;
        do_log(LOG_INFO, "execute command \"%s\"", cmd);//!!
        if (command.cmd->level > client->userlevel) {
            do_data_set_err(do_data_out, "Permission denied");
            break;
        }
        if (command.helponly || (!command.external && command.id == COMMAND_HELP)) {
            do_rpc_cmd_get_help(client->ctrl, &command, do_data_out
#ifdef PSEUDO_TELNET
                 , client->pseudo_telnet_run == 1
#endif
            );
            break;
        }
        if (!command.external) {
            if (command.id == COMMAND_EXIT) {
                do_data_free(do_data_in);
                do_data_free(do_data_out);
                return -1;//!!
            }
            if (command.id == COMMAND_STAT) {
                do_data_set_str(do_data_out, do_proc_state_get_name(do_proc_state_get_state(client->ctrl->info.proc_state)));
                break;
            }
        }
        if (do_proc_state_get_state(client->ctrl->info.proc_state) == DO_STATE_STARTING ||
            do_proc_state_get_state(client->ctrl->info.proc_state) == DO_STATE_RESTARTING) {
            do_data_set_err(do_data_out,
#ifdef _WIN32
                              "Service is not init yet"
#else
                              "Daemon is not init yet"
#endif
            );
            break;
        }
        if (command.cmd->data)
#ifdef PSEUDO_TELNET
            if (!client->pseudo_telnet_run)
#endif
            if (!do_read_data(client->sock, do_data_in,
#ifdef GCRYPT
                           &client->iv, &((do_rpc_t*)client->ctrl)->key,
#endif
                            0))
                break;

        if (command.external) {
            if (!client->ctrl->extended.func)
                do_data_set_err(do_data_out, "Undefined external engine");
            else
                client->ctrl->extended.func(client->id, client->ctrl->extended.owner, command.id, command.param_list, do_data_in, do_data_out);
        }
        else
            do_rpc_cmd_exec(client->ctrl, client->id, command.id, command.param_list, do_data_in, do_data_out);
        break;
        rpc_params_free(&command);
    }
    rpc_params_free(&command);

    do_data_free(do_data_in);
    int retval;
#ifdef PSEUDO_TELNET
    if (client->pseudo_telnet_run)
        retval = send_text(client, do_data_get(do_data_out), do_data_size(do_data_out));
    else
#endif
        retval = do_send_data(client->sock, do_data_out,
#ifdef GCRYPT
                   &client->iv, &((do_rpc_t*)client->ctrl)->key,
#endif
                    client->support_flags);

    do_data_free(do_data_out);
    return retval;
}

static int do_rpc_client_init(do_rpc_user_t *client)
{
#ifdef  PSEUDO_TELNET
    client->history = do_string_list_new();
    client->history_pos = 0;
#endif
    sockaddr_t  peer;
    socklen_t peerlen;
    peerlen = sizeof(peer);
    client->addr[0] = '\0';
    client->username[0] = '\0';
    client->connect_time = 0;
    client->conv = (iconv_t) -1;

    if (0 == getpeername(client->sock, &peer, &peerlen))    {
        do_log_debug(ZONE, "Connect from %s:%i",
            inet_ntoa(((sockaddr_in_t *)&peer)->sin_addr),
            ntohs(((sockaddr_in_t *)&peer)->sin_port));
        strcpy(client->addr, inet_ntoa(((sockaddr_in_t *)&peer)->sin_addr));
    }

    client->connect_time = time(NULL);

#ifdef PSEUDO_TELNET
    if (client->pseudo_telnet_run) {
        if (!telnet_init(client)) {
            close_socket(client->sock);
            do_log_debug(ZONE, "disconnect from %s", client->addr);
            return 0;
        }
        if (do_proc_state_get_state(client->ctrl->info.proc_state) == DO_STATE_STARTING ||
            do_proc_state_get_state(client->ctrl->info.proc_state) == DO_STATE_RESTARTING) {
            send_str(client, "%s is not init yet\n\r", client->ctrl->info.name);
            DO_SEC_SLEEP(1);
            close_socket(client->sock);
            do_log_debug(ZONE, "disconnect from %s", client->addr);
            return 0;
        }
        return 1;
    }
    else {
#endif
    if (do_proc_state_get_state(client->ctrl->info.proc_state) == DO_STATE_STARTING ||
        do_proc_state_get_state(client->ctrl->info.proc_state) == DO_STATE_RESTARTING) {
        close_socket(client->sock);
        do_log_debug(ZONE, "disconnect from %s", client->addr);
        return 0;
    }
    return negotiations(client);
#ifdef PSEUDO_TELNET
    }
#endif
}

static int negotiations(do_rpc_user_t *client)
{
    do_exch_flag_t support_flags = do_exch_support_flags;
    if (!do_send(client->sock, &support_flags, sizeof(support_flags))) {
        do_log(LOG_ERR, "Error send support flags to %s", client->addr);
        return 0;
    }

    if (do_read(client->sock, &support_flags, sizeof(support_flags), 1) != sizeof(support_flags)) {
        do_log(LOG_ERR, "Error read support flags from %s", client->addr);
        return 0;
    }
    client->support_flags = support_flags & do_exch_support_flags;
    client->conv = (iconv_t) -1;
#ifdef GCRYPT
    do_rpc_t *ctrl = client->ctrl;
    //do_log(LOG_INFO, "Send 1iv to %s %d", client->addr, client->support_flags);
    if (client->support_flags & DO_EXCH_FLAG_AES) {
        //do_log(LOG_INFO, "Send iv to %s", client->addr);
        if (!do_send(client->sock, client->iv.IV, ctrl->key.blk_len)) {
            do_log(LOG_ERR, "Error send iv to %s", client->addr);
            return 0;
        }
    }
#endif
    if (client->support_flags & DO_EXCH_FLAG_LOCALE) {
        char loc[DO_EXCH_LOCALE_NAME_SIZE];
        if (read_str(client, loc, DO_EXCH_LOCALE_NAME_SIZE
#ifdef PSEUDO_TELNET
            ,0,0
#endif
           ) != 1 ) {
            do_log(LOG_ERR, "Error read client locale name from %s", client->addr);
            return 0;
        }
        else {
            const char *str = get_locale_lang();
            if (strcmp(str, loc)) {
                client->conv = iconv_open(str, loc);
                if (client->conv == (iconv_t)-1) {
                    do_log(LOG_ERR, "Error iconv_open (\"%s\" to \"%s\") ", loc, str);
                    return 0;
                }
            }
        }
    }

    return 1;
}

static do_rpc_user_t *get_crnt_client(do_rpc_t *ctrl, int handle)
{
    if (handle >= 0 && handle < MAX_CLIENTS && ctrl->clients[handle].thread)
        return &ctrl->clients[handle];
    return NULL;
}

#ifdef GCRYPT
static int crypt_init_key(do_rpc_t *ctrl, const char *key_file)
{
    FILE *fp;
    char buff[1024];
    uint8_t hex_byte[3] = {0, 0, 0};
    if ((fp = fopen(key_file, "r")) == NULL)  {
        do_log(LOG_ERR, "Error opening key file (%s): %s", key_file, strerror(errno));
        return 0;
    }
    if (!fgets(buff, 1024, fp)) {
        do_log(LOG_ERR, "Error read key file (%s): %s", key_file, strerror(errno));
        fclose(fp);
        return 0;
    }
    if (!fgets(buff, 1024, fp)) {
        do_log(LOG_ERR, "Error read key file (%s): %s", key_file, strerror(errno));
        fclose(fp);
        return 0;
    }
    ctrl->key.key_len = gcry_cipher_get_algo_keylen(GCRY_CIPHER_AES);
    ctrl->key.blk_len = gcry_cipher_get_algo_blklen(GCRY_CIPHER_AES);
    if (strlen(buff) < ctrl->key.key_len*2)  {
        do_log(LOG_ERR, "Invalid key file (%s): %s", key_file, strerror(errno));
        fclose(fp);
        return 0;
    }
    fclose(fp);
    ctrl->key.key = do_malloc(ctrl->key.key_len);
    size_t i;
    unsigned int u;
    for (i = 0; i < ctrl->key.key_len; i++) {
        hex_byte[0] = buff[i*2];
        hex_byte[1] = buff[i*2 + 1];
		if (sscanf((const char *)hex_byte, "%x", &u) != 1) {
            do_log(LOG_ERR, "Invalid key file (%s): %s", key_file, strerror(errno));
            fclose(fp);
            return 0;
		}
        ctrl->key.key[i] = u;
    }
    return 1;
}
static void *crypt_init_iv(do_rpc_t *ctrl)
{
    unsigned char *iv = do_malloc(ctrl->key.blk_len);
    gcry_randomize(iv, ctrl->key.blk_len, GCRY_STRONG_RANDOM);
    return iv;
}
#endif
static void do_rpc_client_clear(do_rpc_user_t *client)
{
    if (client->conv != (iconv_t) -1)
        iconv_close(client->conv);
    client->conv = (iconv_t) -1;
#ifdef PSEUDO_TELNET
    if (client->history) {
        do_string_list_free(client->history);
        client->history = NULL;
    }
#endif
#ifdef GCRYPT
    do_free(client->iv.IV);
#endif
}
#ifdef PSEUDO_TELNET
static int send_str(do_rpc_user_t *client, const char* cformat, ...)
{
    char *buf = NULL;
    if (!cformat)
        return 1;

    va_list ap;
    int sz = 1024, n = -1;
    while (n == -1) {
        buf = (char*)do_realloc(buf, sz + 1);
        if (!buf) return 0;
        va_start(ap, cformat);
        n = vsnprintf(buf, sz , cformat, ap);
        va_end(ap);
        sz *=2;
    }
    if (send(client->sock, buf, strlen(buf), 0) < 0 ) {
        do_log(LOG_ERR, "error write: " SOCKERROR_SUFFIX);
        do_free(buf);
        return 0;
    }
    do_free(buf);
    return 1;
}
#endif

static void  locale(do_data_t *out);
static void  sessions(do_rpc_t *ctrl, do_list_t* params, do_data_t *out, int clientId);
static void  kill(do_rpc_t *ctrl, do_list_t* params, do_data_t *out, int clientId);
static void  chstate(do_rpc_t *ctrl, do_list_t* params, do_state_t newstate, do_data_t *out, int handle);
static void  get_file(do_rpc_user_t *client, do_list_t* params, do_data_t *out);
static void  put_file(do_rpc_user_t *client, do_list_t* params, do_data_t *in, do_data_t *out);

typedef  enum {
    CMD_EXIT = COMMAND_EXIT,
    CMD_HELP = COMMAND_HELP,
    CMD_STAT = COMMAND_STAT,
    CMD_STOP,
    CMD_RESTART,
    CMD_RESUME,
    CMD_PAUSE,
    CMD_LOCALE,
    CMD_SESSIONS,
    CMD_KILL_SESSION,
    CMD_DOC,
    CMD_GET_FILE,
    CMD_PUT_FILE,
    CMD_VER,
    CMD_UNDEFINE,
} command_t;

static const do_rpc_command_t commands[CMD_UNDEFINE] = {
    {"exit", "exit", "exit", 0, 0},
    {"help", "this screen", "help", 0, 0},
    {"state", "view process state", "sorry", 0, 0},
    {"stop", "exit & kill process.", "exit & kill process.", 1, 0},
    {"restart", "restart process", "restart process" "\n" "\tparam: [nowait]", 1, 0},
    {"resume", "resume process", "resume process" "\n" "\tparam: [nowait]", 1, 0},
    {"pause", "pause process", "pause process" "\n" "\tparam: [nowait]", 1, 0},
    {"locale", "view locale of daemon", "sorry", 0, 0},
    {"sessions", "view open sessions", "view active sessions " "\n" "\tparam: [id | count]", 0, 0},
    {"kill", "kill session", "kill session " "\n" "\tparam: <id> [kill] [nowait]", 1, 0},
    {"doc", "display the doc pages", "doc", 0, 0},
    {"get_file", "get file","get_file " "\n" "\tparam: <file name>", 0, 0},
    {"put_file", "send file","put_file " "\n" "\tparam: <file name><LF><data>", 0, 1},
    {"ver", "version information", "show version information", 0, 0}};


static void do_rpc_cmd_exec(do_rpc_t *ctrl, int handle, int command, do_list_t *param_list, do_data_t *in, do_data_t *out)
{
    switch (command) {
        case CMD_VER:
            do_rpc_cmd_get_version(ctrl, out);
            break;
        case CMD_STAT:
            do_data_set_str(out, do_proc_state_get_name(do_proc_state_get_state(ctrl->info.proc_state)));
            break;
        case CMD_STOP:
            chstate(ctrl, param_list, DO_STATE_STOPING, out, handle);
            break;
        case CMD_RESTART:
            chstate(ctrl, param_list, DO_STATE_RESTARTING, out, handle);
            break;
        case CMD_PAUSE:
            chstate(ctrl, param_list, DO_STATE_PAUSING, out, handle);
            break;
        case CMD_RESUME:
            chstate(ctrl, param_list, DO_STATE_RESUMING, out, handle);
            break;
        case CMD_LOCALE:
            locale(out);
            break;
        case CMD_SESSIONS:
            sessions(ctrl, param_list, out, handle);
            break;
        case CMD_KILL_SESSION:
            kill(ctrl, param_list, out, handle);
            break;
        case CMD_DOC:
            do_rpc_cmd_get_doc(ctrl, out);
            break;
        case CMD_GET_FILE:
            get_file(do_rpc_get_client_current(ctrl, handle), param_list, out);
            break;
        case CMD_PUT_FILE:
            put_file(do_rpc_get_client_current(ctrl, handle), param_list, in, out);
            break;
    }
}
static double do_atof(const char *str)
{
    char decimal_point = localeconv()->decimal_point[0];
    if (decimal_point !=  '.')
    {
        char* str1 = do_strdup(str);
        char* cp = str1;
        for (; *cp; cp++)
            if (*cp == '.')
            {
                *cp = decimal_point;
                break;
            };
        double result = atof(str1);
        do_free(str1);
        return result;
    }
    else
        return atof(str);
}
# define ISLEAP(year)	((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))
#define	SECS_PER_HOUR (60 * 60)
static const unsigned short int __mon_yday[2][13] = {
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};
static void day_of_the_week (struct tm *tm)
{
  int corr_year = 1900 + tm->tm_year - (tm->tm_mon < 2);
  int wday = (-473
	      + (365 * (tm->tm_year - 70))
	      + (corr_year / 4)
	      - ((corr_year / 4) / 25) + ((corr_year / 4) % 25 < 0)
	      + (((corr_year / 4) / 25) / 4)
	      + __mon_yday[0][tm->tm_mon]
	      + tm->tm_mday - 1);
  tm->tm_wday = ((wday % 7) + 7) % 7;
}

static void day_of_the_year (struct tm *tm)
{
  tm->tm_yday = (__mon_yday[ISLEAP(1900 + tm->tm_year)][tm->tm_mon]
		 + (tm->tm_mday - 1));
}
static int do_atodate(const char *str, struct tm *tm)
{
    tm->tm_isdst = 0; // no daylight
    memset(tm, 0, sizeof(*tm));

    char *cp = (char*)str;
    tm->tm_mday = atoi(cp) ;
    while (*cp && *cp != '/' && *cp != '.')
      ++cp;
    if (*cp) ++cp;
    tm->tm_mon = atoi(cp) - 1;
    while (*cp && *cp != '/' && *cp != '.')
      ++cp;
    if (*cp) ++cp;
    tm->tm_year = atoi(cp);
    if (tm->tm_year > 1000)
        tm->tm_year -= 1900;
    else
        tm->tm_year += 100;

    day_of_the_week(tm);
    day_of_the_year(tm);
    return (tm->tm_year > -70 && tm->tm_year < 10000) && (tm->tm_mon >=0 && tm->tm_mon <12) && (tm->tm_mday > 0 && tm->tm_mday < 32);
}

static void rpc_param_add_from_scan(do_list_t *params, do_scanner_t *scan)
{
    if (scan->lex == do_lx_end ||
        scan->lex == do_lx_error)
        return;

    do_rpc_command_param_t *param;
    param = do_malloc0(sizeof(do_rpc_command_param_t));
    param->type = scan->lex;
    do_list_add(params, param);
    switch (param->type) {
        case do_lx_equal:
        case do_lx_question:
        case do_lx_minus:
        case do_lx_plus:
        case do_lx_colon:
        case do_lx_comma:
            break;
        case do_lx_cinteger:
            param->value.integer = atoi(scan->lexem);
            break;
        case do_lx_cstring:
        case do_lx_ident:
            param->value.string = do_strdup(scan->lexem);
            break;
        case do_lx_cdate:
            do_atodate(scan->lexem, &param->value.date);
            break;
        case do_lx_creal:
            param->value.real = do_atof(scan->lexem);
            break;
        case do_lx_end:
        case do_lx_error:
            break;
    }
}
static int do_rpc_cmd_parser(do_rpc_user_t *client, const char *cmd, do_rpc_command_info_t *command, do_data_t *out)
{
    command->id  = -1;
    command->param_list = do_list_new(1);
    command->external = 0;
    command->cmd = NULL;
    command->helponly = 0;

    int  i;
    do_scanner_t scan;
    do_scanner_init(&scan, (char*)cmd, client->conv);

    if (do_scanner_step(&scan) == do_lx_question) {
        command->helponly = 1;
        command->cmd = (do_rpc_command_t*)&commands[CMD_HELP];
        return 1;
    }
    if (scan.lex != do_lx_ident) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return 0;
    }
    for (i = 0; i < CMD_UNDEFINE; i++)
        if (!strcmp(commands[i].name, scan.lexem)) {
            command->id = i;
            command->cmd = (do_rpc_command_t*)&commands[i];
            break;
        }
    command->external = command->id == -1;
    if (command->external)
        for (i = 0; i < client->ctrl->extended.command_count; i++)
            if (!strcmp(client->ctrl->extended.commands[i].name, scan.lexem)) {
                command->id = i;
                command->cmd = &client->ctrl->extended.commands[i];
                break;
            }
    if (command->id == -1) {
        do_data_set_errf(out, "Syntax error. Undefine command %s Use ? for help", scan.lexem);
        return 0;
    }
    do_scanner_t scan1;
    do_scanner_copy(&scan1, &scan);
    while (do_scanner_step(&scan) != do_lx_end) {
        if (scan.lex == do_lx_question) { // help
            command->helponly = 1;
            return 1;
        }
        if (scan.lex == do_lx_ident) {
            rpc_param_add_from_scan(command->param_list, &scan);

            do_scanner_copy(&scan1, &scan);
            do_scanner_step(&scan);
            if (scan.lex != do_lx_equal && scan.lex != do_lx_colon) {
                do_scanner_copy(&scan, &scan1);
                continue;
            }
            do_scanner_copy(&scan1, &scan);
            do_scanner_step(&scan);
            if (scan1.lex == do_lx_equal) {
                if (scan.lex != do_lx_ident && scan.lex != do_lx_cinteger && scan.lex != do_lx_creal && scan.lex !=do_lx_cstring) {
                    do_data_set_err(out, "Systax error. Invalid param. Use doc for help");
                    return 0;
                }
            }
            else {
                if (scan.lex != do_lx_ident && scan.lex != do_lx_cinteger) {
                    do_data_set_err(out, "Systax error. Invalid param. Use doc for help");
                    return 0;
                }
            }
            continue;
        }
        if (scan.lex == do_lx_minus) {
            do_scanner_step(&scan);
                if (scan.lex != do_lx_ident) {
                do_data_set_err(out, "Systax error. Invalid param. Use doc for help");
                return 0;
            }
            continue;
        }
        if (scan.lex != do_lx_cinteger && scan.lex != do_lx_creal && scan.lex !=do_lx_cstring && scan.lex !=do_lx_cdate) {
            do_data_set_err(out, "Systax error. Invalid param. Use doc for help");
            return 0;
        }
        rpc_param_add_from_scan(command->param_list, &scan);
    }
    return 1;
}

static void do_rpc_cmd_get_help(do_rpc_t *ctrl, do_rpc_command_info_t* cmd, do_data_t *out
#ifdef PSEUDO_TELNET
                 , int color
#endif
)
{
    if ( cmd != NULL && cmd->id != -1 && (cmd->id != CMD_HELP || cmd->external))
        do_data_set_str(out, cmd->cmd->help);
    else {
#ifdef PSEUDO_TELNET
        if ( color )
            do_data_set_str(out, FONT_UNDER "USAGE COMMAND" FONT_DEFAULT "" "\n"
                               "\t" FONT_BOLD " <command> " FONT_BOLD "[" FONT_BOLD "param" FONT_DEFAULT "[...]]" FONT_BOLD "<LF>" FONT_DEFAULT"[" FONT_BOLD "<data>" FONT_DEFAULT"]" "\n"
                               "\t\t" FONT_BOLD "command" FONT_DEFAULT " - one of commands represented in section COMMANDS." "\n"
                               "\t\t" FONT_BOLD "param  " FONT_DEFAULT " - send additional param before LF." "\n"
                               "\t\t" FONT_BOLD "data   " FONT_DEFAULT " - send additional data after LF.(in telnet mode not support). See more "FONT_BOLD"doc"FONT_DEFAULT"." "\n"
                               );
        else
#endif
        do_data_set_str(out, "USAGE COMMAND" "\n"
                           "\t<command> [param[...]]<LF>[<data>]" "\n"
                           "\t\tcommand - one of commands represented in section COMMANDS." "\n"
                           "\t\tparam   - send additional param before LF." "\n"
                           "\t\tdata    - send additional data after LF. See more doc." "\n"
                           );

#ifdef PSEUDO_TELNET
        if ( color )
            do_data_add_str(out, FONT_UNDER "COMMANDS" FONT_DEFAULT "\n");
        else
#endif
        do_data_add_str(out, "COMMANDS" "\n");

        int i;
        for (i = 0; i < CMD_UNDEFINE; i++)
#ifdef PSEUDO_TELNET
            if ( color )
                do_data_add_strf(out, "\t" FONT_BOLD "%s" FONT_DEFAULT " - %s" "\n", commands[i].name, commands[i].desc);
            else
#endif
            do_data_add_strf(out, "\t%s - %s" "\n", commands[i].name, commands[i].desc);
#ifdef PSEUDO_TELNET
        if ( color )
            do_data_add_strf(out, FONT_UNDER "Addition command %s :" FONT_DEFAULT "" "\n", ctrl->info.name);
        else
#endif
        do_data_add_strf(out, "Addition command %s :" "\n", ctrl->info.name);
        for (i = 0; i < ctrl->extended.command_count; i++)
#ifdef PSEUDO_TELNET
            if ( color )
                do_data_add_strf(out, "\t" FONT_BOLD "%s" FONT_DEFAULT " - %s" "\n",
                    ctrl->extended.commands[i].name,
                    ctrl->extended.commands[i].desc);
            else
#endif
            do_data_add_strf(out, "\t%s - %s" "\n", ctrl->extended.commands[i].name,
                    ctrl->extended.commands[i].desc);
    }
}
inline static void systax_error(do_data_t *out, int com)
{
    do_data_set_errf(out, "Syntax error. Use %s ? for help", commands[com].name);
}
#define param_type(a,i)  ((do_rpc_command_param_t*)a->list[i])->type
#define param_value(a,i) ((do_rpc_command_param_t*)a->list[i])->value
#define param_count(a)   a->count


static void sessions(do_rpc_t *ctrl, do_list_t *params, do_data_t *out, int clientId)
{
    int countonly = 0;
    int count = 0, id = -1, i;

    switch (param_count(params)) {
        case 0:
            break;
        case 1:
            if (param_type(params, 0) == do_lx_ident) {
                if (strcmp(param_value(params, 0).string, "count")) {
                    systax_error(out, CMD_SESSIONS);
                    return;
                }
                countonly = 1;
            }
            else
            if (param_type(params, 0) == do_lx_cinteger) {
                id = param_value(params, 0).integer;
                countonly = 1;
            }
            else {
                systax_error(out, CMD_SESSIONS);
                return;
            }
        default:
            systax_error(out, CMD_SESSIONS);
            return;
    }

    char *buf;
    if (id >= 0) {
        buf = do_rpc_get_client_info(ctrl, id);
        if (!buf)
            do_data_set_errf(out, "Session %d not active", id);
        else {
            do_data_set_str(out, buf);
             do_free(buf);
        }
        return;
    }
    do_data_set_str(out, "");
    for (i = 0; i < do_rpc_client_count(ctrl); i++)
        if ( (buf = do_rpc_get_client_info(ctrl, i)) != NULL) {
            do_data_add_strf(out, "%c%s" "\n", (i == clientId) ? '*' : ' ', buf);
            do_free(buf);
            count++;
        }
    if (countonly)
        do_data_set_strf(out , "%d", count);
    return;
}

void kill(do_rpc_t *ctrl, do_list_t *params, do_data_t *out, int handle)
{
    int kill9 = 0, nowait = 0, id = -1, i;

    if (param_count(params) < 1 || param_count(params) > 3 ||
        param_type(params, 0) != do_lx_cinteger) {
        systax_error(out, CMD_KILL_SESSION);
        return;
    }
    id = param_value(params, 0).integer;
    for (i = 1; i < param_count(params); i++) {
        if (param_type(params, i) != do_lx_ident) {
            systax_error(out, CMD_KILL_SESSION);
            return;
        }
        if (!strcmp(param_value(params, i).string, "kill"))
            kill9 = 1;
        else
        if (!strcmp(param_value(params, i).string, "nowait"))
            nowait = 1;
        else {
            systax_error(out, CMD_KILL_SESSION);
            return;
        }
    }
    do_rpc_client_kill(ctrl, handle, id, kill9);
    if (id == handle)
        return;

    if (!nowait) {
        int max_wait = MAX_STEP_WAIT;
        while ( max_wait && do_rpc_client_live(ctrl, id) )  {
            do_rpc_send_number(ctrl, handle, MAX_STEP_WAIT - max_wait + 1);
            max_wait--;
            DO_SEC_SLEEP(1);
        }
        if (max_wait < MAX_STEP_WAIT)
            do_rpc_send_msg(ctrl, handle, ".");
        if (do_rpc_client_live(ctrl, id))
            do_data_set_errf(out, "Couldn't stop session. Try \"kill %d kill\". Or try stop and start manually ", id);
    }
}
static void do_rpc_cmd_get_doc(do_rpc_t *ctrl, do_data_t *out)
{
   do_data_set_strf(out ,"\t\tInput grammar" "\n"
                    "\t\t-------------" "\n"
                    "<Intput string> -> <command><parameters><CR><LF>" "\n"
                    "<command> ->  identifier" "\n"
                    "<arguments> -> <argument><argument> | eps" "\n"
                    "<argument> -> <const> | identifier = <const> | identifier = identifier" "\n"
                    "<const> -> integer | string | float" "\n"
                    "A input string is a word specifies the command to be executed and sequence of argument to the invoked command." "\n"
                    "A input string terminates in line feed. Carriage return will be united with line feed." "\n"
                    "The maximum length of this string is %dbyte." "\n"
                    "For viewing of the list of all commands use ? or command help." "\n"
                    "The argument can be a constant or the optional identifier assignments." "\n"
                    "A command can require input of the additional data of a special format." "\n"
                    "A command can return a text or a data of a special format." "\n"
                    "Format of data." "\n"
                    "[HEADER][PREFIX][DATA] - See file <control/ctrl_exch.h> for details. " "\n", MAX_BUFFER_LEN);
}

static void do_rpc_cmd_get_version(do_rpc_t *ctrl, do_data_t *out)
{
    do_data_set_strf(out, "%s", ctrl->info.ver);
}

static void locale(do_data_t *out)
{
    do_data_set_str(out, get_locale_lang());
}

void chstate(do_rpc_t *ctrl, do_list_t *params, do_state_t newstate, do_data_t *out, int handle)
{
    int nowait = 0;

    if (param_count(params) > 2) {
        do_data_set_err(out, "Syntax error. Use <command> ? for help");
        return;
    }
    if (param_count(params) == 1) {
        if (param_type(params, 0) != do_lx_ident ||
            strcmp(param_value(params, 0).string, "nowait")) {
            do_data_set_err(out, "Syntax error. Use <command> ? for help");
            return;
        }
        nowait = 1;
    }

    int sessions = 0, i;
    do_state_t wait_state = DO_STATE_UNDEFINE, new_state = DO_STATE_UNDEFINE;
    switch (newstate) {
        case DO_STATE_STOPING:
            if (!nowait)
                do_rpc_send_msg(ctrl, handle, "The %s on %s is going down NOW!", ctrl->info.name, ctrl->host);
            wait_state = DO_STATE_STOP;
            new_state = DO_STATE_STOPING;
            break;
        case DO_STATE_RESTARTING:
            for (i = 0; i < do_rpc_client_count(ctrl); i++)
                if (do_rpc_client_live(ctrl, i))
                    sessions++;
            if (sessions > 1) {
                do_data_set_err(out, "Can't restart. Close rest active session. For more detail run \"sessions\".");
                return;
            }
            if (!nowait)
                do_rpc_send_msg(ctrl, handle, "The %s on %s is restart NOW!", ctrl->info.name, ctrl->host);
            wait_state = DO_STATE_RUN;
            new_state = DO_STATE_RESTARTING;
            break;
        case DO_STATE_PAUSING:
            if (do_proc_state_get_state(ctrl->info.proc_state) != DO_STATE_RUN) {
                do_data_set_errf(out, "The %s on %s is not run.", ctrl->info.name, ctrl->host);
                return;
            }
            if (!nowait)
                do_rpc_send_msg(ctrl, handle, "The %s on %s is going pause NOW!", ctrl->info.name, ctrl->host ); //!!
            wait_state = DO_STATE_PAUSE;
            new_state = DO_STATE_PAUSING;
            break;
        case DO_STATE_RESUMING:
            if (do_proc_state_get_state(ctrl->info.proc_state) != DO_STATE_PAUSE) {
                do_data_set_errf(out, "The %s on %s is not paused.", ctrl->info.name, ctrl->host);
                return;
            }
            if (!nowait)
                do_rpc_send_msg(ctrl, handle, "The %s on %s is resume NOW!", ctrl->info.name, ctrl->host);
            wait_state = DO_STATE_RUN;
            new_state = DO_STATE_RESUMING;
            break;
        default :
            do_data_set_err(out, "Undefine state");
    };
    if (new_state != DO_STATE_UNDEFINE)
        do_rpc_set_state(ctrl, newstate);
    if (!nowait) {
        while (wait_state != DO_STATE_UNDEFINE && wait_state != do_proc_state_get_state(ctrl->info.proc_state))
            DO_SEC_SLEEP(1);
    }
}

static void get_file(do_rpc_user_t *client, do_list_t* params, do_data_t *out)
{
    FILE *fp;

    if (param_count(params) != 1 ||
        param_type(params, 0) != do_lx_cstring) {
        do_data_set_errf(out, "Syntax error. Use %s ? for help", commands[CMD_GET_FILE].name);
        return;
    }

    if ((fp = fopen(param_value(params, 0).string, "rb")) == NULL)  {
        do_log(LOG_ERR, "Error opening file (%s): %s", param_value(params, 0).string, strerror(errno));
        do_data_set_errf(out, "Error opening file (%s): %s", param_value(params, 0).string, strerror(errno));
        return;
    }
    fseek(fp, 0L, SEEK_END);
    if (!do_data_alloc(out, ftell(fp))) {
        fclose(fp);
        do_data_set_errf(out, "Not enough memory");
        return;
    }
    fseek(fp, 0L, SEEK_SET);
    if (fread(do_data_get(out), do_data_size(out), 1, fp) != 1) {
        do_log(LOG_ERR, "Error read from file (%s): %s", param_value(params, 0).string, strerror(errno));
        do_data_set_errf(out, "Error read from file (%s): %s", param_value(params, 0).string, strerror(errno));
    }
    fclose(fp);
#ifdef ZLIB
    do_data_set_send_flags(out, DO_EXCH_FLAG_ZIP);
#endif
}
static void put_file(do_rpc_user_t *client, do_list_t *params, do_data_t *in, do_data_t *out)
{
    FILE *fp;
    int n;

    if (param_count(params) != 1 ||
        param_type(params, 0) != do_lx_cstring) {
        do_data_set_errf(out, "Syntax error. Use %s ? for help", commands[CMD_PUT_FILE].name);
        return;
    }

    if (!do_data_size(in)) {
        do_data_set_errf(out, "File \"%s\" is empty", param_value(params, 0).string);
        return;
    }
    mode_t oldmask;
    oldmask = umask(000);
    if ((fp = fopen(param_value(params, 0).string, "wb")) == NULL)  {
        do_log(LOG_ERR, "Error create file (%s): %s", param_value(params, 0).string, strerror(errno));
        do_data_set_errf(out, "Error create file (%s): %s", param_value(params, 0).string, strerror(errno));
        return;
    }
    if ( (n = fwrite(do_data_get(in), do_data_size(in), 1, fp)) != 1 ) {
        do_log(LOG_ERR, "Error write to file (%s): %s", param_value(params, 0).string, strerror(errno));
        do_data_set_errf(out, "Error write to  file (%s): %s", param_value(params, 0).string, strerror(errno));
    }
    umask(oldmask);
    fclose(fp);
}

#ifdef _WIN32
VOID ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwCheckPoint, DWORD dwWaitHint);
#endif
static void do_rpc_set_state(do_rpc_t *ctrl, do_state_t newstate)
{
    do_proc_state_set_state(ctrl->info.proc_state, newstate);
    do_log_debug(ZONE, "proccess new stat %s", do_proc_state_get_name(newstate));
#ifdef _WIN32
    DWORD State = 0;
    switch (newstate) {
        case DO_STATE_RUN:
            State = SERVICE_RUNNING;
            break;
        case DO_STATE_PAUSE:
            State = SERVICE_PAUSED;
            break;
        case DO_STATE_STOPING:
            State = SERVICE_STOP_PENDING;
            break;
        case DO_STATE_STARTING:
            State = SERVICE_START_PENDING;
            break;
        case DO_STATE_PAUSING:
            State = SERVICE_PAUSE_PENDING;
            break;
        case DO_STATE_RESUMING:
            State = SERVICE_CONTINUE_PENDING;
            break;
        case DO_STATE_RESTARTING:
            State = SERVICE_START_PENDING;
            break;
        case DO_STATE_STOP:
        case DO_STATE_UNDEFINE:
            break;
    }
    if (State)
        ReportSvcStatus(State, 0, 0, 0);
#endif
}
