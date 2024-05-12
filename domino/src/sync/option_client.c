#include "option_client.h"
#include "../config/config.h"
#include <dolib.h>
#include <getopt.h>
#include <time.h>

option_client_t *option_client_new()
{
    option_client_t *result = (option_client_t *) do_malloc ( sizeof(option_client_t));
    result->debug_level = DEFAULT_DEBUG_LEVEL;
    result->log_type = DO_LOG_TYPE_STDOUT;
    result->domino_config_file = NULL;
    result->log_file = DOMINO_CONFIG(SYNC_CLIENT_LOG_FILE);
    result->show_version = 0;
    result->port = SYNC_DEFAULT_TCP_PORT;
    result->host = NULL;
    result->pid_file = NULL;
    result->user = do_strdup(SYNC_USERNAME);
    result->password = do_strdup(SYNC_PASSWORD);
    result->objs = do_list_new(1);
    result->alias_name = NULL;
    result->key_file = NULL;
    result->clone = FALSE;

    return result;
}
void option_client_free(option_client_t *opt)
{
    int i;
    if (opt->domino_config_file)
        do_free(opt->domino_config_file);
    if (opt->host)
        do_free(opt->host);
    if (opt->log_file)
        do_free(opt->log_file);
    if (opt->user)
        do_free(opt->user);
    if (opt->password)
        do_free(opt->password);
    if (opt->alias_name)
        do_free(opt->alias_name);
    if (opt->key_file)
        do_free(opt->key_file);
    for (i = 0; i < opt->objs->count; i++)
        switch (((obj_arg_t*)opt->objs->list[i])->obj) {
            case OBJ_DOCUMENT:
            case OBJ_DOCUMENT_PERIOD:
            case OBJ_DOCUMENT_MODIFIED:
                do_list_free(((obj_arg_t*)opt->objs->list[i])->document.documents);
                break;
            default:
                break;
        }

    do_list_free(opt->objs);
    do_free(opt);
}
static char *const shortoptions = "d:u:p:a:k:?V";
enum {
   OPT_NOARG,
   OPT_HELP,
   OPT_USER,
   OPT_PASSWORD,
   OPT_VERSION,
   OPT_KEY,
   OPT_ALIAS,
   OPT_DEBUG,
   OPT_PIDFILE,
   OPT_CLONE,
};

static const struct option longoptions[] = {
   {"help",                no_argument,       NULL, OPT_HELP},
   {"version",             no_argument,       NULL, OPT_VERSION},
   {"debug",               required_argument, NULL, OPT_DEBUG},
   {"user",                required_argument, NULL, OPT_USER},
   {"password",            required_argument, NULL, OPT_PASSWORD},
   {"key",                 required_argument, NULL, OPT_KEY},
   {"alias",               required_argument, NULL, OPT_ALIAS},
   {"writepid",            required_argument, NULL, OPT_PIDFILE},
   {"clone",               no_argument,       NULL, OPT_CLONE},
   {0,                     no_argument,       NULL, OPT_NOARG}
};
void print_usage()
{
   printf("usage: sync_client [options] <host[:port]> [object ...]\n"
         "  Options are as follows:\n"
         "  -d, --debug <level>       set debug level (>0)\n"
         "  -u, --user  <name>        set username\n"
         "  --clone                   clone handbook's record\n"
         "  -p, --password <pas>      set password\n"
         "  -V, --version             display version info\n"
         "  -a, --alias               set alias name (default local)\n"
         "  -k, --key                 set key file\n"
         "  -?, --help                display this help\n"
         "  --writepid <pid_file>     write pid file\n"
         "  Object are as follows:\n"
         " product <code>                  - sync product\n"
         " products <code1> <code2>        - sync products\n"
         " parcels <sklad> <code1> <code2> - sync parcels\n"
         " stocks  <sklad>                 - sync stocks\n"
         " updated-stocks                  - sync updated stocks\n"
         " partner <region>.<code>         - sync partner\n"
         " partners [<region>]             - sync partners\n"
         " groups                          - sync groups\n"
         " operations                      - sync operations\n"
         " sklads                          - sync sklads\n"
         " otdels                          - sync otdels\n"
         " users                           - sync users\n"
         " handbooks                       - sync products, partners, groups, operations, sklads, otdels, users\n"
         " document  <dtype>.<sklad>.<number>[,<dtype>.<sklad>.<number>...] - sync documents\n"
         " document_period <dtype>.<sklad>[,<dtype>.<sklad>...] {all | <date_begin> <date_end>} - sync documents in period\n"
         " document_modified <dtype>.<sklad>[,<dtype>.<sklad>...] <date_begin> <date_end> - sync modified documents from protocol \n"
         " checks    <sklad> <date_begin> <date_end> - sync checks\n"
         " realization  <sklad> <date_begin> <date_end> - sync realization\n"
         " protocol  <date_begin> <date_end> - sync protocol\n"
         " check_balance_41  <sklad> <date_end> - check balance 41\n"
         " check_parcels  <sklad> - check parcels for exists\n"
         "\n"
         " make_realization <sklad> <date_begin> <date_end> - make realization\n"
         "\n"
         " DATE GRAMMAR \n"
         "  Date -> date | date+<int> | date-<int> \n"
         "  date -> DD/MM/YYYY | today | yesterday | tomorrow \n"
         "\n"
         "Copyright (C) 2008-2010 Youra Richesse\n"
         "sync-client is free software and comes with ABSOLUTELY NO WARRANTY\n"
         "under the terms of the GNU General Public License\n"
         "\n"
         "Report bugs to sync Support Center:\n"
         "  mir@glekar.ru\n");
}
int date_parse(const char *str, struct tm *tm)
{
    do_scanner_t scan;
    do_lex_t lex;
    int n, minus;
    time_t now;

    do_scanner_init(&scan, (char*)str, (iconv_t)-1);
    lex = do_scanner_step(&scan);

    switch (lex) {
        case do_lx_cdate:
            if (!do_atodate(scan.lexem, tm))
                return FALSE;
            break;
        case do_lx_ident:
            now = time(NULL);
            *tm = *localtime(&now);
            if (!strcmp(scan.lexem, "yesterday"))
                tm->tm_mday--;
            else
            if (!strcmp(scan.lexem, "today"));
            else
            if (!strcmp(scan.lexem, "tomorrow"))
                tm->tm_mday++;
            else
                return FALSE;
            if (mktime(tm) == -1)
                return FALSE;
            break;
        default:
            return FALSE;
    }
    lex = do_scanner_step(&scan);
    switch (lex) {
        case do_lx_minus:
            minus = 1;
            break;
        case do_lx_plus:
            minus = 0;
            break;
        case do_lx_cinteger:
            minus = 0;
            n = atoi(scan.lexem);
            break;
        case do_lx_end:
            return TRUE;
        default:
            return FALSE;
    }
    if (lex != do_lx_cinteger) {
        lex = do_scanner_step(&scan);
        if (lex != do_lx_cinteger)
            return FALSE;
        n = atoi(scan.lexem);
    }
    tm->tm_mday += (minus) ? -n : n;
    if (mktime(tm) == -1)
        return FALSE;

    return TRUE;
}

int option_client_parse_options(option_client_t *opt, int argc, char *argv[])
{
    int options = 0;
    int oneshot = 0;
    int option_index;
    int errflag = 0;
    int c, i;
    int argind;
    char *value;
    char *arg, *ch;
    obj_arg_t *obj;
    do_string_list_t *strs;
    option_document_t key;
    struct tm tm;


    while (!errflag &&
          (c = getopt_long(argc, argv, shortoptions,
            longoptions, &option_index)) != -1) {
      options++;
      switch (c) {
      case 'd':
      case OPT_DEBUG:
         if (optarg[0] == '-') {
            errflag++;
            break;
         }
         options--;
         opt->debug_level = atoi(optarg);
         break;
      case 'p':
      case OPT_PASSWORD:
         if (optarg[0] == '-') {
            errflag++;
            break;
         }
         options--;
         do_free(opt->password);
         opt->password = do_strdup(optarg);
         break;
      case OPT_CLONE:
         opt->clone = TRUE;
         break;

      case OPT_PIDFILE:
         if (optarg[0] == '-') {
            errflag++;
            break;
         }
         options--;
         if (opt->pid_file) do_free(opt->pid_file);
         opt->pid_file = do_strdup(optarg);
         break;
      case 'u':
      case OPT_USER:
         if (optarg[0] == '-') {
            errflag++;
            break;
         }
         options--;
         do_free(opt->user);
         opt->user = do_strdup(optarg);
         break;
      case 'a':
      case OPT_ALIAS:
         if (optarg[0] == '-') {
            errflag++;
            break;
         }
         options--;
         if (opt->alias_name)
            do_free(opt->alias_name);
         opt->alias_name = do_strdup(optarg);
         break;
      case 'k':
      case OPT_KEY:
         if (optarg[0] == '-') {
            errflag++;
            break;
         }
         options--;
         if (opt->key_file)
            do_free(opt->key_file);
         opt->key_file = do_strdup(optarg);
         break;
      case 'V':
      case OPT_VERSION:
         opt->show_version = 1;
         oneshot = 1;
         break;
      case '?':
      case OPT_HELP:
      default:
         errflag++;
      }
    }
    if ((oneshot == 1) && options > 1) {
      do_log(LOG_ERR, "Error: too many arguments.");
      errflag++;
    }
    if (!oneshot && !errflag) {
        argind = optind;
        if (argind < argc) {
            arg = do_strdup(argv[argind]);
            ch = strchr (arg, ':');
            if (ch) {
                *ch = '\0';
                opt->host = do_strdup(arg);
                ch++;
                opt->port = atoi(ch);
                do_free(arg);
            }
            else
                opt->host = arg;
            argind++;
            while (argind < argc && !errflag) {
                obj = (obj_arg_t*)do_malloc0(sizeof(obj_arg_t));
                arg = argv[argind];
                argind++;
                if (!strcmp(arg, "product")) {
                    obj->obj = OBJ_PRODUCT;
                    if (argind == argc)
                        errflag++;
                    else {

                        strcpy(obj->product.code, argv[argind]);
                        argind++;
                    }
                }
                else
                if (!strcmp(arg, "products")) {
                    obj->obj = OBJ_PRODUCTS;
                    if (argind == argc)
                        errflag++;
                    else {
                        strcpy(obj->products.code1, argv[argind]);
                        argind++;
                    }
                    if (argind == argc)
                        errflag++;
                    else {
                        strcpy(obj->products.code2, argv[argind]);
                        argind++;
                    }
                }
                else
                if (!strcmp(arg, "partner")) {
                    obj->obj = OBJ_PARTNER;
                    if (argind == argc)
                        errflag++;
                    else {
                        value = do_strdup(argv[argind]);
                        ch = strchr(value, '.');
                        if (!ch)
                            errflag++;
                        else {
                            *ch = '\0';
                            strcpy(obj->partner.g_code, value);
                            obj->partner.code = atoi(++ch);
                        }
                        argind++;
                        do_free(value);
                    }
                }
                else
                if (!strcmp(arg, "stocks")) {
                    obj->obj = OBJ_STOCKS1;
                    if (argind == argc)
                        errflag++;
                    else {
                        strcpy(obj->stocks.sklad, argv[argind]);
                        argind++;
                    }
                }
                else
                if (!strcmp(arg, "updated-stocks")) {
                    obj->obj = OBJ_UPDATED_STOCKS;
                }
                else
                if (!strcmp(arg, "partners")) {
                    obj->obj = OBJ_PARTNERS;
                    if (argind == argc)
                        obj->partner.g_code[0] = '\0';
                    else {
                        strcpy(obj->partner.g_code, argv[argind]);
                        argind++;
                    }
                }
                else
                if (!strcmp(arg, "groups")) {
                    obj->obj = OBJ_GROUPS;
                }
                else
                if (!strcmp(arg, "parcels")) {
                    obj->obj = OBJ_PARCELS;
                    if (argind == argc)
                        errflag++;
                    else {
                        strcpy(obj->parcels.sklad, argv[argind]);
                        argind++;
                    }
                    if (argind == argc)
                        errflag++;
                    else {
                        strcpy(obj->parcels.code1, argv[argind]);
                        argind++;
                    }
                    if (argind == argc)
                        errflag++;
                    else {
                        strcpy(obj->parcels.code2, argv[argind]);
                        argind++;
                    }

                }
                else
                if (!strcmp(arg, "check_parcels")) {
                    obj->obj = OBJ_CHECK_PARCELS;
                    if (argind == argc)
                        errflag++;
                    else {
                        strcpy(obj->parcels.sklad, argv[argind]);
                        argind++;
                    }
                }
                else
                if (!strcmp(arg, "operations")) {
                    obj->obj = OBJ_OPERATIONS;
                }
                else
                if (!strcmp(arg, "sklads")) {
                    obj->obj = OBJ_SKLADS;
                }
                else
                if (!strcmp(arg, "otdels")) {
                    obj->obj = OBJ_OTDELS;
                }
                else
                if (!strcmp(arg, "users")) {
                    obj->obj = OBJ_USERS;
                }
                else
                if (!strcmp(arg, "handbooks")) {
                    obj->obj = OBJ_GROUPS;
                    do_list_add(opt->objs, obj);
                    obj = (obj_arg_t*)do_malloc0(sizeof(obj_arg_t));
                    obj->obj = OBJ_OPERATIONS;
                    do_list_add(opt->objs, obj);
                    obj = (obj_arg_t*)do_malloc0(sizeof(obj_arg_t));
                    obj->obj = OBJ_SKLADS;
                    do_list_add(opt->objs, obj);
                    obj = (obj_arg_t*)do_malloc0(sizeof(obj_arg_t));
                    obj->obj = OBJ_OTDELS;
                    do_list_add(opt->objs, obj);
                    obj = (obj_arg_t*)do_malloc0(sizeof(obj_arg_t));
                    obj->obj = OBJ_USERS;
                    do_list_add(opt->objs, obj);
                    obj = (obj_arg_t*)do_malloc0(sizeof(obj_arg_t));
                    obj->obj = OBJ_PARTNERS;
                    do_list_add(opt->objs, obj);
                    obj = (obj_arg_t*)do_malloc0(sizeof(obj_arg_t));
                    obj->obj = OBJ_PRODUCTS;
                    //do_list_add(opt->objs, obj);
                }
                else
                if (!strcmp(arg, "document")) {
                    obj->obj = OBJ_DOCUMENT;
                    obj->document.documents = do_list_new(TRUE);
                    if (argind == argc) {
                        errflag++;
                        break;
                    }
                    strs = do_string_list_new_from_csv(argv[argind]);
                    argind++;
                    if (!strs->count) {
                        errflag++;
                        break;
                    }
                    for (i = 0; i < strs->count; i++) {
                        value = strs->list[i];
                        ch = strchr(value, '.');
                        if (!ch) {
                            errflag++;
                            break;
                        }
                        *ch = '\0';
                        strcpy(key.dtype, value);
                        value = ++ch;
                        ch = strchr(value, '.');
                        if (!ch) {
                            errflag++;
                            break;
                        }
                        *ch = '\0';
                        strcpy(key.sklad, value);
                        value = ++ch;
                        strcpy(key.document, value);
                        do_list_add_alloc(obj->document.documents, &key, sizeof(key));
                    }
                    do_string_list_free(strs);
                }
                else
                if (!strcmp(arg, "document_period")) {
                    obj->obj = OBJ_DOCUMENT_PERIOD;
                    obj->document.documents = do_list_new(TRUE);
                    if (argind == argc) {
                        errflag++;
                        break;
                    }
                    strs = do_string_list_new_from_csv(argv[argind]);
                    if (!strs->count) {
                         errflag++;
                         break;
                    }
                    for (i = 0; i < strs->count; i++) {
                        value = strs->list[i];
                        ch = strchr(value, '.');
                        if (!ch) {
                            errflag++;
                            break;
                        }
                        *ch = '\0';
                        strcpy(key.dtype, value);
                        value = ch+1;
                        strcpy(key.sklad, value);
                        do_list_add_alloc(obj->document.documents, &key, sizeof(key.dtype) + sizeof(key.sklad));
                    }
                    if (errflag)
                        break;

                    argind++;
                    if (argind == argc) {
                        errflag++;
                        break;
                    }
                    obj->document.all = FALSE;
                    if (!strcmp(argv[argind], "all")) {
                        obj->document.all = TRUE;
                        argind++;
                    }
                    else {
                        if (!date_parse(argv[argind], &tm)) {
                            errflag++;
                            break;
                        }
                        obj->document.date_beg = tm;

                        argind++;
                        if (argind == argc) {
                             obj->document.date_end = obj->document.date_beg;
                        }
                        else {
                            if (!date_parse(argv[argind], &tm)) {
                                errflag++;
                                break;
                            }
                            obj->document.date_end = tm;
                        }
                        argind++;
                    }
                }
                else
                if (!strcmp(arg, "document_modified")) {
                    obj->obj = OBJ_DOCUMENT_MODIFIED;
                    obj->document.documents = do_list_new(TRUE);
                    if (argind == argc) {
                        errflag++;
                        break;
                    }
                    strs = do_string_list_new_from_csv(argv[argind]);
                    if (!strs->count) {
                         errflag++;
                         break;
                    }
                    for (i = 0; i < strs->count; i++) {
                        value = strs->list[i];
                        ch = strchr(value, '.');
                        if (!ch) {
                            errflag++;
                            break;
                        }
                        *ch = '\0';
                        strcpy(key.dtype, value);
                        value = ch+1;
                        strcpy(key.sklad, value);
                        do_list_add_alloc(obj->document.documents, &key, sizeof(key.dtype) + sizeof(key.sklad));
                    }
                    if (errflag)
                        break;

                    argind++;
                    if (argind == argc) {
                        errflag++;
                        break;
                    }
                    if (!date_parse(argv[argind], &tm)) {
                        errflag++;
                        break;
                    }
                    obj->document.date_beg = tm;

                    argind++;
                    if (argind == argc) {
                         errflag++;
                         break;
                    }
                    if (!date_parse(argv[argind], &tm)) {
                        errflag++;
                        break;
                    }
                    obj->document.date_end = tm;
                    argind++;
                }
                else
#ifndef DOMINO78
                if (!strcmp(arg, "checks") ||
                    !strcmp(arg, "realization") ||
                    !strcmp(arg, "make_realization") ) {

                    if ( !strcmp(arg, "checks") )
                        obj->obj = OBJ_CHECKS;
                    else
                    if ( !strcmp(arg, "realization") )
                        obj->obj = OBJ_REALIZATION;
                    else
                    if ( !strcmp(arg, "make_realization") )
                        obj->obj = OBJ_MAKE_REALIZATION;

                    if (argind == argc) {
                        errflag++;
                        break;
                    }

                    strcpy(obj->sklad_period.sklad, argv[argind]);
                    argind++;
                    if (argind == argc) {
                        errflag++;
                        break;
                    }

                    struct tm tm;
                    if (!date_parse(argv[argind], &tm))
                        errflag++;

                    obj->sklad_period.date_beg = tm;

                    argind++;
                    if (argind == argc) {
                        errflag++;
                        break;
                    }

                    if (!date_parse(argv[argind], &tm)) {
                        errflag++;
                        break;
                    }
                    obj->sklad_period.date_end = tm;
                    argind++;
                }
                else
#endif
                if (!strcmp(arg, "protocol")) {
                    obj->obj = OBJ_PROTOCOL;
                    if (argind == argc) {
                        errflag++;
                        break;
                    }

                    struct tm tm;
                    if (!date_parse(argv[argind], &tm))
                        errflag++;

                    obj->protocol.date_beg = tm;

                    argind++;
                    if (argind == argc) {
                        errflag++;
                        break;
                    }

                    if (!date_parse(argv[argind], &tm)) {
                        errflag++;
                        break;
                    }
                    obj->protocol.date_end = tm;
                    argind++;
                }
                else
                if (!strcmp(arg, "check_balance_41")) {
                    obj->obj = OBJ_CHECK_BALANCE_41;
                    if (argind == argc) {
                        errflag++;
                        break;
                    }
                    strcpy(obj->check_balance_41.sklad, argv[argind]);
                    argind++;
                    if (argind == argc) {
                        errflag++;
                        break;
                    }
                    struct tm tm;
                    if (!date_parse(argv[argind], &tm))
                        errflag++;

                    obj->check_balance_41.date_end = tm;

                    argind++;
                }
                else
                    errflag++;

                if (!errflag)
                    do_list_add(opt->objs, obj);
                else
                    do_free(obj);
            }
        }
    }
    if (!oneshot && (!opt->host || !opt->objs->count))
        errflag++;

    if (errflag)
        print_usage(argv);
    return errflag;
}

