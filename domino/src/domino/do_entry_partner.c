

#include "do_entry_partner.h"
#include "do_entry.h"
#include "do_icons.h"
#include "do_context.h"
#include "do_utilx.h"
#include "do_validate.h"
#include <stdlib.h>
#include <string.h>
//#include "do_marshal.h"

enum
{
	PROP_0,
	PROP_FLAGS,
	PROP_REGION,
    PROP_ALIAS,
    PROP_CONTEXT,
};

#define DO_ENTRY_PARTNER_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_ENTRY_PARTNER, DoEntryPartnerPrivate))

typedef struct {
    gchar *region;
    gint   code;
    gchar *name;
} DoPartnerItem;


struct _DoEntryPartnerPrivate
{
	GtkListStore *store;
	DoContext    *context;
	do_alias_t   *alias;
	DoEntryPartnerFlags flags;
	do_sort_list_t *partners;
	gchar *region;
};

GType do_entry_partner_flags_get_type(void)
{
    static GType type = 0;
    if (G_UNLIKELY (type == 0))
    {
        static const GFlagsValue _do_entry_partner_flags_values[] = {
         { DO_ENTRY_PARTNER_FLAGS_SEARCH, "DO_ENTRY_PARTNER_FLAGS_OUR_SEARCH", "search" },
         { DO_ENTRY_PARTNER_FLAGS_EMPTY, "DO_ENTRY_PARTNER_FLAGS_EMPTY", "empty" },
        };
        type = g_flags_register_static ("DoEntryPartnerFlags", _do_entry_partner_flags_values);
    }
    return type;
}

static void      do_entry_partner_validate_init (DoValidateIface *iface);
static void      do_entry_partner_completion (DoValidate *iface);
static gboolean  do_entry_partner_valid(DoValidate *iface);
static gboolean  do_entry_partner_substitution(DoValidate *iface);

G_DEFINE_TYPE_WITH_CODE (DoEntryPartner, do_entry_partner, DO_TYPE_ENTRY,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VALIDATE,
						do_entry_partner_validate_init));

static void do_entry_partner_validate_init (DoValidateIface *iface)
{
    iface->valid = do_entry_partner_valid;
    iface->substitution = do_entry_partner_substitution;
    iface->completion = do_entry_partner_completion;
}

static void do_entry_partner_init(DoEntryPartner *dialog)
{
//    DoEntryPartnerPrivate *priv = DO_ENTRY_PARTNER_GET_PRIVATE (dialog);
}
static gboolean match_func(GtkEntryCompletion *completion,
                                                         const gchar *key,
                                                         GtkTreeIter *iter,
                                                         gpointer user_data)
{
    return TRUE;
}

static gboolean do_entry_partner_valid(DoValidate *iface)
{
    GtkEntry *entry = GTK_ENTRY(iface);
    gchar *text = (char*)gtk_entry_get_text(entry);
	DoEntryPartnerPrivate *priv = DO_ENTRY_PARTNER_GET_PRIVATE (iface);
	do_alias_t *alias = priv->alias;

    if ( *text == '\0' &&
         (priv->flags & DO_ENTRY_PARTNER_FLAGS_EMPTY) )
        return TRUE;

    if ( !priv->context )
        return FALSE;

    do_context_set_kind(priv->context, DO_CONTEXT_KIND_UNDEFINE);
    partner_key0_t partner_key0;
    if ( do_util_str_to_partner_key0(alias, text, &partner_key0) ) {
        partner_rec_t partner;
        if ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) == DO_OK ) {
            gchar *value;
            gint code;
            code = partner.data.code;
            value = do_text(alias, partner.data.g_code);
            do_context_set_kind(priv->context, DO_CONTEXT_KIND_PARTNER);
            g_object_set(priv->context, "region-code", value, "partner-code", code, NULL);
            g_free(value);
            g_object_notify (G_OBJECT (iface), "context");
            return TRUE;
        }
    }
    DoPartnerItem item;
    gint indx;
    item.name = text;

    if ( do_sort_list_find(priv->partners, &indx, &item) ) {
        DoPartnerItem *item;
        item = priv->partners->list[indx];
        do_context_set_kind(priv->context, DO_CONTEXT_KIND_PARTNER);
        g_object_set(priv->context, "region-code", item->region, "partner-code", item->code, NULL);
        g_object_notify (G_OBJECT (iface), "context");
        return TRUE;
    }

    return FALSE;
}

static GObject *do_entry_partner_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject *object;
	GtkEntry *entry;
	GtkEntryCompletion *completion;
	DoEntryPartnerPrivate *priv;
	GtkListStore *store;

	object = G_OBJECT_CLASS (do_entry_partner_parent_class)->constructor(type, n_construct_properties, construct_params);

	entry = GTK_ENTRY(object);
    priv = DO_ENTRY_PARTNER_GET_PRIVATE (object);

    completion = gtk_entry_get_completion(entry);
    gtk_entry_completion_set_match_func(completion, match_func, NULL, NULL);

    store = priv->store = gtk_list_store_new (1, G_TYPE_STRING);
    gtk_entry_completion_set_model(completion, GTK_TREE_MODEL(store));
    g_object_unref (store);
    priv->context = do_context_new();
    do_context_set_kind(priv->context, DO_CONTEXT_KIND_UNDEFINE);
    gtk_entry_completion_set_text_column (completion, 0);
	return object;
}

static int free_item(DoPartnerItem *item, void *user_data)
{
    g_free(item->region);
    g_free(item->name);
    return TRUE;
}
static void do_entry_partner_finalize (GObject *object)
{
	DoEntryPartnerPrivate *priv = DO_ENTRY_PARTNER_GET_PRIVATE (object);
	if ( priv->alias )
        do_alias_free(priv->alias);
    if ( priv->context )
        g_object_unref(priv->context);
    if ( priv->partners ) {
        do_list_foreach(priv->partners, (do_list_foreach_func) free_item, NULL);
        do_sort_list_free(priv->partners);
    }
	G_OBJECT_CLASS (do_entry_partner_parent_class)->finalize (object);
}
static int cmp_item(DoPartnerItem *item1, DoPartnerItem *item2)
{
    return do_text_utf8_str_cmp_no_case(item1->name, item2->name);
}
static gboolean do_entry_partner_fill_list(DoEntryPartner *partner, const gchar *region)
{
	DoEntryPartnerPrivate *priv = DO_ENTRY_PARTNER_GET_PRIVATE (partner);
    partner_key0_t partner_key0;
    do_alias_t *alias = priv->alias;
    gboolean retval = FALSE;

    struct PACK_STRUCT {
        BTI_CHAR g_code[3];
        BTI_LONG code;
        BTI_CHAR name[50];
    } *rec_;

    if ( priv->partners ) {
        do_list_foreach(priv->partners, (do_list_foreach_func) free_item, NULL);
        do_sort_list_free(priv->partners);
    }
    priv->partners = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)cmp_item, NULL);
    do_text_set(alias, partner_key0.g_code, region);
    partner_key0.code = 0;

    do_extended_t *extended;
    do_ret_list_t *list;
    extended = do_extended_new(alias);
    do_extended_set_file_db(extended, DO_DB_PARTNER);
    do_extended_set_key(extended, 0, &partner_key0);
    do_extended_set_filter(extended, TRUE, 1, sizeof(partner_key0.g_code));

    do_extended_set_filter_term(extended, 0, DO_EXTENDED_TYPE_STRING,
                                offsetof(partner_struct_t, g_code), DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO, &partner_key0.g_code);
    do_extended_set_fields(extended, 3,
                           do_size_and_offset(partner_struct_t, g_code),
                           do_size_and_offset(partner_struct_t, code),
                           do_size_and_offset(partner_struct_t, name)
                           );

    list = do_extended_get(extended, -1);

    if ( list ) {
        retval = TRUE;
        int i;
        DoPartnerItem item;
        for ( i = 0; i < do_ret_list_count(list); i++ ) {
            rec_ = do_ret_list_item_data(list, i);
            item.region = do_text(alias, rec_->g_code);
            item.name = do_text(alias, rec_->name);
            item.code = rec_->code;
            do_sort_list_add_alloc(priv->partners, &item, sizeof(item));
        }

        do_ret_list_free(list);
    }
    do_extended_free(extended);
    return retval;
}

static void do_entry_partner_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoEntryPartnerPrivate *priv = DO_ENTRY_PARTNER_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_FLAGS:
			priv->flags = g_value_get_flags (value);
			break;
		case PROP_ALIAS:
            if ( priv->alias )
                do_alias_free(priv->alias);
            priv->alias = domino_alias_new(g_value_dup_string(value));
            do_alias_open(priv->alias, FALSE, DO_DB_PARTNER, DO_DB_END);
			break;
        case PROP_REGION:
            do_entry_partner_fill_list(DO_ENTRY_PARTNER(object), g_value_dup_string(value));
            priv->region = g_value_dup_string(value);
            break;
	}

}
static void do_entry_partner_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoEntryPartnerPrivate *priv = DO_ENTRY_PARTNER_GET_PRIVATE (object);
    switch (prop_id)
    {
        case PROP_FLAGS:
			g_value_set_flags (value, priv->flags);
			break;
        case PROP_CONTEXT:
            g_value_set_object(value, priv->context);
            break;
    }
}

static void do_entry_partner_class_init (DoEntryPartnerClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
    //GtkWidgetClass *widget_class = (GtkWidgetClass*)klass;

	object_class->constructor  = do_entry_partner_constructor;
	object_class->finalize     = do_entry_partner_finalize;
	object_class->get_property = do_entry_partner_get_property;
	object_class->set_property = do_entry_partner_set_property;

	g_type_class_add_private (object_class, sizeof (DoEntryPartnerPrivate));

	g_object_class_install_property
		(object_class,
		 PROP_FLAGS,
		 g_param_spec_flags ("flags",
				     "flags",
				     "flags",
				     DO_TYPE_ENTRY_PARTNER_FLAGS,
				     0,
				     G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
				     G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property
        (object_class,
         PROP_ALIAS,
         g_param_spec_string("alias-name",
                      "Алиас БД",
                      "Алиас БД",
                      NULL,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property
        (object_class,
         PROP_REGION,
         g_param_spec_string("region-code",
                      "region-code",
                      "region-code",
                      NULL,
                       G_PARAM_WRITABLE));
    g_object_class_install_property (object_class,
				   PROP_CONTEXT,
				   g_param_spec_object ("context",
 						      "Контекст партнера",
 						      "Контекст партнера",
 						      DO_TYPE_CONTEXT,
 						      G_PARAM_READABLE));
}

GtkWidget *do_entry_partner_new(const gchar *alias_name, DoEntryPartnerFlags flags)
{
	return g_object_new (DO_TYPE_ENTRY_PARTNER,
	                     "flags", flags,
	                     "alias-name", alias_name,
			     (gpointer) NULL);
}

static void do_entry_partner_completion (DoValidate *iface)
{
	DoEntryPartnerPrivate *priv = DO_ENTRY_PARTNER_GET_PRIVATE (iface);
	DoEntryPartner *entry = DO_ENTRY_PARTNER(iface);
    const gchar *text;
    do_alias_t *alias = priv->alias;

    gtk_list_store_clear(priv->store);
    text = gtk_entry_get_text(GTK_ENTRY(entry));
    if ( text == '\0' )
        return;
    partner_key0_t partner_key0;
    GtkTreeIter iter;
    if ( do_util_str_to_partner_key0(alias, text, &partner_key0) ) {
        partner_rec_t partner;
        if ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) == DO_OK ) {
            char *value = do_text(alias, partner.data.name);
            gtk_list_store_append (priv->store, &iter);
            gtk_list_store_set (priv->store, &iter, 0, value, -1);
            g_free(value);
            return;
        }
    }
    int i;
    for ( i = 0; i < priv->partners->count; i++ ) {
        DoPartnerItem *item;
        item = priv->partners->list[i];
        if ( do_text_utf8_find_str(item->name, text) ) {
            gtk_list_store_append (priv->store, &iter);
            gtk_list_store_set (priv->store, &iter, 0, item->name, -1);
        }
    }
}
static gboolean do_entry_partner_substitution (DoValidate *iface)
{
	DoEntryPartnerPrivate *priv = DO_ENTRY_PARTNER_GET_PRIVATE (iface);
	GtkEntry *entry = GTK_ENTRY(iface);
	const gchar *text = gtk_entry_get_text(entry);
	GtkTreePath *path;
	GtkTreeIter  iter;
	GValue  value = {0,};
	gint count = 0;
	gboolean retval = TRUE;

	do_entry_partner_completion(iface);

    path = gtk_tree_path_new_first();
    while (count < 3 &&  gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->store), &iter, path)) {
        if (!count)
            gtk_tree_model_get_value(GTK_TREE_MODEL(priv->store), &iter, 0, &value);
        count++;
        gtk_tree_path_next(path);
    }
    if ( (count == 1 && strcmp(text, g_value_get_string(&value))) ) {
        gtk_entry_set_text(entry, g_value_get_string(&value));
        retval = FALSE;
    }
    gtk_tree_path_free(path);
    return retval;
}
void do_entry_partner_model_fill(DoEntryPartner *partner, GtkListStore *store)
{
	DoEntryPartnerPrivate *priv = DO_ENTRY_PARTNER_GET_PRIVATE (partner);
	gtk_list_store_clear(store);
	DoPartnerItem *item;
	int i;
	for ( i = 0; i < priv->partners->count; i++ ) {
	    GtkTreeIter iter;
	    item = priv->partners->list[i];
	    gtk_list_store_append(store, &iter);
	    gtk_list_store_set(store, &iter,
                          0, item->name,
                          -1);
	}
}
gboolean do_entry_partner_add_partner_by_name(DoEntryPartner *object, const gchar *name)
{
	DoEntryPartnerPrivate *priv = DO_ENTRY_PARTNER_GET_PRIVATE (object);
    DoPartnerItem item;
    gint indx, res;
    item.name = (gchar*)name;

    if ( do_sort_list_find(priv->partners, &indx, &item) ) {
        DoPartnerItem *item;
        item = priv->partners->list[indx];
        DOMINO_SHOW_ERROR("Партнер: %s уже существует", item->name);

        return FALSE;
    }

    do_alias_t *main_a;
    main_a = domino_alias_new("main");
    if ( !main_a )
        return FALSE;
    if ( !do_alias_open(main_a, FALSE, DO_DB_PARTNER, DO_DB_END) ) {
        do_alias_free(main_a);
        return FALSE;
    }
    if ( !do_alias_tran_begin(main_a) ) {
        do_alias_free(main_a);
        return FALSE;
    }
    if ( !do_alias_tran_begin(priv->alias) ) {
        do_alias_tran_abort(main_a);
        do_alias_free(main_a);
        return FALSE;
    }
    res = TRUE;
    partner_rec_t partner;
    partner_key0_t partner_key0;
    int status;
    do_text_set(main_a, partner_key0.g_code, priv->region);
    partner_key0.code = 1;
    status = do_partner_key0(main_a, &partner_key0, DO_GET_EQUAL);
    while ( status == DO_OK ) {
        partner_key0.code++;
        status = do_partner_key0(main_a, &partner_key0, DO_GET_EQUAL);
    }
    if ( status == DO_OK )
        res = FALSE;
    if ( res ) {
        do_cpy(partner.data.g_code, partner_key0.g_code);
        do_cpy(partner.data.code, partner_key0.code);
        do_text_set(main_a, partner.data.otdel, domino_division());
        do_text_set_empty(partner.data.sklad);
        do_text_set(main_a, partner.data.name, name);
        do_text_set_empty(partner.data.bank);
        do_text_set_empty(partner.data.MFO);
        do_text_set_empty(partner.data.SHET);
        do_text_set_empty(partner.data.ADRES);
        do_text_set_empty(partner.data.PLAT_PGR);
        partner.data.PLAT_PAR = 0;
        do_text_set_empty(partner.data.KSHET);
        do_text_set_empty(partner.data.fio);
        memset(partner.data.SUM, 0, sizeof(partner.data.SUM));
        do_text_set_empty(partner.data.codestr);
        partner.data.state = 0;
        partner.data.type = 0;
        do_text_set_empty(partner.data.Klass);
        do_text_set_empty(partner.data.Param);
        memset(partner.data.Sum_N, 0, sizeof(partner.data.Sum_N));
        do_partner_params_clear(main_a, &partner);
        res = do_partner_insert(main_a, &partner) == DO_OK;
    }
    if ( res ) {
        partner_rec_t buf;
        switch (do_partner_get0(priv->alias, &buf, &partner_key0, DO_GET_EQUAL)) {
            case DO_KEY_NOT_FOUND:
                res = do_partner_insert(priv->alias, &partner) == DO_OK;
                break;
            case DO_OK:
                res = do_partner_update(priv->alias, &partner) == DO_OK;
                break;
            default:
                res = FALSE;
        }
    }

    if ( res )
        res = do_alias_tran_end(main_a);
    if ( res )
        res = do_alias_tran_end(priv->alias);
    if ( !res ) {
        do_alias_tran_abort(main_a);
        do_alias_tran_abort(priv->alias);
    }
    do_alias_free(main_a);
    if ( res ) {
        DoPartnerItem item;
        item.code = partner.data.code;
        item.name = do_text(priv->alias, partner.data.name);
        item.region = do_text(priv->alias, partner.data.g_code);
        do_sort_list_add_alloc(priv->partners, &item, sizeof(item));
        g_object_set(object, "text", "", NULL);
        g_object_set(object, "text", item.name, NULL);
    }
    return TRUE;
}
