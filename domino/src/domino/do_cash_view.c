#ifdef CASH_VIEW
#include "do_cash_view.h"
#include "do_view.h"
#include "do_utilx.h"
#include "do_message.h"
#include "domino.h"
#include <artix.h>
#include <domino.h>
#include "hig.h"
#include "../misc/send_error.h"
#include "do_tree_view.h"
#include "do_icons.h"
#include <gdk/gdk.h>
#include <time.h>
#include <errno.h>
#include <dolib.h>

#define DO_CASH_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_CASH_VIEW, DoCashViewPrivate))

#define OBJECT_ROOT_PATH "CashView"
#define DEFAULT_COL_PACK_FORM_WIDTH 400
#define DEFAULT_COL_DEFAULT_WIDTH 60

// View interface
static void do_cash_view_view_init(DoViewIface *iface);

static gboolean    do_cash_view_view_do_grab_focus(DoView *view);
static void	       do_cash_view_view_do_close(DoView *view);
static const char *do_cash_view_view_get_title(DoView *view);
static GdkPixbuf  *do_cash_view_view_get_icon(DoView *view);
static void		   do_cash_view_view_do_edit(DoView *view, const gchar *tab);
static gboolean	   do_cash_view_view_can_do_edit(DoView *view, const gchar *tab);
static void		   do_cash_view_view_do_popup(DoView *view, GdkEventButton* event);


enum
{
	PROP_0,
	PROP_ALIAS,
};

enum
{
    COLUMN_CASH_CODE = 0,
    COLUMN_CASH_NAME,
    COLUMN_CASH_HOST,
    COLUMN_UNIT,
    COLUMN_INFO,
    COLUMN_CASH_VERSION,
    COLUMN_CASH_LICENSE_EXP,
    COLUMN_VERSION,
    COLUMN_EXCHANGE_STATUS,
    COLUMN_EXCHANGE_MESSAGE,
    COLUMN_DCLUB_STATUS,
    COLUMN_DCLUB_MESSAGE,
    COLUMN_CASH_INFO,
    COLUMN_CASH_TERMINAL,
    COLUMN_THREAD,
    NO_COLUMNS,
};

struct _DoCashViewPrivate
{
    GtkListStore        *model;
    GtkTreeView         *tree_view;
    GtkTreeViewColumn   *columns[NO_COLUMNS];
    GtkTreeSelection    *selection;
    GdkPixbuf           *icon;
    gchar               *alias_name;
    gboolean             finalized;
};

G_DEFINE_TYPE_WITH_CODE (DoCashView, do_cash_view, DO_TYPE_TREE_VIEW,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_cash_view_view_init)
                                    );

static void do_cash_view_view_init(DoViewIface *iface)
{
    iface->do_grab_focus = do_cash_view_view_do_grab_focus;
    iface->do_close      = do_cash_view_view_do_close;
    iface->get_title     = do_cash_view_view_get_title;
    iface->get_icon      = do_cash_view_view_get_icon;
    iface->do_edit       = do_cash_view_view_do_edit;
    iface->can_do_edit   = do_cash_view_view_can_do_edit;
    iface->do_popup      = do_cash_view_view_do_popup;

}

static void do_cash_view_init(DoCashView *view)
{
	DoCashViewPrivate *priv = DO_CASH_VIEW_GET_PRIVATE (view);
    GtkIconTheme *icon_theme;
    icon_theme = gtk_icon_theme_get_default();
    priv->icon = gtk_icon_theme_load_icon(icon_theme, "gtk-info", DO_VIEW_ICON_SIZE, 0, NULL);
}

#define MAX_MESSAGE_LEN 1024
typedef struct {
    GtkTreeModel *model_;
    GtkTreePath  *path_;
    gint          cash_code;
    gchar         host[1024];
    gboolean     *finalized_;
    struct {
        gboolean c_exchange_good;
        gchar c_e_message[MAX_MESSAGE_LEN];
        gchar c_e_version[MAX_MESSAGE_LEN];
        gchar c_d_message[MAX_MESSAGE_LEN];
        gchar c_d_version[MAX_MESSAGE_LEN];
        gchar c_cash_info[MAX_MESSAGE_LEN];
        gchar c_cash_ver[MAX_MESSAGE_LEN];
        glong c_cash_license_exp;
        gint c_i;
        gint c_e_status;
        gint c_d_status;
    } state;
} DoCashViewInfo;



static gboolean  do_cash_view_find_by_code(GtkTreeModel *model,
                                                         GtkTreePath *path,
                                                         GtkTreeIter *iter,
                                                         DoCashViewInfo *info)
{
    GValue code = {0,};
    gtk_tree_model_get_value(model, iter, COLUMN_CASH_CODE, &code);
    if (g_value_get_int(&code) == info->cash_code) {
        info->path_ = gtk_tree_path_copy(path);
        return TRUE;
    }
    return FALSE;
}
gboolean update_cash_info (DoCashViewInfo *cash_info)
{
    GtkTreeIter iter;
    cash_info->path_ = NULL;
    gtk_tree_model_foreach(cash_info->model_, (GtkTreeModelForeachFunc)do_cash_view_find_by_code, cash_info);
    if ( cash_info->path_ ) {
        gtk_tree_model_get_iter(GTK_TREE_MODEL(cash_info->model_), &iter, cash_info->path_);
        gtk_list_store_set(GTK_LIST_STORE(cash_info->model_), &iter,
                COLUMN_INFO, cash_info->state.c_i,
                COLUMN_EXCHANGE_STATUS, cash_info->state.c_e_status,
                COLUMN_EXCHANGE_MESSAGE, cash_info->state.c_e_message,
                COLUMN_CASH_VERSION, cash_info->state.c_cash_ver,
                COLUMN_CASH_LICENSE_EXP, cash_info->state.c_cash_license_exp,
                COLUMN_VERSION, cash_info->state.c_e_version,
                COLUMN_DCLUB_STATUS, cash_info->state.c_d_status,
                COLUMN_DCLUB_MESSAGE, cash_info->state.c_d_message,
                COLUMN_CASH_INFO, cash_info->state.c_cash_info,
                -1);
        gtk_tree_path_free(cash_info->path_);
    }
    g_free(cash_info);
    return FALSE;
}
static gpointer do_cash_view_get_cash_info(DoCashViewInfo *cash_info)
{
    artix_info_t *artix_info = NULL;
    do_rpc_client_t *client = NULL;
    do_gtk_log_off();
    int i;

    while ( TRUE ) {

        if ( *cash_info->finalized_ )
            break;

        client = NULL;
        /*if ( cash_info->cash_code != 18 ) { // && cash_info->cash_code != 99 ) {
            cash_info->host[0]='\0';
            client = NULL;
        }*/
        if ( cash_info->host )
            artix_info = artix_info_new_default(cash_info->host, TRUE);
        else
            artix_info = NULL;
        if ( !artix_info ) {
            cash_info->state.c_i = -1;
            strcpy(cash_info->state.c_e_message,"");
            cash_info->state.c_e_status = -1;
            strcpy(cash_info->state.c_d_message,"");
            strcpy(cash_info->state.c_cash_ver,"");
            cash_info->state.c_d_status = -1;
        }
        else {
            char **module_desc = NULL;
            char *license_date_str=NULL;
            int count_desc = 0;

            strcpy(cash_info->state.c_cash_ver,"");
            if ( artix_info_get_module_description(artix_info, "Artix", &module_desc, &count_desc) && count_desc ) {
                gchar *value = NULL, *tmp;
                int i;
                for ( i = 0; i < count_desc; i++ ) {
                    if ( !module_desc[i] ) break;
                    if ( !strncmp(module_desc[i],"license", strlen("license")) ) {
                        char *ch;
                        ch = strstr(module_desc[i],"exp_date_str");
                        if ( ch ) {
                            license_date_str=strdup(ch+strlen("exp_date_str")+1);
                            ch = license_date_str;
                            while ( ch[0] != '\0'   ) {
                                if ( ch[0] == '&' ) {
                                    ch[0] = '\0';
                                    break;
                                }
                                ch++;
                            }
                        }
                    }
                    if ( value ) {
                        tmp = value;
                        value = g_strdup_printf("%s\n%s",tmp, module_desc[i]);
                        g_free(tmp);
                    }
                    else
                        value = g_strdup(module_desc[i]);
                    if ( i == 2 ) break;
                }
                if ( !value ) value = strdup("");
                //printf("%s;%s\n",cash_info->host,value);//!!

                cash_info->state.c_cash_license_exp = -1;
                if ( license_date_str ) {
                    struct tm tm;
                    if ( do_atodate(license_date_str, &tm) )
                        cash_info->state.c_cash_license_exp = mktime(&tm);
                }

                sprintf(cash_info->state.c_cash_ver, "%s", value);
                //printf("%d %s",i, value);//!!
                g_free(value);
            }
            artix_info_list_string_free(module_desc, count_desc);

            cash_info->state.c_i = 1;
            client = do_rpc_client_new(cash_info->host, EXCHANGE_DEFAULT_TCP_PORT, DOMINO_GUEST_NAME, DOMINO_GUEST_NAME, TRUE);
            //client = do_rpc_client_new("localhost", DCLUB_DEFAULT_TCP_PORT, DOMINO_GUEST_NAME, DOMINO_GUEST_NAME);
            if (*cash_info->finalized_)
                break;
            cash_info->state.c_exchange_good = TRUE;
            if (!do_rpc_client_connect(client)) {
                strcpy(cash_info->state.c_e_message, "Невозможно соединиться");
                cash_info->state.c_e_status = -1;
                cash_info->state.c_exchange_good = FALSE;
                //do_log(LOG_INFO, "Not connect for %s:%d", cash_info->host, EXCHANGE_DEFAULT_TCP_PORT);
            }
            else {
                do_data_t *data = do_data_new();
                if (!do_rpc_client_send_command(client, "get_time_product_update", "", NULL, data)) {
                    strcpy(cash_info->state.c_e_message, "Невозможно выполнить get_time_product_update");
                    cash_info->state.c_exchange_good = FALSE;
                }
                else {
                    gchar *value;
                    value = do_data_str(data);
                    strcpy(cash_info->state.c_e_message,value);
                    g_free(value);
                }
                do_data_free(data);
                data = do_data_new(NULL, 0, 0);
                if (!do_rpc_client_send_command(client, "ver", "", NULL, data)) {
                    strcpy(cash_info->state.c_e_version, "Невозможно выполнить ver");
                    cash_info->state.c_exchange_good = FALSE;
                }
                else {
                    gchar *value;
                    value = do_data_str(data);
                    strcpy(cash_info->state.c_e_version, value);
                    g_free(value);
                }
                do_data_free(data);
                cash_info->state.c_e_status = 1;
            }
            do_rpc_client_free(client);
            client = NULL;
            if (*cash_info->finalized_)
                break;
            if (!cash_info->state.c_exchange_good) {
                gchar *buf;
                buf = g_strdup_printf("Exchange in cash %d : %s", cash_info->cash_code, cash_info->state.c_e_message);
                //do_log(LOG_INFO, "%s", buf);
                //send_error_message(buf);
                g_free(buf);
            }
            client = do_rpc_client_new(cash_info->host, DCLUB_DEFAULT_TCP_PORT, DOMINO_GUEST_NAME, DOMINO_GUEST_NAME, TRUE);
            if (!do_rpc_client_connect(client)) {
                strcpy(cash_info->state.c_d_message, "Невозможно соединиться");
                cash_info->state.c_d_status = -1;
                //do_log(LOG_INFO, "Not connect for %s:%d", cash_info->host, DCLUB_DEFAULT_TCP_PORT);
            }
            else {
                do_data_t *data = do_data_new(NULL, 0, 0);
                if (!do_rpc_client_send_command(client, "last", "", NULL, data))
                    strcpy(cash_info->state.c_d_message, "Невозможно выполнить last");
                else {
                    gchar *value;
                    value = do_data_str(data);
                    strcpy(cash_info->state.c_d_message, value);
                    g_free(value);
                    char *ch;
                    for (ch = cash_info->state.c_d_message; *ch != '\0'; ch++)
                        if (*ch == '\n')
                            *ch = ' ';
                }
                do_data_free(data);
                data = do_data_new(NULL, 0, 0);
                if (!do_rpc_client_send_command(client, "ver", "", NULL, data)) {
                    strcpy(cash_info->state.c_d_version,"Невозможно выполнить ver");
                }
                else {
                    gchar *value;
                    value = do_data_str(data);
                    strcpy(cash_info->state.c_d_version, value);
                    g_free(value);
                }
                do_data_free(data);
                cash_info->state.c_d_status = 1;
            }

            do_rpc_client_free(client);
            client = NULL;
            /*artix_cash_t cash_info_1;

            if ( artix_alias_get_cash(artix->alias, &cash_info_1) ) {
                char *shop, *cash;
                shop = artix_text(artix->alias, cash_info_1.shop);
                cash = artix_text(artix->alias, cash_info_1.cash);
                //c_cash_info = g_strdup_printf("shop:%s cash:%s shift:%d close:%d", shop, cash, cash_info.shift, cash_info.close);
                sprintf(cash_info->state.c_cash_info, "%d", cash_info_1.shift);
                g_free(shop);g_free(cash);
            }
            else*/
                strcpy(cash_info->state.c_cash_info, "N/A");
            //!! get cash info

        }
        if (*cash_info->finalized_)
            break;
        DoCashViewInfo *tmp;
        //strcpy(cash_info->state.c_e_version, cash_info->host);
        tmp = g_memdup(cash_info, sizeof(*cash_info));
        g_idle_add((GSourceFunc)update_cash_info, tmp);
        if (artix_info)
            artix_info_free(artix_info);
        artix_info = NULL;
        for (i = 0; i < 20; i++) {
            if (*cash_info->finalized_)
                break;
            DO_SEC_SLEEP(1);
        }
    }
    if (client)
        do_rpc_client_free(client);
    if (artix_info)
        artix_info_free(artix_info);
    g_free(cash_info);

    do_gtk_log_on();
    g_thread_exit(NULL);
    return NULL;
}
/*
static gpointer do_cash_view_get_cash_info(DoCashViewInfo *cash_info)
{
    artix_t *artix = NULL;
    do_rpc_client_t *client = NULL;
    do_gtk_log_off();
    int i;

    while ( TRUE ) {

        if ( *cash_info->finalized_ )
            break;

        client = NULL;
        if ( cash_info->cash_code == 18 ) {
            client = NULL;
        }
        if ( cash_info->host )
            artix = artix_new_default(cash_info->host, TRUE);
        else
            artix = NULL;
        if ( !artix ) {
            cash_info->state.c_i = -1;
            strcpy(cash_info->state.c_e_message,"");
            cash_info->state.c_e_status = -1;
            strcpy(cash_info->state.c_d_message,"");
            strcpy(cash_info->state.c_cash_ver,"");
            cash_info->state.c_d_status = -1;
        }
        else {
            char **module_desc = NULL;
            int count_desc = 0;

            strcpy(cash_info->state.c_cash_ver,"");
            if ( artix_info_get_module_description(artix->info, "Artix", &module_desc, &count_desc) && count_desc ) {
                gchar *value = NULL, *tmp;
                int i;
                for ( i = 0; i < count_desc; i++ ) {
                    if ( value ) {
                        tmp = value;
                        value = g_strdup_printf("%s\n%s",tmp, module_desc[i]);
                        g_free(tmp);
                    }
                    else
                        value = g_strdup(module_desc[i]);
                }
                sprintf(cash_info->state.c_cash_ver, "%s", value);
                g_free(value);
            }
            artix_info_list_string_free(module_desc, count_desc);

            cash_info->state.c_i = 1;
            client = do_rpc_client_new(cash_info->host, EXCHANGE_DEFAULT_TCP_PORT, DOMINO_GUEST_NAME, DOMINO_GUEST_NAME, TRUE);
            //client = do_rpc_client_new("localhost", DCLUB_DEFAULT_TCP_PORT, DOMINO_GUEST_NAME, DOMINO_GUEST_NAME);
            if (*cash_info->finalized_)
                break;
            cash_info->state.c_exchange_good = TRUE;
            if (!do_rpc_client_connect(client)) {
                strcpy(cash_info->state.c_e_message, "Невозможно соединиться");
                cash_info->state.c_e_status = -1;
                cash_info->state.c_exchange_good = FALSE;
                do_log(LOG_INFO, "Not connect for %s:%d", cash_info->host, EXCHANGE_DEFAULT_TCP_PORT);
            }
            else {
                do_data_t *data = do_data_new();
                if (!do_rpc_client_send_command(client, "get_time_product_update", "", NULL, data)) {
                    strcpy(cash_info->state.c_e_message, "Невозможно выполнить get_time_product_update");
                    cash_info->state.c_exchange_good = FALSE;
                }
                else {
                    gchar *value;
                    value = do_data_str(data);
                    strcpy(cash_info->state.c_e_message,value);
                    g_free(value);
                }
                do_data_free(data);
                data = do_data_new(NULL, 0, 0);
                if (!do_rpc_client_send_command(client, "ver", "", NULL, data)) {
                    strcpy(cash_info->state.c_e_version, "Невозможно выполнить ver");
                    cash_info->state.c_exchange_good = FALSE;
                }
                else {
                    gchar *value;
                    value = do_data_str(data);
                    strcpy(cash_info->state.c_e_version, value);
                    g_free(value);
                }
                do_data_free(data);
                cash_info->state.c_e_status = 1;
            }
            do_rpc_client_free(client);
            client = NULL;
            if (*cash_info->finalized_)
                break;
            if (!cash_info->state.c_exchange_good) {
                gchar *buf;
                buf = g_strdup_printf("Exchange in cash %d : %s", cash_info->cash_code, cash_info->state.c_e_message);
                do_log(LOG_INFO, "%s", buf);
                //send_error_message(buf);
                g_free(buf);
            }
            client = do_rpc_client_new(cash_info->host, DCLUB_DEFAULT_TCP_PORT, DOMINO_GUEST_NAME, DOMINO_GUEST_NAME, TRUE);
            if (!do_rpc_client_connect(client)) {
                strcpy(cash_info->state.c_d_message, "Невозможно соединиться");
                cash_info->state.c_d_status = -1;
                do_log(LOG_INFO, "Not connect for %s:%d", cash_info->host, DCLUB_DEFAULT_TCP_PORT);
            }
            else {
                do_data_t *data = do_data_new(NULL, 0, 0);
                if (!do_rpc_client_send_command(client, "last", "", NULL, data))
                    strcpy(cash_info->state.c_d_message, "Невозможно выполнить last");
                else {
                    gchar *value;
                    value = do_data_str(data);
                    strcpy(cash_info->state.c_d_message, value);
                    g_free(value);
                    char *ch;
                    for (ch = cash_info->state.c_d_message; *ch != '\0'; ch++)
                        if (*ch == '\n')
                            *ch = ' ';
                }
                do_data_free(data);
                data = do_data_new(NULL, 0, 0);
                if (!do_rpc_client_send_command(client, "ver", "", NULL, data)) {
                    strcpy(cash_info->state.c_d_version,"Невозможно выполнить ver");
                }
                else {
                    gchar *value;
                    value = do_data_str(data);
                    strcpy(cash_info->state.c_d_version, value);
                    g_free(value);
                }
                do_data_free(data);
                cash_info->state.c_d_status = 1;
            }

            do_rpc_client_free(client);
            client = NULL;
            artix_cash_t cash_info_1;
            if ( artix_alias_get_cash(artix->alias, &cash_info_1) ) {
                char *shop, *cash;
                shop = artix_text(artix->alias, cash_info_1.shop);
                cash = artix_text(artix->alias, cash_info_1.cash);
                //c_cash_info = g_strdup_printf("shop:%s cash:%s shift:%d close:%d", shop, cash, cash_info.shift, cash_info.close);
                sprintf(cash_info->state.c_cash_info, "%d", cash_info_1.shift);
                g_free(shop);g_free(cash);
            }
            else
                strcpy(cash_info->state.c_cash_info, "N/A");
            //!! get cash info

        }
        if (*cash_info->finalized_)
            break;
        DoCashViewInfo *tmp;
        //strcpy(cash_info->state.c_e_version, cash_info->host);
        tmp = g_memdup(cash_info, sizeof(*cash_info));
        g_idle_add((GSourceFunc)update_cash_info, tmp);
        if (artix)
            artix_free(artix);
        artix = NULL;
        for (i = 0; i < 20; i++) {
            if (*cash_info->finalized_)
                break;
            DO_SEC_SLEEP(1);
        }
    }
    if (client)
        do_rpc_client_free(client);
    if (artix)
        artix_free(artix);
    g_free(cash_info);

    do_gtk_log_on();
    g_thread_exit(NULL);
    return NULL;
}
*/
static GtkListStore *do_cash_view_create_model(DoCashView *embed)
{
	DoCashViewPrivate *priv = DO_CASH_VIEW_GET_PRIVATE(embed);
    GtkListStore    *model;
    do_alias_t      *alias;
    do_ret_list_t   *list;
    do_extended_t   *extended;
    GtkTreeIter      iter;
    gint indx;
    partner_key0_t partner_key0;
    struct PACK_STRUCT {
        BTI_LONG code;
        BTI_CHAR sklad[10];
        BTI_CHAR name[50];
        BTI_CHAR params[2048];
    } *ret_;

    alias = domino_alias_new(priv->alias_name);
    if (!alias || !do_alias_open(alias, 0, DO_DB_PARTNER, DO_DB_END))
        return NULL;

    model = priv->model = gtk_list_store_new(NO_COLUMNS, G_TYPE_INT,  G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                              G_TYPE_INT,
                              G_TYPE_STRING,
                              G_TYPE_LONG,
                              G_TYPE_STRING,
                              G_TYPE_INT, G_TYPE_STRING,
                              G_TYPE_INT, G_TYPE_STRING,
                              G_TYPE_STRING, G_TYPE_STRING,

                                 G_TYPE_POINTER);

    extended = do_extended_new(alias);
    do_extended_set_file_db(extended, DO_DB_PARTNER);
    do_extended_set_key(extended, 0, &partner_key0);
    do_extended_set_filter(extended, TRUE, 1, sizeof(partner_key0.g_code));

    do_text_set(alias, partner_key0.g_code, do_param(DO_PARAM_PARTNER_REGION_CASH));
    partner_key0.code = 0;

    do_extended_set_filter_term(extended, 0, DO_EXTENDED_TYPE_STRING,
                                offsetof(partner_struct_t, g_code), DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO, &partner_key0.g_code);

    do_extended_set_fields(extended, 4,
                           do_size_and_offset(partner_struct_t, code),
                           do_size_and_offset(partner_struct_t, sklad),
                           do_size_and_offset(partner_struct_t, name),
                           do_size_and_offset(partner_struct_t, params)
                           );

    while ( ((list = do_extended_get(extended, 100)) != NULL) &&
            do_ret_list_count(list) ) {
        for (indx = 0; indx < do_ret_list_count(list); indx++) {
            gchar *name;
            gchar *unit;
            gchar *host;
            gchar *params;
            gchar *number;
            gchar *sb_terminal;
            GThread *thread;
            DoCashViewInfo *info;

            ret_ = do_ret_list_item_data(list, indx);
            params = do_alias_text_(alias, ret_->params,
                              do_ret_list_item_len(list, indx) -
                              (sizeof(*ret_) -
                               sizeof(ret_->params)));
            name = do_text(alias, ret_->name);
            unit = do_text(alias, ret_->sklad);
            number = do_fromparam(params, "CASH", TRUE);
            host = g_strdup_printf("cash-%s-%s", unit, number);
            //host = g_strdup_printf("kassa-%d", (int)ret_->code);
            //)do_fromparam(params, do_param(DO_PARAM_PARTNER_PARAMETR_CASH_IP), TRUE);
            sb_terminal = do_fromparam(params, "SB_MM", TRUE);


            if ( atoi(unit) == 0 )//!! || ret_->code != 18) //!!
                thread = NULL;
            else {
                info = g_malloc0(sizeof(DoCashViewInfo));
                info->model_ = GTK_TREE_MODEL(model);
                info->cash_code = ret_->code;
                strcpy(info->host, host);
                info->finalized_ = &priv->finalized;
                thread = g_thread_new("update_info",(GThreadFunc)do_cash_view_get_cash_info, info);

                gtk_list_store_append(model, &iter);
                //info->path =gtk_tree_model_get_path(GTK_TREE_MODEL(model), &iter);
                gtk_list_store_set(model, &iter,
                    COLUMN_CASH_CODE, ret_->code,
                    COLUMN_CASH_NAME, name,
                    COLUMN_CASH_HOST, host,
                    COLUMN_UNIT, unit,
                    COLUMN_INFO, (*host == '\0') ? 2 : 0,
                    COLUMN_CASH_VERSION, "",
                    COLUMN_CASH_LICENSE_EXP, -1,
                    COLUMN_VERSION, "",
                    COLUMN_EXCHANGE_STATUS, 2,
                    COLUMN_EXCHANGE_MESSAGE, "",
                    COLUMN_DCLUB_STATUS, 2,
                    COLUMN_DCLUB_MESSAGE, "",
                    COLUMN_CASH_INFO,"",
                    COLUMN_CASH_TERMINAL, sb_terminal,
                    COLUMN_THREAD, thread,
                     -1);
            }
            do_free(name);
            do_free(unit);
            do_free(host);
            do_free(number);
            do_free(params);
        }
        do_ret_list_free(list);
    }
    if (list)
        do_ret_list_free(list);
    do_extended_free(extended);
    do_alias_free(alias);
    return model;
}
static void do_cash_view_row_activated(GtkTreeView *tree_view, GtkTreePath *path,  GtkTreeViewColumn *column, DoCashView *view)
{
    do_cash_view_view_do_edit(DO_VIEW(view), NULL);
}

static GObject *do_cash_view_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject             *object;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    GtkListStore        *model;
    DoTreeView          *view;
	DoCashView          *embed;
	GtkTreeView         *tree_view;
	DoCashViewPrivate   *priv;

	object = G_OBJECT_CLASS (do_cash_view_parent_class)->constructor(type, n_construct_properties, construct_params);

	priv = DO_CASH_VIEW_GET_PRIVATE(object);
	view = DO_TREE_VIEW(object);
	embed = DO_CASH_VIEW(object);
	g_object_get(object, "tree-view", &tree_view, NULL);

    g_signal_connect(tree_view, "row-activated", G_CALLBACK(do_cash_view_row_activated), object);
    gtk_tree_view_set_fixed_height_mode(GTK_TREE_VIEW(tree_view), FALSE);

	priv->tree_view = tree_view;
    priv->selection = gtk_tree_view_get_selection(tree_view);

	priv->model = model = do_cash_view_create_model(embed);
	if (model)
        g_object_set(view, "model", model,
                           "fixed-height-mode", FALSE,
                           NULL);

    col = do_tree_view_add_column(view, "Unit", "Аптека", 40);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_UNIT);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_UNIT);

    col = do_tree_view_add_column(view, "Name", "Касса", 100);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_CASH_NAME);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_CASH_NAME);

    col = do_tree_view_add_column(view, "Info", "", 40);
    renderer = gtk_cell_renderer_pixbuf_new ();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_INFO);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
             do_tree_cell_data_func_pixbuf_connect, GINT_TO_POINTER(COLUMN_INFO), NULL);

    col = do_tree_view_add_column(view, "ArtixVer", "Версия Артикс", 100);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_column_pack_start (col, renderer, FALSE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_CASH_VERSION);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_CASH_VERSION);

    col = do_tree_view_add_column(view, "ArtixLicense", "Лицензия", 100);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_column_pack_start (col, renderer, FALSE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_CASH_LICENSE_EXP);
    //gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_CASH_LICENSE_EXP);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
            do_tree_cell_data_func_date_time_date_only, GINT_TO_POINTER(COLUMN_CASH_LICENSE_EXP), NULL);

    col = do_tree_view_add_column(view, "ExchangedVer", "Версия ПО", 100);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_column_pack_start (col, renderer, FALSE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_VERSION);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_VERSION);

    col = do_tree_view_add_column(view, "Exchanged", "Обмен", 100);
    renderer = gtk_cell_renderer_pixbuf_new ();
    gtk_tree_view_column_pack_start (col, renderer, FALSE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_EXCHANGE_STATUS);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
             do_tree_cell_data_func_pixbuf_connect, GINT_TO_POINTER(COLUMN_EXCHANGE_STATUS), NULL);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_EXCHANGE_MESSAGE);


    col = do_tree_view_add_column(view, "Dclub", "Скидки", 100);
    renderer = gtk_cell_renderer_pixbuf_new ();
    gtk_tree_view_column_pack_start (col, renderer, FALSE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_DCLUB_STATUS);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
             do_tree_cell_data_func_pixbuf_connect, GINT_TO_POINTER(COLUMN_DCLUB_STATUS), NULL);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_DCLUB_MESSAGE);
//    gtk_tree_view_column_set_sort_column_id (col, COLUMN_DCLUB_MESSAGE);

    /*col = do_tree_view_add_column(view, "DclubCashInfo", "Смена", 100);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_column_pack_start (col, renderer, FALSE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_CASH_INFO);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_CASH_INFO);*/

    col = do_tree_view_add_column(view, "SberTerminal", "Термина", 100);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_column_pack_start (col, renderer, FALSE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_CASH_TERMINAL);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_CASH_TERMINAL);

    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(priv->model), COLUMN_EXCHANGE_STATUS, GTK_SORT_ASCENDING);
    gtk_tree_view_set_search_column(tree_view, 0);
	gtk_widget_show(GTK_WIDGET(object));

	return object;
}
static gboolean do_cash_view_kill_thread(GtkTreeModel *model,
                                            GtkTreePath *path,
                                            GtkTreeIter *iter,
                                            gpointer data)
{
    GValue thread = {0,};
    gtk_tree_model_get_value(model, iter, COLUMN_THREAD, &thread);
    if (g_value_get_pointer(&thread)) {
        g_thread_join((GThread*)g_value_get_pointer(&thread));
    }
    return FALSE;
}
static void do_cash_view_finalize (GObject *object)
{
    DoCashViewPrivate *priv;
	priv = DO_CASH_VIEW_GET_PRIVATE (object);
	if (priv->alias_name)
		g_free(priv->alias_name);
    priv->finalized = TRUE;

    gtk_tree_model_foreach(GTK_TREE_MODEL(priv->model), do_cash_view_kill_thread, NULL);

	G_OBJECT_CLASS (do_cash_view_parent_class)->finalize (object);
}

static void do_cash_view_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoCashViewPrivate *priv = DO_CASH_VIEW_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_ALIAS:
            g_value_set_string (value, priv->alias_name);
			break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}
	/* no readable properties */
	g_assert_not_reached ();
}

static void do_cash_view_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoCashViewPrivate *priv = DO_CASH_VIEW_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_ALIAS:
            if (!priv->alias_name)
                priv->alias_name = g_value_dup_string (value);
			break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}
}

static void do_cash_view_class_init (DoCashViewClass *klass)
{
    GObjectClass *o_class = G_OBJECT_CLASS (klass);
    //GObjectClass *object_class = GTK_OBJECT_CLASS (klass);

	o_class->constructor  = do_cash_view_constructor;
	o_class->finalize     = do_cash_view_finalize;
	o_class->get_property = do_cash_view_get_property;
	o_class->set_property = do_cash_view_set_property;

	g_type_class_add_private (o_class, sizeof (DoCashViewPrivate));

    g_object_class_install_property
        (o_class,
         PROP_ALIAS,
         g_param_spec_string("alias-name",
                      "Алиас БД",
                      "Алиас БД",
                      NULL,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

}

GtkWidget *do_cash_view_new (const gchar *alias_name)
{
	return g_object_new (DO_TYPE_CASH_VIEW,
			     "alias-name", alias_name,
			     "object-root-path", OBJECT_ROOT_PATH,
			     (gpointer) NULL);
}
static gboolean do_cash_view_view_do_grab_focus(DoView *view)
{
    return do_tree_view_grab_focus(DO_TREE_VIEW(view));
}
static void	do_cash_view_view_do_close(DoView *view)
{
    gtk_widget_destroy(GTK_WIDGET(view));
}
static const char *do_cash_view_view_get_title(DoView *view)
{
    return "Каccы ";
}
static GdkPixbuf  *do_cash_view_view_get_icon(DoView *view)
{
    DoCashViewPrivate *priv = DO_CASH_VIEW_GET_PRIVATE(view);
    return priv->icon;
}

static gboolean	do_cash_view_view_can_do_edit(DoView *view, const gchar *tab)
{
    if (tab && *tab != '\0')
        return FALSE;
    gboolean retval = FALSE;
	DoCashViewPrivate *priv = DO_CASH_VIEW_GET_PRIVATE(view);
    GList *list = gtk_tree_selection_get_selected_rows(priv->selection, (GtkTreeModel**)&priv->model);
    if (list) {
        retval = (g_list_length(list) == 1);
        g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
        g_list_free(list);
    }
    else
        retval = FALSE;
    return retval;
}
static void	do_cash_view_show_record(DoView *view, DoDialogAction action)
{
}

static void	do_cash_view_view_do_edit(DoView *view, const gchar *tab)
{
    do_cash_view_show_record(view, DO_DIALOG_ACTION_EDIT);
}
#define K_KEY "cash"
#ifdef _WIN32
#define TERMINAL_PROG "putty"
#else
#define TERMINAL_PROG "ssh"
#define VNC_PROG "vncc"
#endif
static const gchar *crnt_cash_host;

static void do_cash_view_run_ssh(GtkMenuItem *menuitem, gpointer data)
{
    gint number;
    number = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(menuitem), K_KEY));
    gchar *param = NULL;
#ifdef _WIN32
    switch ( number ) {
        case 0:
            param = g_strdup_printf("-load k%d -l root", crnt_cash_code);
            break;
        case 1:
            param = g_strdup_printf("-load kassa%d -l conspy", crnt_cash_code);
            break;
    };
    gchar *cmdline;
    cmdline = g_strdup_printf("%s %s", TERMINAL_PROG, param);
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char *argv0 = NULL;

    memset (&si, 0, sizeof (si));
    si.cb = sizeof (&si);
    memset (&pi, 0, sizeof (pi));

    if ( !CreateProcess (argv0, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) )
        DOMINO_SHOW_ERROR("Не удалось запустить программу: %s", g_win32_error_message (GetLastError ()));
    g_free(cmdline);
    CloseHandle (pi.hThread);
#else
    const char *progs;
    do_string_list_t *prog;
    GError *error = NULL;
    DOMINO_LOCAL_GET("Extended", "ssh", &progs, NULL);
    prog = do_string_list_new_from_csv(progs);
    if ( number >= prog->count ) {
#ifndef MIN_SCREEN
        do_common_edit(NULL);
#endif
    }
    else {
        param = g_strdup(crnt_cash_host);
        gchar *argv[3] = {prog->list[number], param, NULL};
        if ( !g_spawn_async(NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error) ) {
            DOMINO_SHOW_ERROR("Не удалось запустить программу: %s", error->message);
            g_error_free (error);
        }
    }
    do_string_list_free(prog);
#endif
        g_free(param);
}
static void	do_cash_view_view_do_popup(DoView *view, GdkEventButton* event)
{
	DoCashViewPrivate *priv = DO_CASH_VIEW_GET_PRIVATE(view);
    GtkWidget *menu, *item;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    GList *selected = NULL;
    GValue host = {0,};
    GValue name = {0,};
    gchar *value;
    GtkTreeModel *model;


    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(priv->tree_view));
	selected = gtk_tree_selection_get_selected_rows (selection, &model);

	if ( g_list_length (selected) == 0 ||
	     gtk_tree_model_get_iter (model, &iter, selected->data) != TRUE )
        return;

    gtk_tree_model_get_value(model, &iter, COLUMN_CASH_HOST, &host);
    gtk_tree_model_get_value(model, &iter, COLUMN_CASH_NAME, &name);
    //crnt_cash_code = g_value_get_int(&code)
    crnt_cash_host = g_value_get_string(&host);

    menu = gtk_menu_new ();
    value = g_strdup_printf("root@%s", g_value_get_string(&name));
	item = gtk_menu_item_new_with_label(value);
    g_object_set_data(G_OBJECT(item), K_KEY, GINT_TO_POINTER(0));
	g_free(value);
    g_signal_connect(item, "activate", G_CALLBACK (do_cash_view_run_ssh), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

    value = g_strdup_printf("conspy@%s", g_value_get_string(&name));
	item = gtk_menu_item_new_with_label(value);
    g_object_set_data(G_OBJECT(item), K_KEY, GINT_TO_POINTER(1));
	g_free(value);
    g_signal_connect(item, "activate", G_CALLBACK (do_cash_view_run_ssh), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

    value = g_strdup_printf("vnc@%s", g_value_get_string(&name));
	item = gtk_menu_item_new_with_label(value);
    g_object_set_data(G_OBJECT(item), K_KEY, GINT_TO_POINTER(2));
	g_free(value);
    g_signal_connect(item, "activate", G_CALLBACK (do_cash_view_run_ssh), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

	gtk_widget_show_all(menu);
    gtk_menu_attach_to_widget (GTK_MENU (menu), GTK_WIDGET(view), NULL);

    do_tree_view_popup(DO_TREE_VIEW(view), event, menu);
}
#endif
