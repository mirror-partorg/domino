

#include "do_entry_date_box.h"
#include "do_icons.h"

enum
{
	PROP_0,
	PROP_FLAGS,
	PROP_YEAR,
	PROP_MONTH,
	PROP_DAY,
	PROP_ENTRY,
};

#define DO_ENTRY_DATE_BOX_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_ENTRY_DATE_BOX, DoEntryDateBoxPrivate))


static void button_clicked(GtkWidget *button, DoEntryDateBox *box);
static void do_entry_date_box_validate_init (DoValidateIface *iface);


struct _DoEntryDateBoxPrivate
{
    GtkWidget *entry;
	DoEntryDateFlags flags;
};

G_DEFINE_TYPE_WITH_CODE (DoEntryDateBox, do_entry_date_box, DO_TYPE_ENTRY_BOX,
                       G_IMPLEMENT_INTERFACE (DO_TYPE_VALIDATE,
						do_entry_date_box_validate_init));

static void do_entry_date_box_init(DoEntryDateBox *dialog)
{
}
static gboolean do_entry_date_box_valid(DoValidate *iface)
{
	DoEntryDateBoxPrivate *priv = DO_ENTRY_DATE_BOX_GET_PRIVATE (iface);

	return do_validate_valid(DO_VALIDATE(priv->entry));

}

static void do_entry_date_box_validate_init (DoValidateIface *iface)
{
    iface->valid = do_entry_date_box_valid;
}
static void
signal_calendar_event(GtkWidget *widget,GtkDialog *box)
{
  gtk_dialog_response(box, GTK_RESPONSE_ACCEPT);
}

static GObject *do_entry_date_box_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject   *object;
	GtkWidget  *button;
    DoEntryDateBoxPrivate *priv;

	object = G_OBJECT_CLASS (do_entry_date_box_parent_class)->constructor(type, n_construct_properties, construct_params);
    priv = DO_ENTRY_DATE_BOX_GET_PRIVATE (object);

	button = do_entry_box_button(DO_ENTRY_BOX(object));
	priv->entry = do_entry_box_entry(DO_ENTRY_BOX(object));
    g_signal_connect(button, "clicked", G_CALLBACK(button_clicked), object);

	return object;
}

static void do_entry_date_box_finalize (GObject *object)
{
//	DoEntryDateBoxPrivate *priv = DO_ENTRY_DATE_BOX_GET_PRIVATE (object);
	G_OBJECT_CLASS (do_entry_date_box_parent_class)->finalize (object);
}

static void do_entry_date_box_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoEntryDateBoxPrivate *priv = DO_ENTRY_DATE_BOX_GET_PRIVATE (object);


	switch (prop_id)
	{
		case PROP_FLAGS:
			priv->flags = g_value_get_flags (value);
			break;
        case PROP_YEAR: {
            gint year;
            year = g_value_get_int (value);
            g_object_set(priv->entry, "year", year, NULL);
            break;
        }
        case PROP_MONTH: {
            gint month;
            month = g_value_get_int (value);
            g_object_set(priv->entry, "month", month, NULL);
            break;
        }
        case PROP_DAY: {
            gint day;
            day = g_value_get_int (value);
            g_object_set(priv->entry, "day", day, NULL);
            break;
        }
	}
}
static void do_entry_date_box_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoEntryDateBoxPrivate *priv = DO_ENTRY_DATE_BOX_GET_PRIVATE (object);
    switch (prop_id)
    {
        case PROP_FLAGS: {
			DoEntryDateFlags flags;
			g_object_get(priv->entry, "flags", &flags, NULL);
			g_value_set_flags (value, flags);
			break;
        }
        case PROP_YEAR: {
            gint year;
            g_object_get(priv->entry, "year", &year, NULL);
            g_value_set_int (value, year);
            break;
        }
        case PROP_MONTH: {
            gint month;
            g_object_get(priv->entry, "month", &month, NULL);
            g_value_set_int (value, month);
            break;
        }
        case PROP_DAY: {
            gint day;
            g_object_get(priv->entry, "day", &day, NULL);
            g_value_set_int (value, day);
            break;
        }
        case PROP_ENTRY: {
            g_value_set_object (value, priv->entry);
            break;
        }
    }
}

static void do_entry_date_box_class_init (DoEntryDateBoxClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
    //GtkWidgetClass *widget_class = (GtkWidgetClass*)klass;

	object_class->constructor  = do_entry_date_box_constructor;
	object_class->finalize     = do_entry_date_box_finalize;
	object_class->get_property = do_entry_date_box_get_property;
	object_class->set_property = do_entry_date_box_set_property;

	g_type_class_add_private (object_class, sizeof (DoEntryDateBoxPrivate));

	g_object_class_install_property
		(object_class,
		 PROP_FLAGS,
		 g_param_spec_flags ("flags",
				     "flags",
				     "flags",
				     DO_TYPE_ENTRY_DATE_FLAGS,
				     DO_ENTRY_DATE_FLAGS_DEFAULT,
				     G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
				     G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_property (object_class,
                                   PROP_YEAR,
                                   g_param_spec_int ("year",
						     "Year",
						     "The selected year",
						     0, G_MAXINT >> 9, 0,
						     G_PARAM_READWRITE));

  /**
   * GtkCalendar:month:
   *
   * The selected month (as a number between 0 and 11).
   * This property gets initially set to the current month.
   */
  g_object_class_install_property (object_class,
                                   PROP_MONTH,
                                   g_param_spec_int ("month",
						     "Month",
						     "The selected month (as a number between 0 and 11)",
						     1, 12, 1,
						     G_PARAM_READWRITE));

  g_object_class_install_property (object_class,
                                   PROP_ENTRY,
                                   g_param_spec_object ("entry",
							"DoEntryDate",
							"DoEntryDate",
							DO_TYPE_ENTRY_DATE,
							G_PARAM_READABLE));

  /**
   * GtkCalendar:day:
   *
   * The selected day (as a number between 1 and 31, or 0
   * to unselect the currently selected day).
   * This property gets initially set to the current day.
   */
  g_object_class_install_property (object_class,
                                   PROP_DAY,
                                   g_param_spec_int ("day",
						     "Day",
						     "The selected day (as a number between 1 and 31, or 0 to unselect the currently selected day)",
						     0, 31, 0,
						     G_PARAM_READWRITE));

}

GtkWidget *do_entry_date_box_new(DoEntryDateFlags flags)
{
	return g_object_new (DO_TYPE_ENTRY_DATE_BOX,
	                     "entry-type", DO_TYPE_ENTRY_DATE,
	                     "flags", flags,
	                     "spacing", 6,
	                     "homogeneous", FALSE,
			     (gpointer) NULL);
}

static void button_clicked(GtkWidget *button, DoEntryDateBox *box)
{
	DoEntryDateBoxPrivate *priv = DO_ENTRY_DATE_BOX_GET_PRIVATE (box);
    GtkWidget *dialog;
    GtkWidget *calendar;
    gint result;
    GtkWidget *win;
    win = gtk_widget_get_toplevel(GTK_WIDGET(box));

    dialog = gtk_dialog_new_with_buttons(
            "Выбор даты", GTK_WINDOW(win),
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            "Сегодня",
            1,
            "ok",
            GTK_RESPONSE_ACCEPT,
            NULL);
    gtk_window_set_icon_name(GTK_WINDOW(dialog), DO_STOCK_DOMINO_APPS);

    calendar = gtk_calendar_new();
    gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog)))
            , calendar);
    g_signal_connect(calendar, "day-selected-double-click", (GCallback)signal_calendar_event, dialog);
    gint year;
    gint month;
    gint day;
    GDate date = {0,};
    g_object_get(priv->entry, "year", &year, "month", &month, "day", &day, NULL);
    if (g_date_valid_day(day))
        g_date_set_day(&date, day);
    if (g_date_valid_month(month))
        g_date_set_month(&date, month);
    if (g_date_valid_year(year))
        g_date_set_year(&date, year);

    if (g_date_valid(&date)) {
        gtk_calendar_select_month(GTK_CALENDAR(calendar), month - 1, year);
        gtk_calendar_select_day(GTK_CALENDAR(calendar), day);
    }

    gtk_widget_show_all(dialog);

    result = gtk_dialog_run(GTK_DIALOG(dialog));
    gboolean editable;
    g_object_get(priv->entry, "editable", &editable, NULL);
    if (editable)
    switch(result){
        case GTK_RESPONSE_ACCEPT:
            g_object_get(calendar, "year", &year, "month", &month, "day", &day, NULL);
            g_object_set(priv->entry, "year", year, "month", month + 1, "day", day, NULL);
            break;
        case 1:
            g_date_set_time_t(&date, time(NULL));
            g_object_set(priv->entry, "year", date.year, "month", date.month, "day", date.day, NULL);
            break;
        case GTK_RESPONSE_DELETE_EVENT:
        default:
            break;
    }
    gtk_widget_destroy(dialog);
}
GtkWidget  *do_entry_date_box_entry(DoEntryDateBox *box)
{
	DoEntryDateBoxPrivate *priv = DO_ENTRY_DATE_BOX_GET_PRIVATE (box);
	return priv->entry;

}
