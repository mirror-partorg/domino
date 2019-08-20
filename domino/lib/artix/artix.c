
#include "artix.h"
#include "../../misc/define.h"
#include "../../config/config.h"
#include "../../misc/iconv_.h"
#include <stdio.h>
#include <dolib.h>
#include <string.h>

static char *get_fb_connection_string(const char* host, const char *cs);

DO_EXPORT artix_t *artix_new(const char* host, const char *cs, int info_port,
		   const char *username, const char *password, int use_utf)
{
    artix_t *artix = (artix_t *)do_malloc0(sizeof(artix_t));
    if (info_port) {
        artix->info = artix_info_new(host, info_port, use_utf);
        if (!artix->info) {
            do_free(artix);
            return NULL;
        }
    }
    char *connection_string = get_fb_connection_string(host, cs);
#ifdef ARTIX_OLD
    artix->alias = artix_alias_new(connection_string, username, password, use_utf);
    do_free(connection_string);
    if (!artix->alias) {
        if (artix->info)
            artix_info_free(artix->info);
        do_free(artix);
        return NULL;
    }
#endif
    return artix;
}
DO_EXPORT artix_t *artix_new_default(const char* host, int use_utf)
{
    return artix_new(host, ARTIX_DEFAULT_CONNECTION_STRING,
    ARTIX_DEFAULT_INFO_SERVICE_PORT,
    ARTIX_DEFAULT_USER, ARTIX_DEFAULT_PASSWORD, use_utf);
}
DO_EXPORT void artix_free(artix_t *artix)
{
    if (artix) {
#ifdef ARTIX_OLD
        artix_alias_free(artix->alias);
#endif
        if (artix->info)
            artix_info_free(artix->info);
        do_free(artix);
    }
}
static char *get_fb_connection_string(const char* host, const char *db)
{
    if (!strcmp(host, "127.0.0.1") ||
        !strcmp(host, "localhost"))
        return do_strdup(db);
    else {
        char* connection_string = (char*) do_malloc(strlen(host) + 1 + strlen(db) + 1);
        sprintf(connection_string, "%s:%s", host, db);
        return connection_string;
    }
}

