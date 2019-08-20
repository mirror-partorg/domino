
#include "do_link_dialog.h"
#include "misc.h"
#include "exchange.h"
#include "do_message.h"
#include "exchange-const.h"
#include <glib.h>
#include <domino.h>


#define BORDER_SIZE 5
#define PADDING 6

#define DO_LINK_DIALOG_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_LINK_DIALOG, DoLinkDialogPrivate))


enum
{
	PROP_0 = 0,
	PROP_ALIAS,
	PROP_1C_OBJS,
	PROP_HANDBOOK,
	PROP_HANDBOOK_KEY,
	PROP_EMPTY,
	PROP_SAVED,
	PROP_COMMENT,
};

struct _DoLinkDialogPrivate
{
    do_alias_t    *alias;
    GSList        *objs_1c;
    DoExchangeHandbook  handbook;
    partner_key0_t     *partner_key;
    gboolean      init_handbook;
    gboolean      saved;
   

    GtkWidget     *label;
    GtkWidget     *tlabel;
    GtkWidget     *view;
    gchar         *name;
	GtkTreeViewColumn *col;
	GtkWidget     *button;

};

G_DEFINE_TYPE(DoLinkDialog, do_link_dialog, GTK_TYPE_DIALOG)

GType do_exchange_handbook_get_type(void)
{
    static GType etype = 0;
    if (G_UNLIKELY(etype == 0)) {
        static const GEnumValue values[] = {
            { DO_EXCHANGE_HANDBOOK_PARTNER, "DO_EXCHANGE_HANDBOOK_PARTNER", "partner" },
            { DO_EXCHANGE_HANDBOOK_SKLAD, "DO_EXCHANGE_HANDBOOK_SKLAD", "sklad" },
            //!!{ DO_EXCHANGE_HANDBOOK_CASHIER, "DO_EXCHANGE_HANDBOOK_CASHIER", "cashier" },
            { DO_EXCHANGE_HANDBOOK_FIRM, "DO_EXCHANGE_HANDBOOK_FIRM", "firm" },
            { 0, NULL, NULL }
        };
        etype = g_enum_register_static (g_intern_static_string ("DoExchangeHandbook"), values);
    }
    return etype;
}
enum {
    COL_GUID = 0,
    COL_CODE,
    COL_NAME,
    COL_DESC,
    COL_NAME1,
    COL_NAME_LIST,
    COL_NAME1_LIST,
    COL_NO
};
static GType col_types[COL_NO] =
{
    G_TYPE_STRING,
    G_TYPE_STRING,
    G_TYPE_STRING,
    G_TYPE_STRING,
    G_TYPE_STRING,
    G_TYPE_POINTER,
    G_TYPE_POINTER,
};
static ExchangeType exchange_type[DO_EXCHANGE_HANDBOOK_NO] =
{
    EXCHANGE_TYPE_HANDBOOK_PARTNER,
    EXCHANGE_TYPE_HANDBOOK_UNIT,
//    EXCHANGE_TYPE_HANDBOOK_MEMBER_FIRM,
    EXCHANGE_TYPE_HANDBOOK_FIRM,
};

static void do_link_dialog_init(DoLinkDialog *dialog)
{
/*    GtkWidget *action_area;

    action_area = gtk_dialog_get_action_area(GTK_DIALOG(dialog));
    gtk_container_border_width(GTK_CONTAINER(action_area), 5);
    gtk_box_set_spacing (GTK_BOX(action_area), 6);*/
}
static void do_link_dialog_view_row_activated_cb(GtkTreeView       *tree_view,
                                                        GtkTreePath       *path,
                                                        GtkTreeViewColumn *column,
                                                        DoLinkDialog *dialog);
//static void do_link_dialog_button_clicked(GtkButton *button, DoLinkDialog *dialog);
static GObject *do_link_dialog_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject    *object;
	GtkDialog  *dialog;
	GtkWidget  *box;
	GtkWidget  *hbox;
	GtkWidget  *label;
	GtkWidget  *scroll;
	GtkWidget  *view;
	GtkWidget  *w;
	GtkTreeViewColumn *col;
	GtkCellRenderer *renderer;
	DoLinkDialogPrivate *priv;

	object = G_OBJECT_CLASS (do_link_dialog_parent_class)->constructor(type, n_construct_properties, construct_params);
	dialog = GTK_DIALOG(object);
	priv = DO_LINK_DIALOG_GET_PRIVATE(object);

	gtk_window_set_icon_name(GTK_WINDOW(dialog), "domino");
	gtk_window_set_title(GTK_WINDOW(dialog), "");
    gtk_window_set_resizable (GTK_WINDOW(dialog), TRUE);

#if GTK_MAJOR_VERSION < 3
	box = gtk_vbox_new(FALSE, 0);
#else
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
#endif


    gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(dialog)), box, TRUE, TRUE,0);

    priv->tlabel=label = gtk_label_new("");
    gtk_label_set_markup(GTK_LABEL(label), "<b>Выберите соотвествие</b>");
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);

//    box = gtk_dialog_get_content_area(dialog);

    gtk_container_set_border_width( GTK_CONTAINER( box ), BORDER_SIZE);

#if GTK_MAJOR_VERSION < 3
	hbox = gtk_hbox_new(FALSE, 0);
#else
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
#endif


    gtk_box_pack_start(GTK_BOX(box), hbox, TRUE, TRUE, PADDING);

#if GTK_MAJOR_VERSION < 3
	box = gtk_vbox_new(FALSE, 0);
#else
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
#endif

    w =  gtk_alignment_new( 0.0f, 0.0f, 0.0f, 0.0f );
    gtk_widget_set_size_request( w, PADDING*2, 0u );

    gtk_box_pack_start(GTK_BOX(hbox), w, FALSE, FALSE,0);
    gtk_box_pack_start(GTK_BOX(hbox), box, TRUE, TRUE, 0);

    priv->label = label = gtk_label_new("");
    gtk_misc_set_alignment(GTK_MISC(priv->label), 0.0, 0.5);
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);

    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);


    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroll),
                                       GTK_SHADOW_ETCHED_IN);

    gtk_box_pack_start(GTK_BOX(box), scroll, TRUE, TRUE, PADDING);

    priv->view = view = gtk_tree_view_new();
    gtk_container_add(GTK_CONTAINER (scroll), view);
    g_signal_connect(view, "row-activated", G_CALLBACK(do_link_dialog_view_row_activated_cb), object);

    gtk_tree_view_set_search_column(GTK_TREE_VIEW(view), COL_NAME);


    priv->col = col = gtk_tree_view_column_new();
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_set_title (col, "Справочник 1с");
    gtk_tree_view_column_add_attribute (col, renderer, "markup", COL_DESC);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);

    gtk_container_set_border_width (GTK_CONTAINER (dialog), BORDER_SIZE);

    w = gtk_dialog_add_button(dialog,
                          "Все справочники", GTK_RESPONSE_HELP);
    gtk_widget_set_sensitive(w, FALSE);
    //g_signal_connect(w, "clicked", G_CALLBACK(do_link_dialog_button_clicked), object);

	w = gtk_dialog_add_button(dialog,
                          GTK_STOCK_OK, GTK_RESPONSE_OK);
    //g_signal_connect(w, "clicked", G_CALLBACK(do_link_dialog_button_clicked), object);

    priv->button=w=gtk_dialog_add_button(dialog,
                          "Пропустить", GTK_RESPONSE_NO);
    //g_signal_connect(w, "clicked", G_CALLBACK(do_link_dialog_button_clicked), object);
	w = gtk_dialog_add_button(dialog,
                          GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
    //g_signal_connect(w, "clicked", G_CALLBACK(do_link_dialog_button_clicked), object);
    gtk_widget_show_all(gtk_dialog_get_content_area(dialog));

    window_configure_init(GTK_WIDGET(dialog), priv->name, 600, 400);

    return object;
}

static void do_link_dialog_finalize (GObject *object)
{
    DoLinkDialogPrivate *priv = DO_LINK_DIALOG_GET_PRIVATE (object);

    g_free(priv->name);

	G_OBJECT_CLASS (do_link_dialog_parent_class)->finalize (object);
}

static void do_link_dialog_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    DoLinkDialogPrivate *priv = DO_LINK_DIALOG_GET_PRIVATE (object);

	switch (prop_id)
	{
	    case PROP_SAVED:
            g_value_set_boolean(value, priv->saved);
            break;
	}
	/* no readable properties */
	//g_assert_not_reached ();
}
static void do_link_dialog_set_1c_objects(DoLinkDialog *dialog, GSList *objs);
static void do_link_dialog_set_alias(DoLinkDialog *dialog, do_alias_t *alias);
static void do_link_dialog_set_handbook(DoLinkDialog *dialog, DoExchangeHandbook handbook);
static void do_link_dialog_set_handbook_key(DoLinkDialog *dialog, partner_key0_t *partner_key);
static void do_link_dialog_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
    DoLinkDialogPrivate *priv = DO_LINK_DIALOG_GET_PRIVATE (object);

	switch (prop_id)
	{
        case PROP_ALIAS:
            do_link_dialog_set_alias(DO_LINK_DIALOG(object), g_value_get_pointer(value));
            break;
        case PROP_1C_OBJS:
            do_link_dialog_set_1c_objects(DO_LINK_DIALOG(object), g_value_get_pointer(value));
            break;
        case PROP_HANDBOOK:
            do_link_dialog_set_handbook(DO_LINK_DIALOG(object), g_value_get_enum(value));
            break;
        case PROP_EMPTY:
            gtk_widget_set_sensitive(priv->button, g_value_get_boolean(value));
            break;
        case PROP_COMMENT: {
            gchar *str_;
            str_= g_strdup_printf("%s\n<b>Выберите соотвествие</b>",g_value_get_string(value));
	    gtk_label_set_markup(GTK_LABEL(priv->tlabel), str_);
            g_free(str_);
            break;
        }
        case PROP_HANDBOOK_KEY:
            do_link_dialog_set_handbook_key(DO_LINK_DIALOG(object), g_value_get_pointer(value));
            break;
	}
}

static void do_link_dialog_class_init (DoLinkDialogClass *klass)
{
    GObjectClass *o_class = G_OBJECT_CLASS (klass);

	o_class->constructor  = do_link_dialog_constructor;
	o_class->finalize     = do_link_dialog_finalize;
	o_class->get_property = do_link_dialog_get_property;
	o_class->set_property = do_link_dialog_set_property;


	g_type_class_add_private (o_class, sizeof (DoLinkDialogPrivate));

    g_object_class_install_property
        (o_class,
         PROP_ALIAS,
         g_param_spec_pointer("alias",
                      "domino alias",
                      "domino alias",
                      //DO_TYPE_VIEW ,
                       G_PARAM_WRITABLE));
    g_object_class_install_property
        (o_class,
         PROP_1C_OBJS,
         g_param_spec_pointer("objects-1c",
                      "1c exchange objects",
                      "1c exchange objects",
                      //DO_TYPE_VIEW ,
                       G_PARAM_WRITABLE));

    g_object_class_install_property
        (o_class,
         PROP_HANDBOOK_KEY,
         g_param_spec_pointer("domino-handbook-key",
                      "domino handbook key0",
                      "domino handbook key",
                      //DO_TYPE_VIEW ,
                       G_PARAM_WRITABLE));

    g_object_class_install_property
        (o_class,
         PROP_EMPTY,
         g_param_spec_boolean("empty",
                      "empty button",
                      "empty button",
                      FALSE,
                      //DO_TYPE_VIEW ,
                       G_PARAM_WRITABLE));

    g_object_class_install_property
        (o_class,
         PROP_SAVED,
         g_param_spec_boolean("saved",
                      "saved flag",
                      "saved flag",
                      FALSE,
                      //DO_TYPE_VIEW ,
                       G_PARAM_READABLE));

    g_object_class_install_property
        (o_class,
         PROP_COMMENT,
         g_param_spec_string("comment",
                      "comment",
                      "comment",
                      NULL,
                       G_PARAM_READWRITE));

    g_object_class_install_property
        (o_class,
         PROP_HANDBOOK,
         g_param_spec_enum("handbook",
                      "exchange handbook type",
                      "exchange handbook type",
                      DO_TYPE_EXCHANGE_HANDBOOK,
                      DO_EXCHANGE_HANDBOOK_PARTNER,
                       G_PARAM_WRITABLE | G_PARAM_CONSTRUCT));

}
GtkWidget  *do_link_dialog_new(do_alias_t *alias, GSList *objs_1c, DoExchangeHandbook handbook, partner_key0_t *partner_key)
{
	return g_object_new (DO_TYPE_LINK_DIALOG,
			     "alias", alias,
			     "objects-1c", objs_1c,
			     "handbook", handbook,
			     "domino-handbook-key", partner_key,
			     (gpointer) NULL);
}
static void do_link_dialog_init_data(DoLinkDialog *dialog);
static void do_link_dialog_set_1c_objects(DoLinkDialog *dialog, GSList *objs)
{
    DoLinkDialogPrivate *priv = DO_LINK_DIALOG_GET_PRIVATE (dialog);
    priv->objs_1c = objs;
    do_link_dialog_init_data(dialog);
}
static void do_link_dialog_set_handbook_key(DoLinkDialog *dialog, partner_key0_t *partner_key)
{
    DoLinkDialogPrivate *priv = DO_LINK_DIALOG_GET_PRIVATE (dialog);
    priv->partner_key = partner_key;
    do_link_dialog_init_data(dialog);
}
static void do_link_dialog_set_alias(DoLinkDialog *dialog, do_alias_t *alias)
{
    DoLinkDialogPrivate *priv = DO_LINK_DIALOG_GET_PRIVATE (dialog);
    priv->alias = alias;
    do_link_dialog_init_data(dialog);
}
static void do_link_dialog_set_handbook(DoLinkDialog *dialog, DoExchangeHandbook handbook)
{
    DoLinkDialogPrivate *priv = DO_LINK_DIALOG_GET_PRIVATE (dialog);
    GEnumValue  *enum_value;
    GEnumClass  *enum_class;

    g_return_if_fail (handbook < DO_EXCHANGE_HANDBOOK_NO);

    enum_class = G_ENUM_CLASS (g_type_class_peek (DO_TYPE_EXCHANGE_HANDBOOK));
    enum_value = g_enum_get_value(enum_class, handbook);
    priv->handbook = handbook;
    priv->init_handbook = TRUE;
    g_free(priv->name);
    priv->name = g_strdup_printf("DoLinkDialog-%s", enum_value->value_nick);
    do_link_dialog_init_data(dialog);
}
//static gboolean do_link_dialog_save(DoLinkDialog *dialog, gboolean empty);
static GtkListStore *stores[DO_EXCHANGE_HANDBOOK_NO] = {NULL,};
static gboolean do_link_dialog_make_store(DoLinkDialog *dialog);
static void do_link_dialog_init_data(DoLinkDialog *dialog)
{
    DoLinkDialogPrivate *priv = DO_LINK_DIALOG_GET_PRIVATE (dialog);
    if ( !priv->alias || !priv->init_handbook || !priv->objs_1c || !priv->partner_key )
        return;

    if ( !stores[priv->handbook] ) {
        if ( !do_link_dialog_make_store(dialog) )
            return;
    }
    gtk_tree_view_set_model(GTK_TREE_VIEW(priv->view), GTK_TREE_MODEL(stores[priv->handbook]));
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(stores[priv->handbook]), COL_NAME, GTK_SORT_ASCENDING);
    gtk_tree_view_column_set_sort_column_id (priv->col, COL_NAME);
    gtk_tree_view_column_set_sort_order(priv->col, GTK_SORT_ASCENDING);

    int status;
    do_alias_t *alias = priv->alias;
    gchar *name;
    gchar *markup;
    gchar *name1 = NULL;
    GSList *cols = NULL;
    GtkTreePath *path;
    MiscGtkTreeModelFindColumn column;

    switch ( priv->handbook ) {
        case DO_EXCHANGE_HANDBOOK_PARTNER: {
            partner_rec_t partner;
            partner_key0_t *partner_key0;
            partner_key0 = priv->partner_key;
            status = do_partner_get0(alias, &partner, partner_key0, DO_GET_EQUAL);
            if ( status == DO_ERROR )
                return;
            if ( status == DO_KEY_NOT_FOUND ) {
                gchar *str;
                str = do_util_partner_key0_to_str(alias, partner_key0);
                ERROR_AND_SHOW("Не найден партнер \"%s\"", str);
                g_free(str);
                return;
            }
            name = do_text(alias, partner.data.name);
            name1 = do_partner_param(alias, &partner, "ОПФ");

            column.text = name;
            column.column = COL_NAME;
            column.column_cache = COL_NAME_LIST;
            cols = g_slist_append(cols, g_memdup(&column, sizeof(column)));

            column.text = name1;
            column.column = COL_NAME1;
            column.column_cache = COL_NAME1_LIST;
            cols = g_slist_append(cols, g_memdup(&column, sizeof(column)));

            break;
        }
        case DO_EXCHANGE_HANDBOOK_SKLAD:
        //case DO_EXCHANGE_HANDBOOK_CASHIER:
        case DO_EXCHANGE_HANDBOOK_FIRM:        {
            partner_rec_t partner;
            partner_key0_t *partner_key0;
            partner_key0 =  priv->partner_key;
            status = do_partner_get0(alias, &partner, partner_key0, DO_GET_EQUAL);
            if ( status == DO_ERROR )
                return;
            if ( status == DO_KEY_NOT_FOUND ) {
                gchar *str;
                str = do_util_partner_key0_to_str(alias, partner_key0);
                ERROR_AND_SHOW("Не найден партнер \"%s\"", str);
                g_free(str);
                return;
            }
            name = do_text(alias, partner.data.name);
            column.text = name;
            column.column = COL_NAME;
            column.column_cache = COL_NAME_LIST;
            cols = g_slist_append(cols, g_memdup(&column, sizeof(column)));
            break;
        }
        /*case DO_EXCHANGE_HANDBOOK_SKLAD: {
            sklad_rec_t sklad;
            sklad_key0_t *sklad_key0;
            sklad_key0 = (void*)priv->handbook_key;
            status = do_sklad_get0(alias, &sklad, sklad_key0, DO_GET_EQUAL);
            if ( status == DO_ERROR )
                return;
            if ( status == DO_KEY_NOT_FOUND ) {
                gchar *str;
                str = do_text(alias, sklad_key0->code);
                ERROR_AND_SHOW("Не найден склад \"%s\"", str);
                g_free(str);
                return;
            }
            name = do_text(alias, sklad.data.name);
            column.text = name;
            column.column = COL_NAME;
            column.column_cache = COL_NAME_LIST;
            cols = g_slist_append(cols, g_memdup(&column, sizeof(column)));
            break;
        }*/
        case DO_EXCHANGE_HANDBOOK_NO:
            name = g_strdup("");
            break;
    }

    markup = g_markup_printf_escaped("%s%s<i>%s</i>", name, (!name1) ? "" : "\n", (!name1) ? "" : name1);

    gtk_label_set_markup(GTK_LABEL(priv->label), markup);

    path = misc_find_by_name_from_model(GTK_TREE_MODEL(stores[priv->handbook]), cols);
    g_slist_foreach(cols, (GFunc)g_free, NULL);
    g_slist_free(cols);

    if ( path ) {
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->view), path, 0, FALSE);
        gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(priv->view), path, NULL,  TRUE, 0.5, 0.0);
        gtk_tree_path_free(path);
    }
    g_free(name);
    g_free(name1);
    g_free(markup);
}

static gboolean do_link_dialog_make_store(DoLinkDialog *dialog)
{
    DoLinkDialogPrivate *priv = DO_LINK_DIALOG_GET_PRIVATE (dialog);
    GtkTreeIter iter;

    stores[priv->handbook] = gtk_list_store_newv(COL_NO, col_types);
    gtk_list_store_append(stores[priv->handbook], &iter);
    gtk_list_store_set(stores[priv->handbook], &iter,
                       COL_GUID, CONST_EMPTY_GUID,
                       COL_DESC, "<i>(Пусто)</i>",
                       -1);

    GSList *l;
    for ( l=priv->objs_1c; l; l=l->next ) {
        ExchangeObject *obj;
        gchar *desc_markup;

        obj = l->data;

        if ( obj->type != exchange_type[priv->handbook] )
            continue;
        if ( obj->key->group )
            continue;

        ExchangeProperty *name,*name1;
        name = exchange_object_get_property(obj, ELEM_1C_NAME);
        name1 = NULL;

        if ( !name || name->type != EXCHANGE_TYPE_CONST_STRING || name->empty ) {
            OBJ_MY_ERROR(obj, "Неопределен или неправильный тип \"%s\"", ELEM_1C_NAME);
            return FALSE;
        }
        switch ( priv->handbook ) {
            case DO_EXCHANGE_HANDBOOK_PARTNER: {
                name1 = exchange_object_get_property(obj, ELEM_1C_NAME_FULL);
                if ( !name1 || name1->type != EXCHANGE_TYPE_CONST_STRING ) {
                    OBJ_MY_ERROR(obj, "Неопределен или неправильный тип \"%s\"", ELEM_1C_NAME_FULL);
                    return FALSE;
                }
                desc_markup = g_markup_printf_escaped("<b>%s</b>\n%s", g_value_get_string(&name->value),
                                                                  name1->empty ? "" : g_value_get_string(&name1->value));
                /*desc_markup = g_strdup_printf("<b>%s</b>\n%s", g_value_get_string(&name->value),
                                                                  name1->empty ? "" : g_value_get_string(&name1->value));*/
                break;
            }
            case DO_EXCHANGE_HANDBOOK_SKLAD: {
                name1 = exchange_object_get_property(obj, ELEM_1C_UNIT_KIND);
                if ( !name1 || name1->type != EXCHANGE_TYPE_LIST_UNIT_KIND ) {
                    OBJ_MY_ERROR(obj, "Неопределен или неправильный тип \"%s\"", ELEM_1C_UNIT_KIND);
                    return FALSE;
                }
                desc_markup = g_markup_printf_escaped("<b>%s</b>\n%s", g_value_get_string(&name->value),
                                                                  name1->empty ? "" : g_value_get_string(&name1->value));
/*
                ExchangeProperty *owner_prop;
                ExchangeKey *owner_key;
                ExchangeObject *owner_obj;

                owner_prop = exchange_object_get_property(obj, ELEM_1C_OWNER);
                if ( !owner_prop ) {
                    desc_markup = g_markup_printf_escaped("<b>%s</b>\n", g_value_get_string(&name->value));
                }
                else {

                    if ( !owner_prop || owner_prop->type != EXCHANGE_TYPE_HANDBOOK_FIRM ) {
                        OBJ_MY_ERROR(obj, "Неопределен или неправильный тип \"%s\"", ELEM_1C_OWNER);
                        return FALSE;
                    }
                    owner_key = g_value_get_pointer(&owner_prop->value);

                    owner_obj = find_obj_by_key(priv->objs_1c, owner_key);
                    if ( !owner_obj ) {
                        PROP_MY_ERROR(owner_prop, "Не найден объект с %s %s", ELEM_1C_GUID, owner_key->guid);
                        return FALSE;

                    }
                    name1 = exchange_object_get_property(owner_obj, ELEM_1C_NAME);
                    if ( !name1 || name1->type != EXCHANGE_TYPE_CONST_STRING || name1->empty ) {
                        OBJ_MY_ERROR(owner_obj, "Неопределен или неправильный тип \"%s\"", ELEM_1C_NAME);
                        return FALSE;
                    }
                    desc_markup = g_markup_printf_escaped("<b>%s</b>\n%s", g_value_get_string(&name->value), g_value_get_string(&name1->value));
                }*/
                break;
            }
            case DO_EXCHANGE_HANDBOOK_FIRM:
            //case DO_EXCHANGE_HANDBOOK_CASHIER:
                desc_markup = g_markup_printf_escaped("%s", g_value_get_string(&name->value));
                break;
            case DO_EXCHANGE_HANDBOOK_NO:
                desc_markup = g_strdup("<Неправильный тип>");

        }

        gtk_list_store_append(stores[priv->handbook], &iter);
        gtk_list_store_set(stores[priv->handbook], &iter,
                           COL_GUID, obj->key->guid,
                           COL_CODE, obj->key->code,
                           COL_NAME, g_value_get_string(&name->value),
                           COL_NAME1, (!name1 || name1->empty) ? "" : g_value_get_string(&name1->value),
                           COL_DESC, desc_markup,
                           -1);
        g_free(desc_markup);
    }
    return TRUE;

}
static void do_link_dialog_view_row_activated_cb(GtkTreeView       *tree_view,
                                                        GtkTreePath       *path,
                                                        GtkTreeViewColumn *column,
                                                        DoLinkDialog *dialog)
{
    gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
}

static gboolean do_link_dialog_partner_save(DoLinkDialog *dialog, const gchar *guid);
//static gboolean do_link_dialog_sklad_save(DoLinkDialog *dialog, const gchar *guid);

gboolean do_link_dialog_save(DoLinkDialog *dialog, gboolean empty)
{
    DoLinkDialogPrivate *priv = DO_LINK_DIALOG_GET_PRIVATE (dialog);
    const gchar *guid;
    gboolean res = TRUE;
    if ( empty )
        guid = CONST_EMPTY_GUID;
    else {
        GtkTreePath  *path = NULL;
        GtkTreeModel *model;
        model = gtk_tree_view_get_model(GTK_TREE_VIEW(priv->view));
        gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->view), &path, NULL);
        if ( path ) {
            GValue value = {0,};
            GtkTreeIter iter;
            gtk_tree_model_get_iter(model, &iter, path);
            gtk_tree_model_get_value(model, &iter, COL_GUID, &value);
            guid = g_value_get_string(&value);
        }
        if ( !path || (!empty && !g_strcmp0(guid, CONST_EMPTY_GUID)) ) {
            GtkWidget *d;
            d = gtk_message_dialog_new(GTK_WINDOW(dialog), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                       "%s", "Необходимо выбрать из списка");
            gtk_window_set_icon_name(GTK_WINDOW(d), "domino");
            gtk_dialog_run(GTK_DIALOG(d));
            gtk_widget_destroy(d);
            res = FALSE;
        }
        gtk_tree_path_free(path);
    }
    if ( !res )
        return FALSE;

    switch ( priv->handbook ) {
        case DO_EXCHANGE_HANDBOOK_FIRM:
//        case DO_EXCHANGE_HANDBOOK_CASHIER:
        case DO_EXCHANGE_HANDBOOK_PARTNER:
        case DO_EXCHANGE_HANDBOOK_SKLAD:
            res = do_link_dialog_partner_save(dialog, guid);
            break;
/*        case DO_EXCHANGE_HANDBOOK_SKLAD:
            res = do_link_dialog_sklad_save(dialog, guid);
            break;*/
        case DO_EXCHANGE_HANDBOOK_NO:
            res = FALSE;
            break;
    }
    return res;
};
static gboolean do_link_dialog_partner_save(DoLinkDialog *dialog, const gchar *guid)
{
    DoLinkDialogPrivate *priv = DO_LINK_DIALOG_GET_PRIVATE (dialog);
    partner_key0_t *partner_key0;
    partner_data_key0_t partner_data_key0;
    partner_data_rec_t partner_data;
    do_alias_t *alias;
    int status;
    const gchar *account = NULL;
    ExchangeObject *obj;
    ExchangeProperty *name = NULL;

    if ( g_strcmp0(guid, CONST_EMPTY_GUID) ) {
        obj = find_exchange_object_by_guid(priv->objs_1c, guid);
        if ( !obj ) {
            MY_ERROR("Объект с %s \"%s\" не найден. ", ELEM_1C_GUID, guid);
            return FALSE;
        }
        name = exchange_object_get_property(obj, ELEM_1C_NAME);
        if ( !name || name->type != EXCHANGE_TYPE_CONST_STRING || name->empty ) {
            OBJ_MY_ERROR(obj, "Неопределен или неправильный тип \"%s\"", ELEM_1C_NAME);
            return FALSE;
        }

        if ( priv->handbook == DO_EXCHANGE_HANDBOOK_PARTNER ) {
            gboolean many;
            account = get_partner_default_account(priv->objs_1c, obj, &many);
            if ( !account ) {
                GtkWidget *d;
                gint res;
                if ( many )
                    d = gtk_message_dialog_new(GTK_WINDOW(dialog), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING, GTK_BUTTONS_YES_NO,
                                           "У контрагента %s не заполнен основной. Продолжить?", g_value_get_string(&name->value));
                else
                    d = gtk_message_dialog_new(GTK_WINDOW(dialog), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING, GTK_BUTTONS_YES_NO,
                                           "У контрагента %s нет счета. Продолжить?", g_value_get_string(&name->value));
                gtk_window_set_icon_name(GTK_WINDOW(d), "domino");
                res = gtk_dialog_run(GTK_DIALOG(d));
                gtk_widget_destroy(d);
                if ( res != GTK_RESPONSE_YES )
                    return FALSE;
            }
        }
    }

    alias = priv->alias;
    partner_key0 = priv->partner_key;

    do_cpy(partner_data_key0.g_code, partner_key0->g_code);
    partner_data_key0.code = partner_key0->code;
    partner_data_key0.type = PARTNER_DATA_DOMINO_TYPE;
    partner_data_key0.number = firm_guid_number;

    status = do_partner_data_get0(alias, &partner_data, &partner_data_key0, DO_GET_EQUAL);
    if ( status == DO_ERROR )
        return FALSE;
    time_t now = time(NULL);
    struct tm tm;
    tm = *localtime(&now);

    if ( status == DO_KEY_NOT_FOUND ) {
        do_cpy(partner_data.data.g_code, partner_data_key0.g_code);
        partner_data.data.code = partner_data_key0.code;
        partner_data.data.type = partner_data_key0.type;
        partner_data.data.number = partner_data_key0.number;
    }
    do_date_set(&partner_data.data.date, tm);
    do_time_set(&partner_data.data.time, tm);
    do_partner_data_params_clear(alias, &partner_data);

    do_partner_data_param_set(alias, &partner_data, "GUID", guid);
    if ( name )
        do_partner_data_param_set(alias, &partner_data, "NAME", g_value_get_string(&name->value));
    if ( account )
        do_partner_data_param_set(alias, &partner_data, "DEFAULT_ACCOUNT", account);

    if ( status == DO_KEY_NOT_FOUND )
        status = do_partner_data_insert(alias, &partner_data);
    else
        status = do_partner_data_update(alias, &partner_data);

    if ( status != DO_OK )
        return FALSE;

    do_protocol_partner_add_key(alias, partner_key0, DO_PROTOCOL_ACTION_CHANGE);

    priv->saved = TRUE;

    return TRUE;


}
/*
static gboolean do_link_dialog_sklad_save(DoLinkDialog *dialog, const gchar *guid)
{
    DoLinkDialogPrivate *priv = DO_LINK_DIALOG_GET_PRIVATE (dialog);

    sklad_key0_t *sklad_key0;
    sklad_rec_t  sklad;
    do_alias_t *alias;
    ExchangeObject *obj;
    ExchangeProperty *name = NULL;

    if ( g_strcmp0(guid, CONST_EMPTY_GUID) ) {
        obj = find_exchange_object_by_guid(priv->objs_1c, guid);
        if ( !obj ) {
            MY_ERROR("Объект с %s \"%s\" не найден. ", ELEM_1C_GUID, guid);
            return FALSE;
        }
        name = exchange_object_get_property(obj, ELEM_1C_NAME);
        if ( !name || name->type != EXCHANGE_TYPE_CONST_STRING || name->empty ) {
            OBJ_MY_ERROR(obj, "Неопределен или неправильный тип \"%s\"", ELEM_1C_NAME);
            return FALSE;
        }

    }

    alias = priv->alias;
    sklad_key0 = (void*)priv->handbook_key;

    if ( do_sklad_get0(alias, &sklad, sklad_key0, DO_GET_EQUAL) != DO_OK )
        return FALSE;


    do_sklad_param_set(alias, &sklad, "GUID_1C", guid);
    if ( name )
        do_sklad_param_set(alias, &sklad, "NAME_1C", g_value_get_string(&name->value));

    if ( do_sklad_update(alias, &sklad) != DO_OK )
        return FALSE;
    gchar *code;
    code = do_text(alias, sklad_key0->code);

    if ( !do_protocol_add(alias, DO_PROTOCOL_OBJECT_SKLAD, code, DO_PROTOCOL_ACTION_CHANGE, NULL) ) {
        g_free(code);
        return FALSE;
    }
    g_free(code);
    priv->saved = TRUE;

    return TRUE;
}
*/
