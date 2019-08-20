

#include <string.h>
#include <gtk/gtk.h>

#include "do_purchase_dialog.h"
#include "do_icons.h"
#include "do_entry_date_box.h"
#include "do_entry.h"
#include "do_entry_calc.h"
#include "do_unit_box.h"
#include "do_utilx.h"
#include "do_validate.h"
#include "do_enum_box.h"
#include "do_enum_value.h"
#include "domino.h"
#include "hig.h"
#include <dolib.h>
#include <domino.h>

enum
{
	PROP_0,
	PROP_UNITS,
	PROP_DATE1,
	PROP_DATE2,
	PROP_MAIN_WINDOW,
	PROP_NO,
};

#define DO_PURCHASE_DIALOG_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_PURCHASE_DIALOG, DoPurchaseDialogPrivate))
#define DO_TYPE_PURCHASE_DIALOG_FLAGS do_purchase_dialog_flags_get_type()


#define OBJECT_ROOT_PATH "PurchaseDialog"
#define DEFAULT_WINDOW_POSITION "center-on-parent"
#define DEFAULT_WINDOW_SIZE ""
#define PURCHASE_TEMPLATE "purchase"

enum
{
	UNITS_ENTRY,
	DATE1_ENTRY,
	DATE2_ENTRY,
	N_ENTRIES
};

struct _DoPurchaseDialogPrivate
{

	GtkWidget *widget[N_ENTRIES];
	GtkWidget *entry[N_ENTRIES];
	GtkWidget *main_window;
	GDate date1, date2;

};


G_DEFINE_TYPE (DoPurchaseDialog, do_purchase_dialog, GTK_TYPE_DIALOG)

static GtkWidget *add_field(DoPurchaseDialog *dialog, gint id, GtkWidget *widget);

static void entry_activate(GtkWidget *entry, DoPurchaseDialog *dialog);
static void do_purchase_dialog_read_last_value(DoPurchaseDialog *dialog);
static void do_purchase_dialog_save_value(DoPurchaseDialog *dialog);
static gboolean do_purchase_dialog_valid(DoPurchaseDialog *dialog);

static void do_purchase_dialog_init(DoPurchaseDialog *dialog)
{

}
static void do_purchase_dialog_ok(GtkButton *button, DoPurchaseDialog *dialog)
{
    if ( do_purchase_dialog_valid(dialog) ) {
        do_purchase_dialog_save_value(dialog);
        gtk_dialog_accept(GTK_DIALOG(dialog));
    }
}

static GObject *do_purchase_dialog_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject *object;
	GtkWindow *window;
	GtkDialog *dialog;
	DoPurchaseDialog *purchase_dialog;
	//DoPurchaseDialogPrivate *priv;
	GtkWidget *vbox;
    GtkWidget *l;
    GtkWidget *t;
    GtkAccelGroup *accel_group;

	int row = 0;
	object = G_OBJECT_CLASS (do_purchase_dialog_parent_class)->constructor(type, n_construct_properties, construct_params);

    //priv = DO_PURCHASE_DIALOG_GET_PRIVATE(object);
	window = GTK_WINDOW (object);
	dialog = GTK_DIALOG (object);
	vbox   = gtk_dialog_get_content_area(dialog);
	purchase_dialog = DO_PURCHASE_DIALOG(object);

    accel_group = gtk_accel_group_new ();
    gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);


	gtk_box_set_spacing (GTK_BOX (vbox), 2); /* Message has 24, we want 12 = 2 + 2 * 5 */

	gtk_window_set_icon_name (window, DO_STOCK_DOMINO_APPS);
	gtk_window_set_title (window, "Просмотр приходов за период");

    g_signal_connect (window, "configure-event",
                      G_CALLBACK (do_window_configure_event_cb), OBJECT_ROOT_PATH);

    //gtk_dialog_set_has_separator(dialog, FALSE);

    t = hig_workarea_create();
    //gtk_table_set_homogeneous(GTK_TABLE(t), TRUE);

	gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (t), FALSE, FALSE, 0);

    hig_workarea_add_section_title(t, &row, "Параметры  ");

    l = add_field(purchase_dialog, UNITS_ENTRY,
               do_unit_box_new());
    hig_workarea_add_row(t, &row, "Аптеки:", l, NULL);

	l = add_field(purchase_dialog, DATE1_ENTRY, do_entry_date_box_new(DO_ENTRY_DATE_FLAGS_DEFAULT));
    hig_workarea_add_row(t, &row, "Начало:", l, NULL);

	l = add_field(purchase_dialog, DATE2_ENTRY, do_entry_date_box_new(DO_ENTRY_DATE_FLAGS_DEFAULT));
    hig_workarea_add_row(t, &row, "Конец:", l, NULL);


    hig_workarea_finish( t, &row );


	l = gtk_dialog_add_button (dialog, "Отмена", GTK_RESPONSE_CANCEL);
    /*g_signal_connect (l, "clicked",
                      G_CALLBACK (do_purchase_dialog_cancel), object);*/
	l = gtk_dialog_add_button (dialog, "Ok", GTK_RESPONSE_ACCEPT);
    g_signal_connect (l, "clicked",
                      G_CALLBACK (do_purchase_dialog_ok), object);



	//gtk_dialog_set_default_response (dialog, GTK_RESPONSE_ACCEPT);
	do_purchase_dialog_read_last_value(purchase_dialog);

    DOMINO_PROFILE_OBJECT_INIT(G_OBJECT(window), OBJECT_ROOT_PATH,
              "window-position", DEFAULT_WINDOW_POSITION,
              "window-size", DEFAULT_WINDOW_SIZE,
               NULL);

	gtk_widget_show_all (vbox);

	return object;
}

static void do_purchase_dialog_finalize (GObject *object)
{
//	DoPurchaseDialogPrivate *priv = DO_PURCHASE_DIALOG_GET_PRIVATE (object);
	G_OBJECT_CLASS (do_purchase_dialog_parent_class)->finalize (object);
}

static void do_purchase_dialog_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoPurchaseDialogPrivate *priv = DO_PURCHASE_DIALOG_GET_PRIVATE (object);

	switch (prop_id)
	{

        case PROP_UNITS: {
            g_object_set(priv->widget[UNITS_ENTRY], "units", g_value_get_string(value), NULL);
            break;
        }
        case PROP_DATE1: {
            GDate *date;
            date = g_value_get_object(value);

            g_object_set(priv->entry[DATE1_ENTRY],"year", date->year,
                                               "month", date->month,
                                              "day", date->day,
                                            NULL);

        }
        case PROP_DATE2: {
            GDate *date;
            date = g_value_get_object(value);

            g_object_set(priv->entry[DATE1_ENTRY],"year", date->year,
                                               "month", date->month,
                                              "day", date->day,
                                            NULL);

        }

        case PROP_MAIN_WINDOW:
            priv->main_window = g_value_get_object(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}

}
static void do_purchase_dialog_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoPurchaseDialogPrivate *priv = DO_PURCHASE_DIALOG_GET_PRIVATE (object);
    switch (prop_id)
    {

        case PROP_UNITS: {
            gchar *val;
            g_object_get(priv->widget[UNITS_ENTRY], "units", &val, NULL);
            g_value_set_string(value, val);
            break;
        }
        case PROP_DATE1: {
            //GDate *date;
            int year,month,day;
            g_object_get(priv->entry[DATE1_ENTRY], "year", &year,
                                            "month", &month,
                                            "day", &day,
                                            NULL);
            g_date_set_dmy(&priv->date1, day, month, year);
            g_value_set_boxed(value, &priv->date1);
            break;
        }
        case PROP_DATE2: {
            //GDate *date;
            int year,month,day;
            g_object_get(priv->entry[DATE2_ENTRY], "year", &year,
                                            "month", &month,
                                            "day", &day,
                                            NULL);
            g_date_set_dmy(&priv->date2,day, month, year);
            g_value_set_boxed(value, &priv->date2);
            break;
        }
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;

    }
}
static void do_purchase_dialog_class_init (DoPurchaseDialogClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor = do_purchase_dialog_constructor;
	object_class->finalize = do_purchase_dialog_finalize;
	object_class->get_property = do_purchase_dialog_get_property;
	object_class->set_property = do_purchase_dialog_set_property;

	g_type_class_add_private (object_class, sizeof (DoPurchaseDialogPrivate));

    g_object_class_install_property
        (object_class,
         PROP_UNITS,
         g_param_spec_string("units",
                      "Список подразделений",
                      "Список подразделений",
                      NULL,
                       G_PARAM_READWRITE ));

    g_object_class_install_property (object_class,
				   PROP_DATE1,
				   g_param_spec_boxed("date1",
 						      "Начальная дата",
 						      "Дата начальная",
 						      G_TYPE_DATE,
 						      G_PARAM_READWRITE));
    g_object_class_install_property (object_class,
				   PROP_DATE2,
				   g_param_spec_boxed("date2",
 						      "Конечная дата",
 						      "Дата конечная",
 						      G_TYPE_DATE,
 						      G_PARAM_READWRITE));
    g_object_class_install_property (object_class,
				   PROP_MAIN_WINDOW,
				   g_param_spec_object  ("main-window",
 						      "Главное окно",
 						      "Главное окно",
 						      GTK_TYPE_WIDGET,
 						      G_PARAM_WRITABLE));
}
GtkWidget *do_purchase_dialog_new (GtkWidget *parent)
{
    GtkWidget *object;
    object = g_object_new (DO_TYPE_PURCHASE_DIALOG,
			     "main-window", parent,
			     (gpointer) NULL);
    return object;
}

static void do_purchase_dialog_read_last_value(DoPurchaseDialog *dialog)
{
    DoPurchaseDialogPrivate *priv = DO_PURCHASE_DIALOG_GET_PRIVATE(dialog);
    gchar *v_d_s_year;
    gchar *v_d_s_month;
    gchar *v_d_s_day;
    gchar *v_d_e_year;
    gchar *v_d_e_month;
    gchar *v_d_e_day;
    gchar *v_units;

    DOMINO_PROFILE_GET(OBJECT_ROOT_PATH,
                                        "DateStartYear", &v_d_s_year,
                                        "DateStartMonth", &v_d_s_month,
                                        "DateStartDay", &v_d_s_day,
                                        "DateEndYear", &v_d_e_year,
                                        "DateEndMonth", &v_d_e_month,
                                        "DateEndDay", &v_d_e_day,
                                        "Units", &v_units,
                                        NULL);
    if ( v_d_s_year && atoi(v_d_s_year) > 0 )
        g_object_set(priv->entry[DATE1_ENTRY], "year", atoi(v_d_s_year),
                                            "month", atoi(v_d_s_month),
                                            "day", atoi(v_d_s_day),
                                            NULL);
    if ( v_d_s_year && atoi(v_d_s_year) > 0 )
        g_object_set(priv->entry[DATE2_ENTRY], "year", atoi(v_d_e_year),
                                            "month", atoi(v_d_e_month),
                                            "day", atoi(v_d_e_day),
                                            NULL);
    if ( v_units ) {
        g_object_set(priv->widget[UNITS_ENTRY], "units", v_units, NULL);
    }
}

static GtkWidget *add_field(DoPurchaseDialog *dialog, gint id, GtkWidget *widget)
{
	DoPurchaseDialogPrivate *priv = DO_PURCHASE_DIALOG_GET_PRIVATE (dialog);
	g_assert (id < N_ENTRIES);
	priv->widget[id] = widget;
	if (GTK_IS_ENTRY(widget)) {
        g_signal_connect(widget, "activate", G_CALLBACK(entry_activate), dialog);
        priv->entry[id] = widget;
	}
    else
	if (DO_IS_ENTRY_DATE(widget)) {
        priv->entry[id] = widget;
//	    priv->entry[id] = do_entry_date_entry(DO_ENTRY_DATE(widget));
        g_signal_connect(priv->entry[id], "activate", G_CALLBACK(entry_activate), dialog);
	}
    else
	if (DO_IS_ENTRY_BOX(widget)) {
        priv->entry[id] = do_entry_box_entry(DO_ENTRY_BOX(widget));
//	    priv->entry[id] = do_entry_date_entry(DO_ENTRY_DATE(widget));
        g_signal_connect(priv->entry[id], "activate", G_CALLBACK(entry_activate), dialog);
	}
    else
    if (GTK_IS_BUTTON(widget))
        g_signal_connect(widget, "activate", G_CALLBACK(entry_activate), dialog);
    else
	if (GTK_IS_COMBO_BOX(widget)) {
        priv->entry[id] = gtk_bin_get_child (GTK_BIN (widget));
        if (priv->entry[id])
            g_signal_connect(priv->entry[id], "activate", G_CALLBACK(entry_activate), dialog);
	}
    return widget;
}

static gboolean do_purchase_dialog_valid(DoPurchaseDialog *dialog)
{
	DoPurchaseDialogPrivate *priv = DO_PURCHASE_DIALOG_GET_PRIVATE (dialog);
    gint i;
    for (i = 0; i < N_ENTRIES; i++)
        if (DO_IS_VALIDATE(priv->entry[i]) && !do_validate_valid(DO_VALIDATE(priv->entry[i])))
            return FALSE;
    return TRUE;
}

static void entry_activate(GtkWidget *entry, DoPurchaseDialog *dialog)
{
	DoPurchaseDialogPrivate *priv = DO_PURCHASE_DIALOG_GET_PRIVATE (dialog);
    gint i;
    gboolean valid = TRUE;
    for (i = 0; i < N_ENTRIES; i++)
        if (DO_IS_VALIDATE(priv->entry[i]) && !do_validate_valid(DO_VALIDATE(priv->entry[i]))) {
            valid = FALSE;
            gtk_widget_grab_focus(priv->entry[i]);
            break;
        }
    if ( valid )
        do_purchase_dialog_ok(NULL, dialog);
}

static void do_purchase_dialog_save_value(DoPurchaseDialog *dialog)
{
    DoPurchaseDialogPrivate *priv = DO_PURCHASE_DIALOG_GET_PRIVATE(dialog);
    gint value_date_start_year;
    gint value_date_start_month;
    gint value_date_start_day;
    gint value_date_end_year;
    gint value_date_end_month;
    gint value_date_end_day;
    gchar *v_d_s_year;
    gchar *v_d_s_month;
    gchar *v_d_s_day;
    gchar *v_d_e_year;
    gchar *v_d_e_month;
    gchar *v_d_e_day;
    gchar *units;
    g_object_get(priv->widget[UNITS_ENTRY], "units", &units, NULL);
    g_object_get(priv->entry[DATE1_ENTRY], "year", &value_date_start_year,
                                            "month", &value_date_start_month,
                                            "day", &value_date_start_day,
                                            NULL);
    g_object_get(priv->entry[DATE2_ENTRY], "year", &value_date_end_year,
                                            "month", &value_date_end_month,
                                            "day", &value_date_end_day,
                                            NULL);
    v_d_s_year = g_strdup_printf("%d", value_date_start_year);
    v_d_s_month = g_strdup_printf("%d", value_date_start_month);
    v_d_s_day = g_strdup_printf("%d", value_date_start_day);
    v_d_e_year = g_strdup_printf("%d", value_date_end_year);
    v_d_e_month = g_strdup_printf("%d", value_date_end_month);
    v_d_e_day = g_strdup_printf("%d", value_date_end_day);
    DOMINO_PROFILE_SET(OBJECT_ROOT_PATH, "LastValue",
                                        "DateStartYear", v_d_s_year,
                                        "DateStartMonth", v_d_s_month,
                                        "DateStartDay", v_d_s_day,
                                        "DateEndYear", v_d_e_year,
                                        "DateEndMonth", v_d_e_month,
                                        "DateEndDay", v_d_e_day,
                                        "Units", units,
                                        NULL);
//    g_object_unref(units);
    g_free(v_d_s_year);

    g_free(v_d_s_month);
    g_free(v_d_s_day);
    g_free(v_d_e_year);
    g_free(v_d_e_month);
    g_free(v_d_e_day);
}
