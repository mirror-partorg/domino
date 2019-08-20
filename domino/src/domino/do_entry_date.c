

#include "do_entry_date.h"
#include "do_entry.h"
#include "do_icons.h"
#include "do_validate.h"
#include <stdlib.h>
#include <string.h>
//#include "do_marshal.h"

enum
{
	PROP_0,
	PROP_FLAGS,
    PROP_YEAR,
    PROP_MONTH,
    PROP_DAY,
};

#define DO_ENTRY_DATE_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_ENTRY_DATE, DoEntryDatePrivate))

#define CRNT_CENTURY 2000

typedef enum
{
    TEXT_MODE_DATE,
    TEXT_MODE_LIFETIME,
    TEXT_MODE_QTY
}
text_mode_t;

struct _DoEntryDatePrivate
{
	gboolean life_time_mode;
	GtkListStore *store;
	guint year;
	guint month;
	guint day;
	DoEntryDateFlags flags;

};

GType do_entry_date_flags_get_type(void)
{
    static GType type = 0;
    if (G_UNLIKELY (type == 0))
    {
        static const GFlagsValue _do_entry_date_flags_values[] = {
         { DO_ENTRY_DATE_FLAGS_LIFETIME_MODE, "DO_ENTRY_DATE_FLAGS_LIFETIME_MODE", "lifetime-mode" },
         { DO_ENTRY_DATE_FLAGS_OUR_CENTURY, "DO_ENTRY_DATE_FLAGS_OUR_CENTURY", "our-century" },
         { DO_ENTRY_DATE_FLAGS_EMPTY, "DO_ENTRY_DATE_FLAGS_EMPTY", "empty" },
        };
        type = g_flags_register_static ("DoEntryDateFlags", _do_entry_date_flags_values);
    }
    return type;
}

static void entry_icon_press(GtkEntry *entry,  GtkEntryIconPosition icon_pos, GdkEvent  *event, gpointer menu);

static void      do_entry_date_validate_init (DoValidateIface *iface);
static void      do_entry_date_completion (DoValidate *iface);
static gboolean  do_entry_date_valid(DoValidate *iface);
static gboolean  do_entry_date_substitution(DoValidate *iface);

G_DEFINE_TYPE_WITH_CODE (DoEntryDate, do_entry_date, DO_TYPE_ENTRY,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VALIDATE,
						do_entry_date_validate_init));

static void set_date(DoEntryDate *entry);
static void do_entry_date_validate_init (DoValidateIface *iface)
{
    iface->valid = do_entry_date_valid;
    iface->substitution = do_entry_date_substitution;
    iface->completion = do_entry_date_completion;
}

static void do_entry_date_init(DoEntryDate *dialog)
{
//    DoEntryDatePrivate *priv = DO_ENTRY_DATE_GET_PRIVATE (dialog);
}
static gboolean match_func(GtkEntryCompletion *completion,
                                                         const gchar *key,
                                                         GtkTreeIter *iter,
                                                         gpointer user_data)
{
    return TRUE;
}

static gboolean do_entry_date_valid(DoValidate *iface)
{
    GtkEntry *entry = GTK_ENTRY(iface);
    gchar *ch = (char*)gtk_entry_get_text(entry);
	DoEntryDatePrivate *priv = DO_ENTRY_DATE_GET_PRIVATE (iface);
    gint value;
    if (*ch == '\0' && (priv->flags & DO_ENTRY_DATE_FLAGS_EMPTY) == DO_ENTRY_DATE_FLAGS_EMPTY)
        return TRUE;
    GDate *date = g_date_new();
    gboolean retval = FALSE;
    value = strtol(ch, &ch, 10);
    if (*(ch++) == '/' && g_date_valid_day(value)) {
        g_date_set_day(date, value);
        value = strtol(ch, &ch, 10);
        if (*(ch++) == '/'  && g_date_valid_month(value)) {
            g_date_set_month(date, value);
            value = strtol(ch, &ch, 10);
            if (g_date_valid_year(value)) {
                g_date_set_year(date, value);
                retval = g_date_valid(date) && (*ch == '\0');
            }
        }
    }
    if (retval) {
        priv->year = date->year;
        priv->month = date->month;
        priv->day = date->day;
    }
    g_date_free(date);

    return retval;
}

#define TEXT_MODE_KEY "do-text-mode"

static void text_toggled_cb( GtkCheckMenuItem * menu_item,
                        gpointer           vprivate )
{
    if( gtk_check_menu_item_get_active( menu_item ) )
    {
        DoEntryDatePrivate *priv = vprivate;
        text_mode_t mode =
            GPOINTER_TO_UINT( g_object_get_data( G_OBJECT( menu_item ),
                                                 TEXT_MODE_KEY ) );
        switch (mode) {
            case TEXT_MODE_DATE:
                priv->flags = priv->flags ^ DO_ENTRY_DATE_FLAGS_LIFETIME_MODE;
                break;
            case TEXT_MODE_LIFETIME:
                priv->flags = priv->flags | DO_ENTRY_DATE_FLAGS_LIFETIME_MODE;
                break;
            default:
                break;
        }

    }
}



static GObject *do_entry_date_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject *object;
	GtkEntry *entry;
	GtkEntryCompletion *completion;
	DoEntryDatePrivate *priv;
	GtkListStore *store;
	GtkWidget *menu;
    GSList      * l;
    gint          i;

	object = G_OBJECT_CLASS (do_entry_date_parent_class)->constructor(type, n_construct_properties, construct_params);

	entry = GTK_ENTRY(object);
    priv = DO_ENTRY_DATE_GET_PRIVATE (object);

    completion = gtk_entry_get_completion(entry);
    gtk_entry_completion_set_match_func(completion, match_func, NULL, NULL);

    store = priv->store = gtk_list_store_new (1, G_TYPE_STRING);
    gtk_entry_completion_set_model(completion, GTK_TREE_MODEL(store));
    g_object_unref (store);

//    g_signal_connect1 (entry, "activate",
	//		G_CALLBACK (accept_completion_callback), object);

    const char *  text_names[TEXT_MODE_QTY] = {
        "Дата" , "Срок годности"
    };

    //!!gtk_entry_set_icon_from_stock(entry, GTK_ENTRY_ICON_PRIMARY, GTK_STOCK_FIND);
    menu = gtk_menu_new( );
    l = NULL;
    for( i=0; i<TEXT_MODE_QTY; ++i )
    {
        const char * name = text_names[i];
        GtkWidget *  w = gtk_radio_menu_item_new_with_label ( l, name );
        l = gtk_radio_menu_item_get_group( GTK_RADIO_MENU_ITEM( w ) );
        g_object_set_data( G_OBJECT( w ), TEXT_MODE_KEY,
                           GINT_TO_POINTER( i ) );
        g_signal_connect( w, "toggled",
                          G_CALLBACK( text_toggled_cb ), priv );
        gtk_menu_shell_append( GTK_MENU_SHELL( menu ), w );
        gtk_widget_show( w );
        if ((priv->flags & DO_ENTRY_DATE_FLAGS_LIFETIME_MODE) == DO_ENTRY_DATE_FLAGS_LIFETIME_MODE &&
            i == TEXT_MODE_LIFETIME)
            gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (w), TRUE);

    }
    g_signal_connect (entry, "icon-press",
			G_CALLBACK (entry_icon_press), menu);

    /* Use model column 0 as the text column */
    gtk_entry_completion_set_text_column (completion, 0);


	return object;
}

static void do_entry_date_finalize (GObject *object)
{
//	DoEntryDatePrivate *priv = DO_ENTRY_DATE_GET_PRIVATE (object);
	G_OBJECT_CLASS (do_entry_date_parent_class)->finalize (object);
}
static void update_flags(DoEntryDate *entry)
{
}

static void do_entry_date_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoEntryDatePrivate *priv = DO_ENTRY_DATE_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_FLAGS:
			priv->flags = g_value_get_flags (value);
			update_flags(DO_ENTRY_DATE(object));
			break;
        case PROP_YEAR:
            priv->year = g_value_get_int (value);
            //!!set_date(DO_ENTRY_DATE(object));
            break;
        case PROP_MONTH:
            priv->month = g_value_get_int (value);
            //!!set_date(DO_ENTRY_DATE(object));
            break;
        case PROP_DAY:
            priv->day = g_value_get_int (value);
            set_date(DO_ENTRY_DATE(object));
            break;
	}

}
static void do_entry_date_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoEntryDatePrivate *priv = DO_ENTRY_DATE_GET_PRIVATE (object);
    switch (prop_id)
    {
        case PROP_FLAGS:
			g_value_set_flags (value, priv->flags);
			break;
        case PROP_YEAR:
            g_value_set_int (value, priv->year);
            break;
        case PROP_MONTH:
            g_value_set_int (value, priv->month);
            break;
        case PROP_DAY:
            g_value_set_int (value, priv->day);
            break;
    }
}

static void do_entry_date_class_init (DoEntryDateClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
    //GtkWidgetClass *widget_class = (GtkWidgetClass*)klass;

	object_class->constructor  = do_entry_date_constructor;
	object_class->finalize     = do_entry_date_finalize;
	object_class->get_property = do_entry_date_get_property;
	object_class->set_property = do_entry_date_set_property;

	g_type_class_add_private (object_class, sizeof (DoEntryDatePrivate));

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
						     "The selected month (as a number between 1 and 12)",
						     1, 12, 1,
						     G_PARAM_READWRITE));

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

GtkWidget *do_entry_date_new(DoEntryDateFlags flags)
{
	return g_object_new (DO_TYPE_ENTRY_DATE,
	                     "flags", flags,
			     (gpointer) NULL);
}

static void add_completion(GtkListStore *store, GDate *current, int day, int month, int year, DoEntryDatePrivate *priv)
{
    gchar buf[15];
    GDate *date;
    gint nday;
    gint nmonth;
    gint nyear;
    gboolean error = FALSE;

    nday = current->day;
    nmonth = current->month;
    nyear = current->year;

    if ( day > 31 || month > 12  )
        return;


    if (!day && !month && !year) ;

    else
    if (day && month && year) {
        if (g_date_valid_day(day) && g_date_valid_month(month) && g_date_valid_year(year)) {
            nday   = day;
            nmonth = month;
            nyear  = year;
        }
    }
    else
    if (!month && !year) {
        if (g_date_valid_day(day))
            nday   = day;
        else
            error = TRUE;
    }
    else
    if (!day && !year) {
        if (g_date_valid_month(month)) {
            nmonth = month;
            nday = 1;
        }
        else
            error = TRUE;
    }
    else
    if (!day && !month) {
/*        if (g_date_valid_year(year)) {
            nyear  = year;
            nday = 1;
        }
        else*/
            error = TRUE;
    }
    else
    if (!day ) {
        if (g_date_valid_month(month) && g_date_valid_year(year)) {
            nmonth = month;
            nyear  = year;
            nday   = 1;
        }
        else
            error = TRUE;
    }
    else
    if (!month ) {
        if (g_date_valid_day(day) && g_date_valid_year(year)) {
            nday   = day;
            nyear  = year;
        }
        else
            error = TRUE;
    }
    else
    if (!year ) {
        if ( g_date_valid_day(day) && g_date_valid_month(month) ) {
            nday   = day;
            nmonth = month;
        }
        else
            error = TRUE;
    }
    if (!error && ((priv->flags & DO_ENTRY_DATE_FLAGS_OUR_CENTURY) == DO_ENTRY_DATE_FLAGS_OUR_CENTURY))
        error = nyear < CRNT_CENTURY - 100 || nyear > CRNT_CENTURY + 200;
    if (!error) {
        date = g_date_new();
        g_date_set_day(date, nday);
        g_date_set_month(date, nmonth);
        g_date_set_year(date, nyear);
        if (g_date_valid(date)) {
            g_date_strftime(buf, sizeof(buf), "%d/%m/%Y", date);
            GtkTreeIter iter;
            GtkTreePath *path;
            gboolean add = TRUE;
            path = gtk_tree_path_new_first();
            while ( gtk_tree_model_get_iter(GTK_TREE_MODEL(store), &iter, path) ) {
                GValue value={0,};
                gtk_tree_model_get_value(GTK_TREE_MODEL(store), &iter, 0, &value);
                if ( !strcmp(buf, g_value_get_string(&value)) ) {
                    add = FALSE;
                    break;
                }
                gtk_tree_path_next(path);
            }
            gtk_tree_path_free(path);
            if ( add ) {
                gtk_list_store_append (store, &iter);
                gtk_list_store_set (store, &iter, 0, buf, -1);
            }
        }
        g_date_free(date);
    }
}

static void do_entry_date_completion (DoValidate *iface)
{
	DoEntryDatePrivate *priv = DO_ENTRY_DATE_GET_PRIVATE (iface);
	DoEntryDate *entry = DO_ENTRY_DATE(iface);
    const gchar *text;
    gchar *ch;
    GDate  *now = g_date_new();
    g_date_set_time_t(now, time(NULL));
    gint value;

    gtk_list_store_clear(priv->store);
    gint day, month, year;

    text = gtk_entry_get_text(GTK_ENTRY(entry));
    if ( strlen(text) == 5 )
        ch = (gchar*)text;
    ch = (gchar*)text;
    if (*ch != '\0') {

        if (*ch == '+' || *ch == '-') {
            value = strtol(ch, &ch, 10);
            if (!value)
                add_completion(priv->store, now, 0, 0, 0, priv);
            else {
                g_date_add_days(now, value);
                add_completion(priv->store, now, 0, 0, 0, priv);
                g_date_add_days(now, -value);
                g_date_add_months(now, value);
                add_completion(priv->store, now, 0, 0, 0, priv);
                g_date_add_months(now, -value);
                g_date_add_years(now, value);
                add_completion(priv->store, now, 0, 0, 0, priv);
                g_date_add_years(now, -value);
            }
        }
        else {
            value = strtol(ch, &ch, 10);
            if (!value)
                add_completion(priv->store, now, 0, 0, 0, priv);
            else
            if (value > 31) {
                if ( strlen(text) < 4 ) {
                if ((priv->flags & DO_ENTRY_DATE_FLAGS_LIFETIME_MODE) == DO_ENTRY_DATE_FLAGS_LIFETIME_MODE) {
                    year = value % 10;
                    month = (value - year) / 10;
                    add_completion(priv->store, now, 0, month, year + CRNT_CENTURY, priv);

                    month = value % 10;
                    day = (value - month) / 10;
                    add_completion(priv->store, now, day, month, 0, priv);
                }
                else {
                    month = value % 10;
                    day = (value - month) / 10;
                    add_completion(priv->store, now, day, month, 0, priv);

                    year = value % 10;
                    month = (value - year) / 10;
                    add_completion(priv->store, now, 0, month, year + CRNT_CENTURY, priv);
                }
                }
                if ((priv->flags & DO_ENTRY_DATE_FLAGS_LIFETIME_MODE) == DO_ENTRY_DATE_FLAGS_LIFETIME_MODE) {
                    year = value % 100;
                    month = ((value - year) / 100) % 100;
                    day = ((value - year) - month * 100) / 10000;
                    add_completion(priv->store, now, day, month, year + CRNT_CENTURY, priv);

                    month = value % 100;
                    day = (value - month) / 100;
                    add_completion(priv->store, now, day, month, 0, priv);
                }
                else {
                    month = value % 100;
                    day = (value - month) / 100;
                    add_completion(priv->store, now, day, month, 0, priv);


                    year = value % 100;
                    month = ((value - year) / 100) % 100;
                    day = ((value - year) - month * 100) / 10000;
                    add_completion(priv->store, now, day, month, year + CRNT_CENTURY, priv);
                }

                year = value % 10000;
                month = ((value - year) / 10000) % 100;
                day = ((value - year) / 10000 - month * 10000) / 1000000;
                add_completion(priv->store, now, day, month, year, priv);
            }
            else {
                day = value;
                if (*ch == '/')
                    ch++;
                month = strtol(ch, &ch, 10);
                if (*ch == '/')
                    ch++;
                year = strtol(ch, &ch, 10);
                if (!month) {
                    if ((priv->flags & DO_ENTRY_DATE_FLAGS_LIFETIME_MODE) == DO_ENTRY_DATE_FLAGS_LIFETIME_MODE) {
                        add_completion(priv->store, now, 0, day, 0, priv);
                        add_completion(priv->store, now, day, 0, 0, priv);
                    }
                    else {
                        add_completion(priv->store, now, day, 0, 0, priv);
                        add_completion(priv->store, now, 0, day, 0, priv);
                    }
                }
                else {
                    if (year) {
                        if (year < 99)
                            year += CRNT_CENTURY;
                        add_completion(priv->store, now, day, month, year, priv);
                    }
                    else {
                        if ((priv->flags & DO_ENTRY_DATE_FLAGS_LIFETIME_MODE) == DO_ENTRY_DATE_FLAGS_LIFETIME_MODE) {
                            add_completion(priv->store, now, 0, day, month, priv);
                            add_completion(priv->store, now, day, month, 0, priv);
                        }
                        else {
                            add_completion(priv->store, now, day, month, 0, priv);
                            add_completion(priv->store, now, 0, day, month, priv);
                        }
                    }
                }
            }
        }
    }
}
static gboolean do_entry_date_substitution (DoValidate *iface)
{
	DoEntryDatePrivate *priv = DO_ENTRY_DATE_GET_PRIVATE (iface);
	GtkEntry *entry = GTK_ENTRY(iface);
	const gchar *text = gtk_entry_get_text(entry);
	GtkTreePath *path;
	GtkTreeIter  iter;
	GValue  value = {0,};
	gint count = 0;
	gboolean retval = TRUE;

	do_entry_date_completion(iface);

    path = gtk_tree_path_new_first();
    while (count < 3 &&  gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->store), &iter, path)) {
        if (!count)
            gtk_tree_model_get_value(GTK_TREE_MODEL(priv->store), &iter, 0, &value);
        count++;
        gtk_tree_path_next(path);
    }
    if( ((count == 1 && strcmp(text, g_value_get_string(&value))) ||
        ((priv->flags & DO_ENTRY_DATE_FLAGS_LIFETIME_MODE) == DO_ENTRY_DATE_FLAGS_LIFETIME_MODE && count ==2))
        &&
        strcmp(g_value_get_string(&value), text)
      ) {
        gtk_entry_set_text(entry, g_value_get_string(&value));
        retval = TRUE;
    }
    gtk_tree_path_free(path);
    return retval;
}
static void entry_icon_press(GtkEntry *entry,  GtkEntryIconPosition icon_pos, GdkEvent  *event, gpointer menu)
{
    if( icon_pos == GTK_ENTRY_ICON_PRIMARY )
        gtk_menu_popup( GTK_MENU( menu ), NULL, NULL, NULL, NULL, 0,
                        gtk_get_current_event_time( ) );
}
static void set_date(DoEntryDate *entry)
{
	DoEntryDatePrivate *priv = DO_ENTRY_DATE_GET_PRIVATE (entry);
    GDate date = {0,};
    if (g_date_valid_day(priv->day))
        g_date_set_day(&date, priv->day);
    if (g_date_valid_month(priv->month))
        g_date_set_month(&date, priv->month);
    if (g_date_valid_year(priv->year))
        g_date_set_year(&date, priv->year);

    if (g_date_valid(&date)) {
        gchar buf[100];
        g_date_strftime(buf, sizeof(buf), "%d/%m/%Y", &date);
        gtk_entry_set_text(GTK_ENTRY(entry), buf);
    }
    else
        gtk_entry_set_text(GTK_ENTRY(entry), "");
}
void  do_entry_date_set_today(DoEntryDate *entry)
{
	DoEntryDatePrivate *priv = DO_ENTRY_DATE_GET_PRIVATE (entry);
    GDate now = {0, };
    gchar buf[100];
    g_date_set_time_t(&now, time(NULL));
    priv->year = now.year;
    priv->month = now.month;
    priv->day = now.day;
    g_date_strftime(buf, sizeof(buf), "%d/%m/%Y", &now);
    gtk_entry_set_text(GTK_ENTRY(entry), buf);
}
