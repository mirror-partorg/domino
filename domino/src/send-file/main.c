#include <stdio.h>
#include <stdlib.h>
#include "../config/config.h"
#include <dolib.h>
#ifdef _WIN32
#include <windows.h>
#else
#include "../misc/send_error.h"
#endif


#ifdef _WIN32
#define print_m(a,b) MessageBox(NULL, a, (b) ? "Error" : "Information", (b) ? MB_ICONERROR : MB_ICONINFORMATION)
#else
#define print_m(a,b) printf(a)
#endif
void print_usage()
{

    print_m("usage: send_file <host[:port]> <keyfile> <source_file> <dest_file>\n"
         "\n"
         "Copyright (C) 2008-2009 Youra Richesse\n"
         "send_file is free software and comes with ABSOLUTELY NO WARRANTY\n"
         "under the terms of the GNU General Public License\n"
         "\n"
         "Report bugs to send_file Support Center:\n"
         "  mir@glekar.ru\n", FALSE);
    exit(1);
}

int main(int argc, char **argv)
{
#ifndef _WIN32
    char buff[1024];
#endif
    if (argc != 5)
        print_usage();
    do_log_set_type("send_file", DO_LOG_TYPE_SYSLOG, "");
    do_log_set_stdout();
#ifdef _WIN32
    //install_event_message("send_file");
#endif
    do_data_t *send = do_data_new();
    do_data_t *out = do_data_new();

    char *host, *ch;
    int port;

    host = argv[1];
    ch = strchr (host, ':');
    if (ch) {
       *ch = '\0';
       ch++;
       port = atoi(ch);
    }
    else
       port = SYNC_DEFAULT_TCP_PORT;

    char *filename = (char*)do_malloc(strlen(argv[4]) + 3);
    sprintf(filename, "\"%s\"", argv[4]);

    if (!do_data_read_from_file(send, argv[3]))
        goto err;
#ifdef ZLIB
    do_data_set_send_flags(send, DO_EXCH_FLAG_ZIP);
#endif

    do_rpc_client_t *client = do_rpc_client_new(host, port, USER, PASSWORD, FALSE);

#ifdef GCRYPT
    if (!do_rpc_client_crypt_init_key(client, argv[2]))
        goto err;
#endif
    if (!do_rpc_client_connect(client))
        goto err;

    if (!do_rpc_client_send_command(client, "put_file", filename, send, out))
        goto err;
    char *error = do_data_error(out);
    if (error && error[0]) {
        do_log(LOG_ERR, error);
        goto err;
    }
    print_m("Ok", FALSE);
    return 0;
err:
#ifndef _WIN32
    sprintf(buff, "send file %s to %s:%d", filename, host, port);
    send_error_message_with_host_info(buff);
#endif
    print_m("Error see log", TRUE);
    return 1;
}
