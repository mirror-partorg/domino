#include "send_error.h"

#ifdef USE_IM

#include <loudmouth/loudmouth.h>
#include <errno.h>
#include <dolib.h>
#include "send_error.h"
#include "../config/config.h"
#ifdef _WIN32
#include <windows.h>
#endif
#ifdef _LINUX
#include <unistd.h>
#endif

typedef struct {
    char *user;
    char *password;
    char *domain;
    char *host;
    char *sendto;
    int  port;
} send_error_t;

static int disable_send = FALSE;
static char *sendto_ = NULL;

#ifdef GLOG_OFF
static void  GLogFuncOff(const gchar *log_domain, GLogLevelFlags log_level,
                                                         const gchar *message,
                                                         gpointer user_data)
{
}
#endif

int send_error_message_with_host_info(const char *msg)
{
    int retval;
    const char *pref = "Error from";
    char hostname[1024], *buf;
    if (gethostname(hostname, 1023))
        strcpy(hostname, "unknow");
    buf = (char*)do_malloc(strlen(pref)+strlen(hostname)+strlen(msg)+10);
    sprintf(buf, "%s %s: %s", pref, hostname, msg);
    retval = send_error_message(buf);
    do_free(buf);
    return retval;
}
int send_error_message_with_host_info_and_prog(const char *prog, const char *msg)
{
    int retval;
    const char *pref = "Error from";
    char hostname[1024], *buf;
    if (gethostname(hostname, 1023))
        strcpy(hostname, "unknow");
    buf = (char*)do_malloc(strlen(pref)+strlen(hostname)+strlen(prog)+strlen(msg)+10);
    sprintf(buf, "%s %s %s: %s", pref, hostname, prog, msg);
    retval = send_error_message(buf);
    do_free(buf);
    return retval;
}

int send_info_message_with_host_info(const char *msg)
{
    int retval;
    const char *pref = "Info from";
    char hostname[1024], *buf;

    if (gethostname(hostname, 1023))
        strcpy(hostname, "unknow");
    buf = (char*)do_malloc(strlen(pref)+strlen(hostname)+strlen(msg)+10);
    sprintf(buf, "%s %s: %s", pref, hostname, msg);
    retval = send_error_message(buf);
    do_free(buf);
    return retval;
}

static void send_error_free(send_error_t *send_error)
{
    if (send_error->user)
        do_free(send_error->user);
    if (send_error->password)
        do_free(send_error->password);
    if (send_error->domain)
        do_free(send_error->domain);
    if (send_error->sendto)
        do_free(send_error->sendto);
    if (send_error->host)
        do_free(send_error->host);
    do_free(send_error);
}

int send_error_set_sendto(const char *value)
{
   if ( sendto_ )
      do_free(sendto_);
   sendto_ = do_strdup(value);
   return TRUE;
}

static send_error_t *send_error_new(const char *filename)
{
    send_error_t *send_error = do_malloc0(sizeof(send_error_t));
    FILE *fp;
#ifdef GLOG_OFF
    g_log_set_default_handler(GLogFuncOff, NULL);
#endif
    int defaultfname = 0;
    char *fname = (char*) filename;
    if (!fname) {
        defaultfname = 1;
        fname = DOMINO_CONFIG(SEND_ERROR_CONFIG_FILE);
    }
    if ((fp = fopen(fname, "r")) == NULL)  {
        do_log(LOG_ERR, "Error opening send_error configuration file (%s): %s", fname, strerror(errno));
        if (defaultfname)
            do_free(fname);
        do_free (send_error);
        return NULL;
    }

    size_t len = 1024;
    char* line = (char *) do_malloc (len + 1);
    int name_len;
    while (!feof(fp))  {
        char* cp, *name, *value, *tail;
        if (fgets(line, len + 1, fp) == NULL)
            break;
        while ((strlen(line) == len) && !feof(fp)) {
            line = do_realloc(line, len * 2 + 1);
            if (!fgets(line + len, len + 1, fp)) {
        	do_free(line);
	        do_log(LOG_ERR, "Error reading send_error configuration file (%s): %s", fname, strerror(errno));
        	return NULL;
            }
            len *= 2;
        }
        cp = line;
        while (cp[0] == ' ') ++cp; // delspace
        /* A leading '#' or ';' signals a comment line.  */
        if (cp[0] == '#' || cp[0] == ';' || cp[0] == '\0' || cp[0] == '\n' || cp[0] == '\r')
            continue;
        name = cp;
        while (cp[0] != '\0' && cp[0] != '\n' && cp[0] != ' ') ++cp;
        if (name == cp)
            continue;

        name_len = cp - name;

        while (cp[0] == ' ') ++cp; // delspace
        value = cp;
        while (cp[0] != '\0' && cp[0] != '\n') ++cp;
        tail = cp - 1;
        while (tail >= value && (tail[0] == ' ' || tail[0] == '\r')) tail --;
        if (value > tail)
            continue;

        value[tail - value + 1] = '\0';
        //!!p_value = strndup(value, tail - value + 1);
        name[name_len] = '\0';

        if (!strcmp(name, "user")) {
            if (send_error->user)
                do_free(send_error->user);
            send_error->user = do_strdup(value);
        }
        else
        if (!strcmp(name, "password")) {
            if (send_error->password)
                do_free(send_error->password);
            send_error->password = do_strdup(value);
        }
        else
        if (!strcmp(name, "domain")) {
            if (send_error->domain)
                do_free(send_error->domain);
            send_error->domain = do_strdup(value);
        }
        else
        if (!strcmp(name, "host")) {
            if (send_error->host)
                do_free(send_error->host);
            send_error->host = do_strdup(value);
        }
        else
        if (!strcmp(name, "port")) {
            send_error->port = atoi(value);
        }
        else
        if (!strcmp(name, "error-send-to")) {
            if (send_error->sendto)
                do_free(send_error->sendto);
            send_error->sendto = do_strdup(value);
        }
    }
    if ( sendto_ ) {
        if (send_error->sendto)
            do_free(send_error->sendto);
        send_error->sendto = do_strdup(sendto_);
    }
    do_free(line);
    fclose(fp);
    if (defaultfname)
        do_free(fname);
    if (!send_error->user || !send_error->domain || !send_error->password || !send_error->sendto) {
        do_log(LOG_ERR, "send_error config failed");
        send_error_free(send_error);
        return NULL;
    }
    if (!send_error->port)
        send_error->port = XMPP_PORT;
    return send_error;
}
void send_error_message_set_disable_send()
{
    disable_send = 1;
}
void send_error_message_set_enable_send()
{
    disable_send = 0;
}
int send_error_message(const char *message)
{
    if ( disable_send )
        return 1;
    send_error_t *send_error;
    do_string_list_t *sendlist;
    int err = 0;
    int i;


    send_error = send_error_new(NULL);
    if (!send_error)
        return 0;

    LmConnection *conn;
    GError       *error = NULL;
    LmMessage    *m;

    conn = lm_connection_new (send_error->domain);
    if ( !conn ) {
        do_log(LOG_ERR, "XMPP Couldn't create connection to '%s'",
                         send_error->domain);
        send_error_free(send_error);
        return 0;
    }
    if (!lm_connection_open_and_block (conn, &error)) {
        do_log(LOG_ERR, "XMPP Couldn't open connection to '%s': %s",
                         send_error->domain, error ? error->message : "");
        send_error_free(send_error);
        return 0;
    }

    if (!lm_connection_authenticate_and_block (conn, send_error->user, send_error->password,
                                                   "domino", &error)) {
        do_log(LOG_ERR, "XMPP Couldn't authenticaten %s",
                         error ? error->message : "");
        send_error_free(send_error);
        return 0;
    }

    sendlist = do_string_list_new_from_csv(send_error->sendto);
    for (i = 0; i < sendlist->count; i++) {
        m = lm_message_new (sendlist->list[i], LM_MESSAGE_TYPE_MESSAGE);
        lm_message_node_add_child (m->node, "body", message);

        if (!lm_connection_send (conn, m, &error)) {
            do_log(LOG_ERR, "Error while sending message to '%s': %s",
                         sendlist->list[i], error ? error->message:"");
            err = 1;
            break;
        }

        lm_message_unref (m);
    }
    lm_connection_close (conn, NULL);
    lm_connection_unref (conn);

    do_string_list_free(sendlist);
    send_error_free(send_error);
    return !err;
}
int send_error_message2(const char *message,const char *filename)
{
    if ( disable_send )
        return 1;
    send_error_t *send_error;
    do_string_list_t *sendlist;
    int err = 0;
    int i;


    send_error = send_error_new(filename);
    if (!send_error)
        return 0;

    LmConnection *conn;
    GError       *error = NULL;
    LmMessage    *m;

    conn = lm_connection_new (send_error->domain);
    if ( !conn ) {
        do_log(LOG_ERR, "XMPP Couldn't create connection to '%s'",
                         send_error->domain);
        send_error_free(send_error);
        return 0;
    }
    if (!lm_connection_open_and_block (conn, &error)) {
        do_log(LOG_ERR, "XMPP Couldn't open connection to '%s': %s",
                         send_error->domain, error ? error->message : "");
        send_error_free(send_error);
        return 0;
    }

    if (!lm_connection_authenticate_and_block (conn, send_error->user, send_error->password,
                                                   "domino", &error)) {
        do_log(LOG_ERR, "XMPP Couldn't authenticaten %s",
                         error ? error->message : "");
        send_error_free(send_error);
        return 0;
    }

    sendlist = do_string_list_new_from_csv(send_error->sendto);
    for (i = 0; i < sendlist->count; i++) {
        m = lm_message_new (sendlist->list[i], LM_MESSAGE_TYPE_MESSAGE);
        lm_message_node_add_child (m->node, "body", message);

        if (!lm_connection_send (conn, m, &error)) {
            do_log(LOG_ERR, "Error while sending message to '%s': %s",
                         sendlist->list[i], error ? error->message:"");
            err = 1;
            break;
        }

        lm_message_unref (m);
    }
    lm_connection_close (conn, NULL);
    lm_connection_unref (conn);

    do_string_list_free(sendlist);
    send_error_free(send_error);
    return !err;
}
#define MAX_LOG_LINE 1023
int send_error_message_width_host_info_printf(const char *msgfmt, ...)
{
    va_list ap;
    int len;
    char message[MAX_LOG_LINE+1];
    va_start(ap, msgfmt);
    len = vsnprintf(message, MAX_LOG_LINE, msgfmt, ap);
    if (len > MAX_LOG_LINE)
        message[MAX_LOG_LINE] = '\0';
    else
        message[len] = '\0';
    va_end(ap);
    return send_error_message_with_host_info(message);
}
/*
int send_error_message_width_host_info_and_program_name_printf(const char *program, const char *msgfmt, ...)
{
    va_list ap;
    int len;
    char message[MAX_LOG_LINE+1];
    va_start(ap, msgfmt);
    len = vsnprintf(message, MAX_LOG_LINE, msgfmt, ap);
    if (len > MAX_LOG_LINE)
        message[MAX_LOG_LINE] = '\0';
    else
        message[len] = '\0';
    va_end(ap);
    return send_error_message_with_host_info_and_prog(program, message);

}
*/
int send_info_message_width_host_info_printf(const char *msgfmt, ...)
{
    va_list ap;
    int len;
    char message[MAX_LOG_LINE+1];
    va_start(ap, msgfmt);
    len = vsnprintf(message, MAX_LOG_LINE, msgfmt, ap);
    if (len > MAX_LOG_LINE)
        message[MAX_LOG_LINE] = '\0';
    else
        message[len] = '\0';
    va_end(ap);
    return send_info_message_with_host_info(message);
}

int send_im_message(const char *user, const char *domain, const char *password, const char *recipients, const char *message)
{
    LmConnection *conn;
    GError       *error = NULL;
    LmMessage    *m;
    int err = FALSE;

    conn = lm_connection_new (domain);
    if ( !conn ) {
        do_log(LOG_ERR, "XMPP Couldn't create connection to '%s'",
                         domain);
        return FALSE;
    }
    if (!lm_connection_open_and_block (conn, &error)) {
        do_log(LOG_ERR, "XMPP Couldn't open connection to '%s': %s",
                         domain, error ? error->message : "");
        return FALSE;
    }

    if (!lm_connection_authenticate_and_block (conn, user, password,
                                                   "domino", &error)) {
        do_log(LOG_ERR, "XMPP Couldn't authenticaten %s",
                         error ? error->message : "");
        return FALSE;
    }

    do_string_list_t *sendlist = do_string_list_new_from_csv(recipients);
    int i;
    for (i = 0; i < sendlist->count; i++) {
        m = lm_message_new (sendlist->list[i], LM_MESSAGE_TYPE_MESSAGE);
        lm_message_node_add_child (m->node, "body", message);

        if (!lm_connection_send (conn, m, &error)) {
            do_log(LOG_ERR, "Error while sending message to '%s': %s",
                         sendlist->list[i], error ? error->message:"");
            err = FALSE;
            break;
        }

        lm_message_unref (m);
    }
    lm_connection_close (conn, NULL);
    lm_connection_unref (conn);

    do_string_list_free(sendlist);
    return !err;
}
#endif
