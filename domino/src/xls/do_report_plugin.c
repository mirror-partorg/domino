#include "do_report_plugin.h"
#include <libgnumeric.h>
#include <gnumeric-gconf.h>
#include <gnm-plugin.h>
#include <workbook-view.h>
#include <workbook.h>
#include <cell.h>
#include <sheet.h>
#include <sheet-merge.h>
#include <session.h>
#include <value.h>
#include <ranges.h>
#include <commands.h>
#include <clipboard.h>
#include <command-context-stderr.h>
#include <string.h>
#include "do_scan.h"

static void do_report_plugin_get_property  (GObject              *object,
					     guint                 param_id,
					     GValue               *value,
					     GParamSpec           *pspec);
static void do_report_plugin_set_property  (GObject              *object,
					     guint                 param_id,
					     const GValue         *value,
					     GParamSpec           *pspec);
static void do_report_plugin_finalize (GObject          *object);
#define DO_REPORT_PLUGIN_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), DO_TYPE_REPORT_PLUGIN, DoReportPluginPrivate))

#define PROGRESS_DETAIL(...) { if (priv->progress) { gchar *v = g_strdup_printf(__VA_ARGS__); do_progress_info_set_details(priv->progress, v); g_free(v);}}
#define PROGRESS_STATUS(...) { if (priv->progress) { gchar *v = g_strdup_printf(__VA_ARGS__); do_progress_info_set_status(priv->progress, v); g_free(v);}}
#define PROGRESS_PROGRESS(a,b) { if (priv->progress) do_progress_info_set_progress(priv->progress, a, b); }
#define PROGRESS_PULSE() { if (priv->progress) do_progress_info_pulse_progress(priv->progress); }
#define CANCELLED ( priv->progress && g_cancellable_is_cancelled(priv->cancellable) )
#define DEBUG(...) { if (priv->debug) {gchar *v = g_strdup_printf(__VA_ARGS__); priv->debug_errors=g_slist_append(priv->debug_errors, v); } }


typedef struct _DoReportPluginPrivate DoReportPluginPrivate;

struct _DoReportPluginPrivate
{
    GOCmdContext *cc;
    GOIOContext  *io_context;
    WorkbookView *wbv;
    WBCGtk       *wc;
    Workbook     *wb;
    gchar        *encoding;
    gchar        *template_filename;
    gchar        *template_uri;
    gchar        *app_name;
    xmlDoc       *values_;
    GSourceFunc   thread_exit_func;
    gpointer      thread_exit_data;
    DoProgressInfo *progress;
    GCancellable  *cancellable;
    gint          total_record;
    gint          crnt_record;
    gboolean      complect_process;
    gboolean      debug;
    GSList        *debug_errors;
    gboolean      readonly;
    gchar         *title;
};


enum {
    PROP_0,
    PROP_APP_NAME,
    PROP_FILENAME,
    PROP_URI,
    PROP_VALUES,
    PROP_PROGRESS_INFO,
    PROP_TOPLEVEL,
    PROP_IS_DONE,
    PROP_DEBUG,
    PROP_READONLY,
    PROP_TITLE,
};

G_DEFINE_TYPE (DoReportPlugin, do_report_plugin, G_TYPE_OBJECT)

typedef enum _DoReportPluginItemType DoReportPluginItemType;
typedef enum _DoReportPluginColRowOperation DoReportPluginColRowOperation;
typedef struct _DoReportPluginItem DoReportPluginItem;
typedef struct _DoReportPluginItemProperty DoReportPluginItemProperty;
typedef struct _DoReportPluginClipboardItem DoReportPluginClipboardItem;

enum _DoReportPluginItemType {
    DO_REPORT_PLUGIN_ITEM_TYPE_BLOCK = 0,
    DO_REPORT_PLUGIN_ITEM_TYPE_EXP = 1,
    DO_REPORT_PLUGIN_ITEM_TYPE_TEXT = 2,
    DO_REPORT_PLUGIN_ITEM_TYPE_VAR = 3,
    DO_REPORT_PLUGIN_ITEM_TYPE_EMPTY = 4,
    DO_REPORT_PLUGIN_ITEM_TYPE_UNDEFINE = 5,
};

enum _DoReportPluginColRowOperation {
    COLS_DELETE=0,
    ROWS_DELETE,
    COLS_INSERT,
    ROWS_INSERT,
};
struct _DoReportPluginItem {
    DoReportPluginItemType type;
    gchar *name;
    DoReportPluginItem *left;
    DoReportPluginItem *right;
    GList *properties;
    gboolean close;
};
struct _DoReportPluginClipboardItem {
    GnmRange *src;
    GnmCellRegion *region;
};

struct _DoReportPluginItemProperty {
    gchar *name;
    gchar *value;
};

typedef struct _DoReportPluginFillInfo DoReportPluginFillInfo;

typedef struct _DoReportPluginSheetInfo DoReportPluginSheetInfo;
typedef struct _DoReportPluginBlock DoReportPluginBlock;
typedef struct _DoReportPluginBlockCollection DoReportPluginBlockCollection;
typedef struct _DoReportPluginBlockPath DoReportPluginBlockPath;

struct _DoReportPluginBlock {
    gchar *name;
    gboolean vertical;
    gboolean auto_height;
    gboolean auto_width;
    gint start;
    gint end;
    DoReportPluginBlock *parent;
    GList *children;
};
struct _DoReportPluginBlockCollection {
    GList *vertical;
    GList *horizontal;
};
struct _DoReportPluginSheetInfo {
    Sheet *sheet;
    xmlNode *root;
    DoReportPluginBlockCollection *blocks;
};

struct _DoReportPluginBlockPath {
    gboolean vertical;
    gint number;
    gint start;
    gint size;
    DoReportPluginBlock *block;
    GPtrArray *children;
};

struct _DoReportPluginFillInfo {
    xmlNode *root;
    GList *blocks;
    GList *paths;
    gboolean pass;
    gboolean run;
    DoReportPlugin *plugin;
};


static gboolean do_report_plugin_make(DoReportPlugin *plugin) G_GNUC_WARN_UNUSED_RESULT;
static gboolean do_report_plugin_sheet_make(DoReportPlugin *plugin, Sheet *sheet, xmlNode *root,
                                            DoReportPluginBlockCollection *blocks) G_GNUC_WARN_UNUSED_RESULT;
static gboolean do_report_plugin_blocks_make(DoReportPlugin *plugin, Sheet *sheet,
                                             xmlNode *root, DoReportPluginBlockCollection *blocks) G_GNUC_WARN_UNUSED_RESULT;
static gboolean do_report_plugin_block_make(DoReportPlugin *plugin, Sheet *sheet,
                                                  DoReportPluginBlock *block, xmlNode *root,
                                                  gint *dr, gint *dc,
                                                  GList *blocks, GList **paths) G_GNUC_WARN_UNUSED_RESULT;
static gboolean do_report_plugin_col_row_operation(DoReportPlugin *plugin, Sheet *sheet,
                                                   DoReportPluginColRowOperation operation,
                                                   gint start, gint count) G_GNUC_WARN_UNUSED_RESULT;

static gboolean do_report_plugin_save_to_temp(DoReportPlugin *plugin) G_GNUC_WARN_UNUSED_RESULT;
static gboolean do_report_plugin_cols_autofit(Sheet *sheet, GList *paths) G_GNUC_WARN_UNUSED_RESULT;
static gboolean do_report_plugin_rows_autofit(Sheet *sheet, GnmRange *range) G_GNUC_WARN_UNUSED_RESULT;

static xmlNode *do_report_plugin_sheet_get_root(DoReportPlugin *plugin, Sheet *sheet);
static DoReportPluginBlockCollection *do_report_plugin_blocks_get(DoReportPlugin *plugin, Sheet *sheet, xmlNode *root);

static gint do_report_plugin_sheet_get_total(DoReportPlugin *plugin, Sheet *sheet, xmlNode *root, DoReportPluginBlockCollection *blocks);
static gint do_report_plugin_block_get_total(DoReportPlugin *plugin, DoReportPluginBlock *block, xmlNode *root);
static DoReportPluginItem *gram_S(DoScan *scan);
static DoReportPluginItem *gram_T(DoScan *scan);
static DoReportPluginItem *gram_V(DoScan *scan);
static gboolean gram_Ps(DoScan *scan, GList **properties);

static gchar *do_report_plugin_get_value(DoReportPluginItem *item, xmlNode *root, GnmValue *value);
static gchar *do_report_plugin_get_value_string(DoReportPluginItem *item, xmlNode *root);
static gchar *do_report_plugin_values_get_text(xmlNode *root, const gchar *name);

static GnmValue *do_report_plugin_cell_fill(GnmCellIter const *iter, DoReportPluginFillInfo *fill);

/*
static DoReportPluginItem *do_report_plugin_get_item(GnmValue *value);

static gint do_report_plugin_block_get_total(DoReportPlugin *plugin, DoReportPluginBlock *block, xmlNode *root);
static DoReportPluginBlockCollection *do_report_plugin_blocks_get(DoReportPlugin *plugin, Sheet *sheet, xmlNode *root);
*/



static void do_report_plugin_init (DoReportPlugin *report_plugin)
{
}

static GObject *do_report_plugin_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
    GObject *object;
    DoReportPluginPrivate *priv;
    DoReportPlugin *plugin;

    object = G_OBJECT_CLASS (do_report_plugin_parent_class)->constructor(type, n_construct_properties, construct_params);
    plugin = DO_REPORT_PLUGIN(object);
    priv = DO_REPORT_PLUGIN_GET_PRIVATE(plugin);

    gnm_session_init(priv->app_name);
    gnm_init();

	GOErrorInfo	*plugin_errs;

	//gnm_pre_parse_init (1, (const gchar**)&priv->app_name);

    priv->cc = cmd_context_stderr_new ();
    priv->io_context = go_io_context_new (priv->cc);
    g_object_unref (priv->cc);
    g_set_application_name(priv->app_name);

        gnm_plugins_init (GO_CMD_CONTEXT (priv->cc));

	go_plugin_db_activate_plugin_list (
		go_plugins_get_available_plugins (), &plugin_errs);

	if ( plugin_errs ) {
		/* FIXME: What do we want to do here? */
		go_error_info_free (plugin_errs);
	}
	return object;
}

static void do_report_plugin_class_init (DoReportPluginClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS (class);

    object_class->constructor  = do_report_plugin_constructor;
    object_class->get_property = do_report_plugin_get_property;
    object_class->set_property = do_report_plugin_set_property;
    object_class->finalize     = do_report_plugin_finalize;

// signal
// property

    g_object_class_install_property (object_class,
				   PROP_APP_NAME,
				   g_param_spec_string ("app-name",
							"Application name",
							"Application name",
							NULL,
							G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property (object_class,
				   PROP_FILENAME,
				   g_param_spec_string ("template-filename",
							"Template filename",
							"Template filename",
							NULL,
							G_PARAM_READWRITE));
    g_object_class_install_property (object_class,
				   PROP_URI,
				   g_param_spec_string ("template-uri",
							"Template uri",
							"Template uri",
							NULL,
							G_PARAM_READWRITE));
    g_object_class_install_property (object_class,
				   PROP_VALUES,
				   g_param_spec_pointer("values",
							"XML document",
							"XML document",
							G_PARAM_READWRITE));
    g_object_class_install_property (object_class,
				   PROP_TOPLEVEL,
				   g_param_spec_object ("toplevel",
							"Toplevel",
							"Toplevel",
						     GTK_TYPE_WINDOW,
						     G_PARAM_READABLE));
    g_object_class_install_property (object_class,
				   PROP_PROGRESS_INFO,
				   g_param_spec_object ("progress-info",
							"progress-info",
							"progress-info",
						     DO_TYPE_PROGRESS_INFO,
						     G_PARAM_WRITABLE));
    g_object_class_install_property (object_class,
				   PROP_IS_DONE,
				   g_param_spec_boolean ("is-done",
							"report is done",
							"report is done",
						     FALSE,
						     G_PARAM_READABLE));
    g_object_class_install_property (object_class,
				   PROP_DEBUG,
				   g_param_spec_boolean ("debug",
							"debug",
							"debug",
						     FALSE,
						     G_PARAM_READWRITE));

    g_object_class_install_property (object_class,
				   PROP_READONLY,
				   g_param_spec_boolean ("readonly",
							"sheet read only",
							"sheet read only",
						     TRUE,
						     G_PARAM_READWRITE));

    g_object_class_install_property (object_class,
				   PROP_TITLE,
				   g_param_spec_string("title",
							"title",
							"title",
						     NULL,
						     G_PARAM_READWRITE));

    g_type_class_add_private (object_class, sizeof (DoReportPluginPrivate));
}

static void do_report_plugin_get_property (GObject     *object,
				guint        param_id,
				GValue      *value,
				GParamSpec  *pspec)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (object);

    switch (param_id) {
        case PROP_APP_NAME:
            g_value_set_string (value, priv->app_name);
            break;
        case PROP_FILENAME:
            g_value_set_string (value, priv->template_filename);
            break;
        case PROP_URI:
            g_value_set_string (value, priv->template_uri);
            break;
        case PROP_VALUES:
            g_value_set_pointer(value, priv->values_);
            break;
        case PROP_TOPLEVEL:
            g_value_set_object (value, wbcg_toplevel(priv->wc));
            break;
        case PROP_IS_DONE:
            g_value_set_boolean(value, do_report_plugin_is_done(DO_REPORT_PLUGIN(object)));
            break;
        case PROP_DEBUG:
            g_value_set_boolean(value, do_report_plugin_get_debug(DO_REPORT_PLUGIN(object)));
            break;
        case PROP_READONLY:
            g_value_set_boolean(value, do_report_plugin_get_readonly(DO_REPORT_PLUGIN(object)));
            break;
        case PROP_TITLE:
            g_value_set_string(value, do_report_plugin_get_title(DO_REPORT_PLUGIN(object)));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
            break;
    }
}

static void
do_report_plugin_set_property (GObject      *object,
				guint         param_id,
				const GValue *value,
				GParamSpec   *pspec)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (object);

    switch ( param_id ) {
        case PROP_APP_NAME:
            priv->app_name = g_value_dup_string (value);
            break;
        case PROP_FILENAME:
            do_report_plugin_set_template_filename(DO_REPORT_PLUGIN(object), g_value_get_string(value));
            break;
        case PROP_URI:
            do_report_plugin_set_template_uri(DO_REPORT_PLUGIN(object), g_value_get_string(value));
            break;
        case PROP_VALUES:
            do_report_plugin_set_values(DO_REPORT_PLUGIN(object), g_value_get_pointer(value));
            break;
        case PROP_PROGRESS_INFO:
            do_report_plugin_set_progress_info(DO_REPORT_PLUGIN(object), g_value_get_object(value));
            break;
        case PROP_DEBUG:
            do_report_plugin_set_debug(DO_REPORT_PLUGIN(object), g_value_get_boolean(value));
            break;
        case PROP_READONLY:
            do_report_plugin_set_readonly(DO_REPORT_PLUGIN(object), g_value_get_boolean(value));
            break;
        case PROP_TITLE:
            do_report_plugin_set_title(DO_REPORT_PLUGIN(object), g_value_get_string(value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
            break;
    }
}
static void do_report_plugin_finalize (GObject          *object)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (object);

    g_free(priv->app_name);
    g_free(priv->template_filename);
    g_free(priv->template_uri);
    if ( priv->values_ )
        xmlFreeDoc(priv->values_);
    if ( priv->wb )
        g_object_unref(priv->wb);
    g_free(priv->title);
    g_object_unref (priv->io_context);
	gnm_shutdown ();

    G_OBJECT_CLASS (do_report_plugin_parent_class)->finalize (object);
}
DoReportPlugin *do_report_plugin_new(const gchar *app_name)
{
    return g_object_new (DO_TYPE_REPORT_PLUGIN, "app-name", app_name, "readonly", TRUE, NULL);
}

void do_report_plugin_set_template_filename(DoReportPlugin *plugin, const gchar *filename)
{
    //DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    gchar *uri;
    uri = go_shell_arg_to_uri(filename);
    do_report_plugin_set_template_uri(plugin, uri);
    g_free(uri);
}
gboolean do_report_plugin_is_done(DoReportPlugin *plugin)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    return priv->complect_process;
}

gchar *do_report_plugin_get_template_filename(DoReportPlugin *plugin)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    return priv->template_filename;
}
gchar *do_report_plugin_get_template_uri(DoReportPlugin *plugin)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    return priv->template_uri;
}
xmlDoc *do_report_plugin_get_values(DoReportPlugin *plugin)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    return priv->values_;
}
GtkWindow *do_report_plugin_get_toplevel(DoReportPlugin *plugin)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    return wbcg_toplevel(priv->wc);
}
gboolean do_report_plugin_get_readonly(DoReportPlugin *plugin)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    return priv->readonly;

}
void do_report_plugin_set_readonly(DoReportPlugin *plugin, gboolean readonly)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    priv->readonly=readonly;
}
gchar *do_report_plugin_get_title(DoReportPlugin *plugin)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    return priv->title;
}
void do_report_plugin_set_title(DoReportPlugin *plugin, const gchar *title)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    g_free(priv->title);
    priv->title=g_strdup(title);
    if ( priv->wc ) {
        GtkWindow *win;
        win = wbcg_toplevel(priv->wc);
        if ( win )
            gtk_window_set_title(win, title);
    }
}


static gboolean do_report_plugin_save_to_temp(DoReportPlugin *plugin)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    if ( !priv->wb )
        return FALSE;

    GOFileSaver *fs = NULL;
    GDateTime *now;
    gboolean res;
    gchar *value;
    gchar *name;
    gchar *filename;

    now = g_date_time_new_now_local();
    name = g_path_get_basename(priv->template_filename);
    fs = go_file_saver_for_file_name (priv->template_filename);
    value = g_date_time_format(now, "%Y%m%d-%H%M%S");
    filename = g_strdup_printf("report-%s-%s", value, name);
    g_free(value);
    g_free(name);
    g_date_time_unref(now);
    name = g_build_filename (g_get_tmp_dir (), filename, NULL);
    g_free(filename);
    value = go_filename_to_uri(name);
    res = wb_view_save_as(priv->wbv, fs, value, priv->cc);
    g_free(name);
    g_free(value);

    return res;

}
void do_report_plugin_set_template_uri(DoReportPlugin *plugin, const gchar *uri)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    GOFileOpener *fo = NULL;

    g_free(priv->template_filename);
    g_free(priv->template_uri);
    priv->template_uri = g_strdup(uri);
    priv->template_filename = go_filename_from_uri(uri);
    if ( priv->wb )
        g_object_unref(priv->wb);
    priv->wbv = wb_view_new_from_uri  (uri, fo,
                        priv->io_context, priv->encoding);
    if ( !priv->wbv ) {
        go_io_error_display (priv->io_context);
    }
    else {
        priv->wb = wb_view_get_workbook (priv->wbv);
    }
    //wb_view_set_attribute (priv->wbv, "WorkbookView::is_protected", "TRUE");
}
void do_report_plugin_set_progress_info(DoReportPlugin *plugin, DoProgressInfo *progress)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    priv->progress = progress;
    priv->cancellable = do_progress_info_get_cancellable(progress);
}

void do_report_plugin_set_values(DoReportPlugin *plugin, xmlDoc *values)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    if ( priv->values_ ) {
        xmlFreeDoc(priv->values_);
    }
    priv->values_ = xmlCopyDoc(values, TRUE);
    //do_report_plugin_make(plugin);
}
GtkWindow *do_report_plugin_show(DoReportPlugin *plugin)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    if ( priv->wb )
        priv->wc = wbc_gtk_new (NULL, priv->wb, NULL, NULL);
    if ( priv->title )
        gtk_window_set_title(wbcg_toplevel(priv->wc), priv->title);

    return do_report_plugin_get_toplevel(plugin);
}
gboolean do_report_plugin_get_debug(DoReportPlugin *plugin)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    return priv->debug;

}
void do_report_plugin_set_debug(DoReportPlugin *plugin, gboolean debug)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    priv->debug = debug;
}

static void free_block(DoReportPluginBlock *a)
{
    g_list_foreach(a->children, (GFunc)free_block, NULL);
    g_list_free(a->children);
    g_free(a->name);
    g_free(a);

}
static void free_collection1(DoReportPluginBlockCollection *blocks)
{
    g_list_foreach(blocks->vertical, (GFunc)free_block, NULL);
    g_list_free(blocks->vertical);
    g_list_foreach(blocks->horizontal, (GFunc)free_block, NULL);
    g_list_free(blocks->horizontal);
}
static void do_report_plugin_make_end(DoReportPlugin *plugin)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    if ( priv->thread_exit_func )
        g_idle_add(priv->thread_exit_func, priv->thread_exit_data);
}
static gboolean do_report_plugin_make(DoReportPlugin *plugin)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);

    priv->complect_process = FALSE;
    gboolean res;


    if ( !priv->template_uri || !priv->values_  || !priv->wb || !priv->wbv ) {
        do_report_plugin_make_end(plugin);
        return FALSE;
    }

    res = do_report_plugin_save_to_temp(plugin);
    if ( !res ) {
        do_report_plugin_make_end(plugin);
        return FALSE;
    }

    do_report_plugin_make_end(plugin);//!!

    Workbook *wb=priv->wb;
    GSList *sheets, *l;
    GSList *sheet_info = NULL;
    int n;

    sheets = workbook_sheets(wb);
    priv->total_record = 0;
    priv->crnt_record = 0;

    for ( l = sheets; l; l=l->next ) {
        DoReportPluginSheetInfo *info;
        info = g_malloc0(sizeof(DoReportPluginSheetInfo));
        info->sheet = l->data;
        sheet_info = g_slist_append(sheet_info, info);

        info->root = do_report_plugin_sheet_get_root(plugin, info->sheet);
        if ( !info->root )
            continue;
        info->blocks = do_report_plugin_blocks_get(plugin, info->sheet, info->root);
        if ( !info->blocks ) {
            res = FALSE;
            break;
        }
        if ( priv->progress )
            priv->total_record += do_report_plugin_sheet_get_total(plugin, info->sheet, info->root, info->blocks);
    }
    for (n=1,l=sheet_info ; res && l; l=l->next,n++ ) {
        DoReportPluginSheetInfo *info;
        info = l->data;

        if ( !info->root )
            continue;

        PROGRESS_DETAIL("%s (%d/%d)",info->sheet->name_unquoted, n, g_slist_length(sheet_info));
        res = do_report_plugin_sheet_make(plugin, info->sheet, info->root, info->blocks);
        info->sheet->is_protected = priv->readonly;
        free_collection1(info->blocks);
    }
    g_slist_foreach(sheet_info, (GFunc)g_free, NULL);
    g_slist_free(sheet_info);

    if ( res )
        priv->complect_process = TRUE;

    if ( priv->debug && priv->debug_errors ) {
        Sheet *sheet;
        GnmCell *cell;
        int i;
        int cols = gnm_conf_get_core_workbook_n_cols ();
        int rows = gnm_conf_get_core_workbook_n_rows ();
        sheet = workbook_sheet_add(wb, 0, cols, rows);
        cell = sheet_cell_fetch(sheet, 0, 0);
        sheet_cell_set_text(cell, "Ошибки:",NULL);
        for (i=1,l = priv->debug_errors; l; l=l->next,i++ ) {
            cell = sheet_cell_fetch(sheet, 0, i);
            sheet_cell_set_text(cell, l->data, NULL);
            g_free(l->data);
        }
        g_slist_free(priv->debug_errors);
        priv->debug_errors=NULL;
    }

    //do_report_plugin_make_end(plugin);
    return res;
}
GThread *do_report_plugin_make_in_thread(DoReportPlugin *plugin, GSourceFunc exit_func, gpointer data)
{
    GThread *res;
    GError *error = NULL;
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);

    priv->thread_exit_func=exit_func;
    priv->thread_exit_data=data;

    res = g_thread_create((GThreadFunc)do_report_plugin_make, plugin, TRUE, &error);
    return res;
}
static gint sort_cells(GnmEvalPos **a, GnmEvalPos **b)
{
    int i;
    i = (*a)->eval.row-(*b)->eval.row;
    if ( i ) return i;
    return (*a)->eval.col-(*b)->eval.col;
}

static void free_property(DoReportPluginItemProperty *i)
{
    g_free(i->name);
    g_free(i->value);
    g_free(i);
}
static gint cmp_property(DoReportPluginItemProperty *a, DoReportPluginItemProperty *b)
{
    return g_strcmp0(a->name, b->name);
}
static DoReportPluginItem *do_report_plugin_item_new(DoReportPluginItemType type)
{
    DoReportPluginItem *item;
    item = g_malloc0(sizeof(DoReportPluginItem));
    item->type = type;
    return item;
}
static void do_report_plugin_item_free(DoReportPluginItem *item)
{
    g_free(item->name);
    if ( item->left )
        do_report_plugin_item_free(item->left);
    if ( item->right )
        do_report_plugin_item_free(item->right);
    g_list_foreach(item->properties, (GFunc)free_property, NULL);
    g_list_free(item->properties);
    g_free(item);

}
static DoReportPluginItem *do_report_plugin_get_item(GnmValue *value)
{
    DoReportPluginItem *item;
    item = g_malloc0(sizeof(DoReportPluginItem));

    if ( VALUE_IS_EMPTY(value) ) {
        item->type = DO_REPORT_PLUGIN_ITEM_TYPE_EMPTY;
        return item;
    }

    if ( !VALUE_IS_STRING(value) ) {
        item->type = DO_REPORT_PLUGIN_ITEM_TYPE_UNDEFINE;
        return item;
    }

    gchar *str;
    DoScan *scan;
    str = value_get_as_string(value);
    scan = do_scan_new(str);
    item = gram_S(scan);
    g_object_unref(scan);
    return item;

}
static DoReportPluginItem *gram_S(DoScan *scan)
{
// <S>-><T><S>|<V><S>|eps
    DoScanPosition *pos;
    DoReportPluginItem *left,*right;

    pos = do_scan_get_position(scan);

    if ( do_scan_step(scan) == DO_SCAN_LEXEM_END ) { // eps
        do_scan_position_free(pos);
        return do_report_plugin_item_new(DO_REPORT_PLUGIN_ITEM_TYPE_EMPTY);
    }
    do_scan_set_position(scan, pos);

    left = gram_V(scan);
    if ( !left ) {
        do_scan_set_position(scan, pos);
        left = gram_T(scan);
    }
    if ( left ) {
        do_scan_position_free(pos);

        right = gram_S(scan);
        if ( !right ) {
            do_report_plugin_item_free(left);
            return NULL;
        }
        if ( right->type == DO_REPORT_PLUGIN_ITEM_TYPE_EMPTY ) {
            do_report_plugin_item_free(right);
            return left;
        }

        DoReportPluginItem *res;
        res = do_report_plugin_item_new(DO_REPORT_PLUGIN_ITEM_TYPE_EXP);
        res->left = left;
        res->right = right;
        return res;
    }

    DoReportPluginItem *res;
    res = do_report_plugin_item_new(DO_REPORT_PLUGIN_ITEM_TYPE_TEXT);
    res->name = g_strdup("");
    return res;
}
static DoReportPluginItem *gram_T(DoScan *scan)
{
// <T>->text
    DoScanPosition *head, *pos;
    head = do_scan_get_position(scan);

    while ( do_scan_skip(scan, DO_SCAN_LEXEM_LESS) ) {
        pos = do_scan_get_position(scan);
        if ( gram_V(scan) ) {
            do_scan_set_position(scan, pos);
            do_scan_position_free(pos);
            break;
        }
        do_scan_position_free(pos);
        do_scan_step(scan);
    }

    DoReportPluginItem *item;
    item = do_report_plugin_item_new(DO_REPORT_PLUGIN_ITEM_TYPE_TEXT);
    item->name = g_strdup(do_scan_get_text_range(scan, head, NULL));
    do_scan_position_free(head);
    return item;
}
const gchar *empty="";
static const gchar *get_property(GList *properties, const char *name)
{
    GList *i;
    DoReportPluginItemProperty p;
    p.name = (gchar*)name;
    i = g_list_find_custom(properties, &p, (GCompareFunc)cmp_property);
    if ( !i || !i->data )
        return empty;
    return ((DoReportPluginItemProperty*)i->data)->value;
}
static gboolean property_is(GList *properties, const char *name, const char *value)
{
    return !g_strcmp0(get_property(properties, name), value);
}

#define RETURN_NULL { g_list_foreach(properties, (GFunc)free_property, NULL); g_list_free(properties); g_free(name); do_scan_set_position(scan, pos); do_scan_position_free(pos);return NULL;}
static DoReportPluginItem *gram_V(DoScan *scan)
{
// <V>->"<"ident <Ps>">" | "<"/ident">"
    DoScanPosition *pos;
    DoScanLexemType type = DO_SCAN_LEXEM_ERROR;
    DoReportPluginItem *item = NULL;
    gboolean close=FALSE;
    gchar *name = NULL;
    GList *properties = NULL;

    pos = do_scan_get_position(scan);
    type = do_scan_step(scan);
    if ( type != DO_SCAN_LEXEM_LESS )
        RETURN_NULL;

    type = do_scan_step(scan);
    if ( type == DO_SCAN_LEXEM_DIV ) {
        type = do_scan_step(scan);
        close = TRUE;
    }
    if ( type != DO_SCAN_LEXEM_IDENT )
        RETURN_NULL;

    g_object_get(scan, "last-lexem", &name, NULL);
    if ( !close )
        if ( !gram_Ps(scan, &properties) )
            RETURN_NULL;

    type = do_scan_step(scan);
    if ( type != DO_SCAN_LEXEM_GREATE )
        RETURN_NULL;
    do_scan_position_free(pos);

    item = do_report_plugin_item_new( ( close || property_is(properties, PROPERTY_TYPE, PROPERTY_VALUE_BLOCK))  ?
                                     DO_REPORT_PLUGIN_ITEM_TYPE_BLOCK : DO_REPORT_PLUGIN_ITEM_TYPE_VAR);
    item->name = name;
    item->properties = properties;
    item->close = close;

    return item;
}

static gboolean gram_Ps(DoScan *scan, GList **properties)
{
//<Ps>-><P><Ps>|eps
//<P>->ident=string
    DoScanPosition *pos;
    DoScanLexemType type = DO_SCAN_LEXEM_ERROR;

    pos = do_scan_get_position(scan);
    type = do_scan_step(scan);

    while ( type == DO_SCAN_LEXEM_IDENT ) {
        gchar *name;
        gchar *value;
        g_object_get(scan, "last-lexem", &name, NULL);
        type = do_scan_step(scan);
        if ( type != DO_SCAN_LEXEM_EQUAL ) {
            g_free(name);
            break;
        }
        type = do_scan_step(scan);
        if ( type != DO_SCAN_LEXEM_CSTRING ) {
            g_free(name);
            break;
        }
        g_object_get(scan, "last-lexem", &value, NULL);
        DoReportPluginItemProperty *property;
        property = g_malloc0(sizeof(DoReportPluginItemProperty));
        property->name = name;
        property->value = value;

        *properties = g_list_append(*properties, property);

        do_scan_position_free(pos);
        pos = do_scan_get_position(scan);
        type = do_scan_step(scan);
    }
    do_scan_set_position(scan,pos);
    do_scan_position_free(pos);
    return TRUE;
}
static gchar *do_report_plugin_get_value(DoReportPluginItem *item, xmlNode *root, GnmValue *value)
{
    gchar *str;
    str = do_report_plugin_get_value_string(item, root);

    return str;
}
static gchar *do_report_plugin_get_value_string(DoReportPluginItem *item, xmlNode *root)
{
    gchar *res = NULL;
    switch ( item->type ) {
        case DO_REPORT_PLUGIN_ITEM_TYPE_EXP: {
            gchar *left, *right;
            left = do_report_plugin_get_value_string(item->left, root);
            right = do_report_plugin_get_value_string(item->right, root);
            if ( left && right )
                res = g_strdup_printf("%s%s", left, right);
            g_free(left);g_free(right);
            break;
        }
        case DO_REPORT_PLUGIN_ITEM_TYPE_TEXT:
            res = g_strdup(item->name);
            break;
        case DO_REPORT_PLUGIN_ITEM_TYPE_BLOCK:
        case DO_REPORT_PLUGIN_ITEM_TYPE_EMPTY:
        case DO_REPORT_PLUGIN_ITEM_TYPE_UNDEFINE:
            res = g_strdup("");
            break;
        case DO_REPORT_PLUGIN_ITEM_TYPE_VAR:
            res = do_report_plugin_values_get_text(root, item->name);
            break;
    }
    return res;
}
static xmlNode *get_child(xmlNode *root, const gchar *name)
{
    xmlNode *cur;
    if ( !root )
        return NULL;

    cur = root->xmlChildrenNode;
    while ( cur ) {
        if ( (cur->type == XML_ELEMENT_NODE) && !g_strcmp0((gchar *)cur->name, name) ) {
            return cur;
        }
        cur = cur->next;
    }
    return get_child(root->parent, name);
}
static gchar *get_text(xmlNode *node)
{
    gchar *res = NULL;
    if ( node->type == XML_TEXT_NODE ) {
        if ( node->content != NULL )
            res=g_strdup((gchar*)node->content);
        else
            res=g_strdup("");
    }
    if ( !res ) {
        xmlNode *cur;
        GString *str = g_string_new("");
        cur = node->xmlChildrenNode;
        while ( cur ) {
            str=g_string_append(str, get_text(cur));
            cur = cur->next;
        }
        res = g_string_free(str, FALSE);
    }
    gchar *p;
    gunichar ch;
    p = res;
    for ( ;*p != '\0'; p = g_utf8_next_char (p) ) { // trim space
        ch = g_utf8_get_char (p);
        if ( !g_unichar_isspace (ch) )
            break;
    }
    p = g_strdup(p);
    g_free(res);
    res = p;
    p = res + strlen(res) - 1;
    for ( ;p > res; p = g_utf8_prev_char(p) ) { // trim space
        ch = g_utf8_get_char (p);
        if ( !g_unichar_isspace (ch) )
            break;
    }
    if ( p >= res ) {
        ch = g_utf8_get_char (p);
        if ( !g_unichar_isspace (ch) )
            p = g_utf8_next_char(p);
        *p = '\0';
    }
    return res;

}
static gchar *do_report_plugin_values_get_text(xmlNode *root, const gchar *name)
{
    xmlNode *node;
    node = get_child(root, name);
    if ( !node )
        return NULL;
    return get_text(node);

}
static void free_paths(DoReportPluginBlockPath *path)
{
    int i;
    for ( i = 0; i < path->children->len; i++ ) {
        GList **list;
        list = g_ptr_array_index(path->children, i);
        g_list_foreach(*list, (GFunc)free_paths, NULL);
    }
    g_ptr_array_free(path->children, TRUE);
}
static gboolean do_report_plugin_col_row_operation(DoReportPlugin *plugin, Sheet *sheet, DoReportPluginColRowOperation operation, gint start, gint count)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);

    GSList *overlap, *l, *list=NULL;
    GnmRange r;
    gboolean col_o;
    gboolean del_o;
    gboolean res = TRUE;
    col_o = (operation == COLS_DELETE) ||
            (operation == COLS_INSERT);
    del_o = (operation == COLS_DELETE) ||
            (operation == ROWS_DELETE);

    if ( col_o )
        range_init_cols(&r, sheet, start, start+count);
    else
        range_init_rows(&r, sheet, start, start+count);

    overlap = gnm_sheet_merge_get_overlap(sheet, &r);
    for ( l = overlap; res && l; l = l->next ) {
        GnmRange const *m = l->data;
        if ( ( col_o &&
               (
                 ((m->start.col <= start ) && (m->end.col > start)) ||
                 ((m->end.col >= start ) && (m->start.col < start ))
               )
             ) ||
             ( !col_o &&
               (
                ((m->start.row <= start) && (m->end.row > start)) ||
                ((m->end.row >= start) && (m->start.row < start))
               )
             )
            ) {
            GnmRange *n;
            n = g_new (GnmRange, 1);
            if ( del_o ) {
                if ( col_o )
                    range_init(n, m->start.col, m->start.row, m->end.col-count, m->end.row);
                else
                    range_init(n, m->start.col, m->start.row, m->end.col, m->end.row-count);
            }
            else {
                if ( col_o )
                    range_init(n, m->start.col, m->start.row, m->end.col+count, m->end.row);
                else
                    range_init(n, m->start.col, m->start.row, m->end.col, m->end.row+count);
            }
            list = g_slist_append(list, n);
            res = !gnm_sheet_merge_remove(sheet, m, priv->cc);
            if ( !res )
                break;

            if ( del_o &&
                 (
                   (col_o && n->start.col == start) ||
                   (!col_o && n->start.row == start)
                 )
                ) {
                GnmCellRegion *region;
                GnmPasteTarget target;
                GnmRange r;
                if ( col_o )
                    range_init(&r, start, n->start.row, start, n->end.row);
                else
                    range_init(&r, n->start.col, start, n->end.col, start);

                region = clipboard_copy_range(sheet, &r);
                if ( col_o ) {
                    r.start.col+=count;
                    r.end.col+=count;
                }
                else {
                    r.start.row+=count;
                    r.end.row+=count;
                }
                paste_target_init(&target, sheet, &r, PASTE_ALL_TYPES);
                res = !clipboard_paste_region(region, &target, priv->cc);
                cellregion_unref(region);
            }
        }
    }
    g_slist_free (overlap);

    if ( del_o ) {
        if ( col_o )
            res=!sheet_delete_cols(sheet, start, count, NULL, priv->cc);
        else
            res=!sheet_delete_rows(sheet, start, count, NULL, priv->cc);
    }
    else {
        if ( col_o )
            res=!sheet_insert_cols(sheet, start, count, NULL, priv->cc);
        else
            res=!sheet_insert_rows(sheet, start, count, NULL, priv->cc);
    }

    for ( l = list; res && l; l = l->next ) {
        GnmRange *m = l->data;
        res = !gnm_sheet_merge_add(sheet, m, FALSE, priv->cc);
        g_free(m);
    }
    g_slist_free (list);

    return res;
}
static gboolean do_report_plugin_cols_autofit(Sheet *sheet, GList *paths)
{
    GList *l;
    int i;
    gboolean res = TRUE;

    for ( l = paths; res && l; l = l->next ) {
        DoReportPluginBlockPath *path;
        path = l->data;
        if ( path->block->auto_width ) {
            GnmRange r;
            range_init_cols(&r, sheet, path->start, path->start+path->size-1);
            colrow_autofit_col(sheet, &r);
        }
        for ( i = 0; i < path->children->len; i++ ) {
            GList **list;
            list = g_ptr_array_index(path->children, i);
            res = do_report_plugin_cols_autofit(sheet, *list);
        }
    }
    return res;
}
static DoReportPluginBlockCollection *do_report_plugin_blocks_get(DoReportPlugin *plugin, Sheet *sheet, xmlNode *root)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    DoReportPluginBlock *p, *crnt = NULL;
    gboolean vertical;
    DoReportPluginBlockCollection *blocks;
    gboolean res = TRUE;

    int indx, dc = 0 , dr = 0;
    GPtrArray *cells;

    blocks = g_malloc0(sizeof(DoReportPluginBlockCollection));

    cells = sheet_cells(sheet, FALSE);
    g_ptr_array_sort(cells, (GCompareFunc)sort_cells);
    PROGRESS_STATUS("Определение листов %s %s", (priv->title) ? "отчет:":"", (priv->title) ? priv->title:"");
    for ( indx = 0; indx < cells->len; indx++ ) {
        GnmEvalPos *pos;
        GnmCell *cell;
        DoReportPluginItem *item;

        pos = g_ptr_array_index(cells, indx);
        if ( pos->eval.row && pos->eval.col )
            continue;
        if ( !pos->eval.row )
            cell = sheet_cell_get(sheet, pos->eval.col + dc, 0);
        else
            cell = sheet_cell_get(sheet, 0, pos->eval.row + dr);

        item = do_report_plugin_get_item(cell->value);

        if ( item->type ==  DO_REPORT_PLUGIN_ITEM_TYPE_BLOCK ) {
            if ( !pos->eval.row && !pos->eval.col )
                vertical = property_is(item->properties, PROPERTY_KIND, PROPERTY_VALUE_VERTICAL);
            else
                vertical = !pos->eval.row;
            p = NULL;
            if ( item->close ) {
                if ( !crnt ) {
                    res = FALSE;
                    DEBUG("Блок <%s> не открыт", item->name);
                    break;
                }
                if ( g_strcmp0(crnt->name, item->name) ) {
                    res = FALSE;
                    DEBUG("Блок <%s> закрыт до закрытия блока <%s>", item->name, crnt->name);
                    break;
                }
                p = crnt;
                crnt = crnt->parent;
            }
            if ( !p ) {
                p = g_malloc0(sizeof(DoReportPluginBlock));
                p->name = g_strdup(item->name);
                p->vertical = vertical;
                p->auto_height = property_is(item->properties, PROPERTY_HEIGHT, PROPERTY_VALUE_AUTO);
                p->auto_width = property_is(item->properties, PROPERTY_WIDTH, PROPERTY_VALUE_AUTO);
                p->start = -1;
                p->end = -1;
                if ( !crnt ) {
                    if ( vertical )
                        blocks->vertical = g_list_append(blocks->vertical, p);
                    else
                        blocks->horizontal = g_list_append(blocks->horizontal, p);
                }
            }
            if ( !item->close ) {
                if ( crnt ) {
                    if ( crnt->vertical != p->vertical ) {
                        res = FALSE;
                        DEBUG("Блок <%s> не закрыт", crnt->name);
                        break;
                    }
                    crnt->children = g_list_append(crnt->children, p);
                    p->parent = crnt;
                }
                crnt = p;
            }
            if ( !item->close )
                p->start = ( p->vertical ) ? pos->eval.col + 1 + dc : pos->eval.row + 1 + dr;
            else
                p->end =  ( p->vertical ) ? pos->eval.col - 1 + dc: pos->eval.row - 1 + dr;
        }
        do_report_plugin_item_free(item);
    }
    if ( !res ) {
        free_collection1(blocks);
        return NULL;
    }
    return blocks;
}
static gboolean do_report_plugin_blocks_make(DoReportPlugin *plugin, Sheet *sheet, xmlNode *root, DoReportPluginBlockCollection *blocks)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    DoReportPluginBlock *p;
    GList *l;
    gboolean res = TRUE;

    int dc = 0 , dr = 0;
    int i;
    GList *paths = NULL;
    dr = 0, dc = 0;
    for ( i = 0; res && i < 2; i++ ) {
        GList *bl, *bl1;
        bl = (i) ? blocks->horizontal : blocks->vertical;
        bl1 = (!i) ? blocks->horizontal : blocks->vertical;
        PROGRESS_STATUS("Расчет %s %s", (priv->title) ? "отчета:":"", (priv->title) ? priv->title:"");
        for ( l = bl; res && l; l = l->next ) {
            p = l->data;
            if ( p->end == -1 ) {
                res = FALSE;
                DEBUG("Блок <%s> не закрыт", p->name);
                break;
            }
            res = do_report_plugin_block_make(plugin, sheet, p, root, &dr, &dc, bl1, &paths );
        }
    }
    if ( res )
        res = do_report_plugin_cols_autofit(sheet, paths);
    g_list_foreach(paths, (GFunc)free_paths, NULL);
    g_list_free(paths);
    return res;
}
static gint do_report_plugin_blocks_get_total(DoReportPlugin *plugin, Sheet *sheet, xmlNode *root, DoReportPluginBlockCollection *blocks)
{
    //DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    GList *l;
    gint res = 0;

    for ( l = blocks->horizontal; l; l = l->next )
        res += do_report_plugin_block_get_total(plugin, l->data, root);
    for ( l = blocks->vertical; l; l = l->next )
        res += do_report_plugin_block_get_total(plugin, l->data, root);

    return res;
}
static gboolean do_report_plugin_sheet_make(DoReportPlugin *plugin, Sheet *sheet, xmlNode *root, DoReportPluginBlockCollection *blocks)
{
    //DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    gboolean res = TRUE;
    DoReportPluginFillInfo fill;

    if ( !do_report_plugin_blocks_make(plugin, sheet, root, blocks) )
        return FALSE;

    fill.root = root;
    fill.pass = FALSE;
    fill.run = FALSE;
    fill.plugin = plugin;
    sheet_foreach_cell_in_range(sheet, CELL_ITER_IGNORE_NONEXISTENT,
                                0,0,
                                sheet->cols.max_used,sheet->rows.max_used,
                                (CellIterFunc)do_report_plugin_cell_fill, &fill);

    /*if ( res ) {
        GnmRange r;
        //range_init_rows(&r, sheet, 0, sheet->rows.max_used);
        range_init_rows(&r, sheet, 9, sheet->rows.max_used);
        res = do_report_plugin_rows_autofit(sheet, &r);
    }*/

    return res;


}
static xmlNode *do_report_plugin_sheet_get_root(DoReportPlugin *plugin, Sheet *sheet)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    xmlNode *root;
    gchar *root_name;

    root = xmlDocGetRootElement(priv->values_);
    root_name = g_utf8_strup((gchar*)root->name, -1);

    if ( !g_strcmp0(root_name, SHEETS_ALIAS) ) {
        root = root->xmlChildrenNode;
        gchar *name;
        name = g_utf8_strup(sheet->name_unquoted, -1);
        while ( root ) {
            gchar *el_name;
            el_name = g_utf8_strup((gchar *)root->name, -1);
            if ( (root->type == XML_ELEMENT_NODE) && !g_strcmp0(el_name, name) ) {
                g_free(el_name);
                break;
            }
            g_free(el_name);
            root = root->next;
        }
        g_free(name);
    }
    g_free(root_name);
    return root;
}
static gint do_report_plugin_sheet_get_total(DoReportPlugin *plugin, Sheet *sheet, xmlNode *root, DoReportPluginBlockCollection *blocks)
{
    //DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    gint res = TRUE;

    res = do_report_plugin_blocks_get_total(plugin, sheet, root, blocks);
    return res;
}
static void do_report_plugin_block_path_shift(GList *paths, gint col)
{
    GList *l;
    int k;
    for ( l = paths; l ; l = l->next ) {
        DoReportPluginBlockPath *path;
        path = l->data;
        path->start += col;
        for ( k = 0; k < path->children->len; k++ ) {
            GList **list=g_ptr_array_index(path->children, k);
            do_report_plugin_block_path_shift(*list, col);
        }
    }
}
static GSList *range_split_unmerge(Sheet *sheet, GnmRange const *r)
{
    GSList *overlap, *l;
    GSList *res = NULL;
    GnmRange *n;

    overlap = gnm_sheet_merge_get_overlap(sheet, r);
    for ( l = overlap; l; l=l->next ) {
        GnmRange const *m = l->data;
        if ( !range_contained(m, r) ) {
            GSList *l1, *l2;
            l1=range_split_ranges(m,r);
            for ( l2=l1; l2; l2=l2->next) {
                n = l2->data;
                if ( !range_contained(n,m) )
                    res = g_slist_concat(res, range_split_unmerge(sheet, n));
                g_free(n);
            }
            break;
        }
    }
    if ( !res ) {
        n = g_new (GnmRange, 1);
        *n = *r;
        res = g_slist_append(res, n);
    }
    g_slist_free (overlap);
    return res;
}
static GSList *do_report_plugin_clipboard_copy_range(DoReportPlugin *plugin, Sheet *sheet, GnmRange const *r)
{
    //DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    GSList *list, *l;
    GSList *res = NULL;

    list = range_split_unmerge(sheet, r);
    for ( l = list; l; l=l->next ) {
        DoReportPluginClipboardItem *item;
        item = g_new(DoReportPluginClipboardItem, 1);
        res = g_slist_append(res, item);
        item->src = l->data;
        item->region = clipboard_copy_range(sheet, item->src);
    }
    g_slist_free (list);
    return res;
}

static gboolean do_report_plugin_block_make(DoReportPlugin *plugin, Sheet *sheet,
                                                  DoReportPluginBlock *block, xmlNode *root,
                                                  gint *dr, gint *dc,
                                                  GList *blocks, GList **paths)
{
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    xmlNode *child;

    GSList *clipboard, *c;
    GnmPasteTarget target;
    DoReportPluginBlockPath *path;
    DoReportPluginFillInfo fill;
    GList *l;
    GnmRange r ;
    gint size;
    gint dc_,dr_;
    gint number;
    gboolean res = TRUE;

    dc_=*dc;dr_=*dr;

    size = block->end-block->start+1;
    if ( block->vertical )
        range_init_cols(&r, sheet, block->start + dc_, block->end + dc_);
    else
        range_init_rows(&r, sheet, block->start + dr_, block->end + dr_);

    clipboard = do_report_plugin_clipboard_copy_range(plugin, sheet, &r);
    number = 0;
    for ( child = root->children; res && child; child = child->next ) {
        if ( (child->type == XML_ELEMENT_NODE) &&
             (!g_strcmp0((gchar*)child->name, block->name)) ) {
            priv->crnt_record++;
            PROGRESS_PROGRESS(priv->crnt_record, priv->total_record);
            if ( CANCELLED ) { res = FALSE; break; };

            number++;

            if ( block->vertical ) {
                path = g_malloc0(sizeof(DoReportPluginBlockPath));
                *dc=*dc+size;
                path->block = block;
                path->number = number;
                path->start = *dc + block->start;
                path->vertical = TRUE;
                *paths = g_list_append(*paths, path);
                path->children = g_ptr_array_new();

                res = do_report_plugin_col_row_operation(plugin, sheet, COLS_INSERT, *dc+block->start, size);

            }
            else {
                *dr=*dr+size;
                res = do_report_plugin_col_row_operation(plugin, sheet, ROWS_INSERT, *dr+block->start, size);

            };
            for ( c = clipboard; res && c; c=c->next ) {
                DoReportPluginClipboardItem *item;
                item = c->data;
                r = *item->src;
                if ( block->vertical ) {
                    r.start.col = block->start + *dc;
                    r.end.col = block->end + *dc;
                }
                else {
                    r.start.row = block->start + *dr;
                    r.end.row = block->end + *dr;
                }

                paste_target_init(&target, sheet, &r, PASTE_ALL_TYPES);
                res = !clipboard_paste_region(item->region, &target, priv->cc);
            }

            if ( block->vertical )
                range_init_cols(&r, sheet, block->start + *dc, block->end + *dc);
            else
                range_init_rows(&r, sheet, block->start + *dr, block->end + *dr);

            for ( l = block->children; res && l ; l = l->next ) {
                DoReportPluginBlock *child_block;
                child_block = l->data;
                if ( block->vertical ) {
                    GList **list = g_malloc0(sizeof(GList **));
                    *list = NULL;
                    g_ptr_array_add(path->children, list);
                    res = do_report_plugin_block_make(plugin, sheet, child_block, child, dr, dc, blocks, list);
                }
                else
                    res = do_report_plugin_block_make(plugin, sheet, child_block, child, dr, dc, blocks, paths);
            }

            if ( !res )
                break;

            if ( block->vertical ) {
                path->size = block->start + *dc - path->start + size;
                r.end.col = block->end + *dc;
            }
            else
                r.end.row = block->end + *dr;

            fill.root = child;
            fill.blocks = blocks;
            fill.pass = block->vertical;
            fill.run = !block->vertical;
            fill.paths = *paths;
            fill.plugin = plugin;
            sheet_foreach_cell_in_range(sheet, CELL_ITER_IGNORE_NONEXISTENT,
                                    r.start.col, r.start.row,
                                    r.end.col,  r.end.row,
                                    (CellIterFunc)do_report_plugin_cell_fill, &fill);

            //if ( !block->vertical && block->auto_height )
                //do_report_plugin_rows_autofit(sheet, &r);
                //colrow_autofit_row(sheet, &r);
        }
    }
    for ( c = clipboard; c; c=c->next ) {
        DoReportPluginClipboardItem *item;
        item = c->data;
        g_free(item->src);
        cellregion_unref(item->region);
        g_free(item);

    }
    g_slist_free(clipboard);

    if ( !res )
        return res;

    if ( block->vertical ) {
        res = do_report_plugin_col_row_operation(plugin, sheet, COLS_DELETE, block->start + *dc + size, 1);
        if ( res )
            res = do_report_plugin_col_row_operation(plugin, sheet, COLS_DELETE, block->start + dc_-1, size + 1);
        *dc=*dc-size-2;
        do_report_plugin_block_path_shift(*paths, -size-1);
    }
    else {
        res = do_report_plugin_col_row_operation(plugin, sheet, ROWS_DELETE, block->start + *dr + size  , 1);
        if ( res )
            res = do_report_plugin_col_row_operation(plugin, sheet, ROWS_DELETE, block->start + dr_-1, size + 1);
        *dr=*dr-size-2;
        if ( !block->vertical && block->auto_height ) {
            GnmRange r;
            range_init_rows(&r, sheet, block->start + dr_ -1, block->start + *dr + size);
            //colrow_autofit_row(sheet, &r);
            res = do_report_plugin_rows_autofit(sheet, &r);
        }
    }
    return res;
}
static gint do_report_plugin_block_get_total(DoReportPlugin *plugin, DoReportPluginBlock *block, xmlNode *root)
{
    //DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (plugin);
    xmlNode *child;
    gint res;

    res = 0;
    for ( child = root->children; child; child = child->next ) {
        if ( (child->type == XML_ELEMENT_NODE) &&
             (!g_strcmp0((gchar*)child->name, block->name)) ) {
            GList *l;
            res++;
            for ( l = block->children; l ; l = l->next ) {
                DoReportPluginBlock *child_block;
                child_block=l->data;
                res+=do_report_plugin_block_get_total(plugin, child_block, child);
            }
        }
    }
    return res;
}
static GList *do_report_plugin_path_get(GList *paths, gint col, gint row)
{
    gboolean contain;
    GList *l, *res = NULL;

    for ( l = paths; l; l = l->next ) {
        DoReportPluginBlockPath *path;
        path = l->data;
        if ( path->vertical )
            contain = ( (path->start <= col) && (col - path->start < path->size) );
        else
            contain = ( (path->start <= row) && (row - path->start < path->size) );
        if ( contain ) {
            res = g_list_append(res, path);
            int i;
            for ( i = 0; i < path->children->len; i++ ) {
                GList **list;
                list = g_ptr_array_index(path->children, i);
                l = do_report_plugin_path_get(*list, col, row);
                if ( l ) {
                    res = g_list_concat(res, l);
                    break;
                }
            }
            break;
        }
    }
    return res;
}
static xmlNode *do_report_plugin_block_run_parted(GList *paths, xmlNode *root, gint col, gint row)
{
    xmlNode *node, *new_node, *res = root;
    GList *p, *l;
    p = do_report_plugin_path_get(paths, col, row);

    node = root;
    for ( l = p; l; l=l->next ) {
        DoReportPluginBlockPath *path;
        path = l->data;
        int count;
        count = path->number;
        res = NULL;
        for ( new_node = node->children; new_node ; new_node = new_node->next ) {
            if ( (node->type == XML_ELEMENT_NODE) &&
                 (!g_strcmp0(path->block->name, (gchar*) new_node->name)) ) {
                    count--;
                if ( count == 0 ) {
                    node=new_node;
                    res = new_node;
                    break;
                }
            }
        }
        if ( !res )
            return root;
    }
    return res;
}

static gchar *path_str(xmlNode *root)
{
    xmlNode *node;
    GList *text = NULL;
    GString *res;

    for ( node = root; node ; node = node->parent )
        if ( node->type == XML_ELEMENT_NODE)
            text = g_list_append(text, g_strdup((gchar*)node->name));

    res = g_string_new("");
    for ( ;text; text=text->prev )
        g_string_append(res, text->data);
    g_list_free(text);

    return g_string_free(res, FALSE);
}

static GnmValue *do_report_plugin_cell_fill(GnmCellIter const *iter, DoReportPluginFillInfo *fill)
{
    GnmCell *cell;
    DoReportPluginItem *item = NULL;
    gchar *value;
    xmlNode *root = NULL;
    DoReportPluginPrivate *priv = DO_REPORT_PLUGIN_GET_PRIVATE (fill->plugin);

    if ( fill->pass ) {
        GList *l;
        for ( l = fill->blocks; l; l=l->next ) {
            DoReportPluginBlock *block;
            block = l->data;
            if ( block->vertical ) {
                if ( block->start <= iter->pp.eval.col && block->end >= iter->pp.eval.col )
                    return NULL;
            }
            else {
                if ( block->start <= iter->pp.eval.row && block->end >= iter->pp.eval.row )
                    return NULL;
            }
        }
    }
    cell = iter->cell;
    if ( fill->run )
        root = do_report_plugin_block_run_parted(fill->paths, fill->root, iter->pp.eval.col, iter->pp.eval.row);
    if ( !root )
        root = fill->root;
    item = do_report_plugin_get_item(cell->value);
    switch ( item->type ) {
        case DO_REPORT_PLUGIN_ITEM_TYPE_EXP:
        case DO_REPORT_PLUGIN_ITEM_TYPE_VAR: {
            value = do_report_plugin_get_value(item, root, cell->value);
            if ( value )
                sheet_cell_set_text(cell, value, NULL);
            else {
                DEBUG("Выражение [%s RC%d:%d] \"%s\" не разобранно. Путь %s", cell_name(cell), iter->pp.eval.col, iter->pp.eval.row, value_get_as_string(cell->value), path_str(root));
                if ( !priv->debug )
                    sheet_cell_set_text(cell, "", NULL);
            }
            g_free(value);
            break;
        }
        case DO_REPORT_PLUGIN_ITEM_TYPE_BLOCK:
            if ( !priv->debug )
                sheet_cell_set_text(cell, "", NULL);
            DEBUG("Блок [%s RC%d:%d]<%s>  неправильно расположен.Путь %s", cell_name(cell), iter->pp.eval.col, iter->pp.eval.row, item->name, path_str(root));
            break;
        default:
            break;
    }
    do_report_plugin_item_free(item);

    return NULL;
}
static GnmValue *do_report_plugin_cell_autofit_heigth(GnmCellIter const *iter, Sheet *shet);
static gboolean do_report_plugin_rows_autofit(Sheet *sheet, GnmRange *range)
{
    int i;
    for ( i = range->start.row; i <= range->end.row; i++ )
        sheet_row_set_size_pixels(sheet, i, 10, TRUE);

    sheet_foreach_cell_in_range(sheet, CELL_ITER_IGNORE_NONEXISTENT,
                                range->start.col,range->start.row,
                                range->end.col,range->end.row,
                                (CellIterFunc)do_report_plugin_cell_autofit_heigth, sheet);

/*    int size;
    size = sheet_row_size_fit_pixels(sheet, range->start.row, range->start.col, range->end.col, FALSE);
    sheet_row_set_size_pixels(sheet, range->start.row, size, FALSE);

    colrow_autofit_row(sheet, range);*/
    return TRUE;
}
#define COL_MARGIN (GNM_COL_MARGIN + 2)
#define ROW_MARGIN (GNM_ROW_MARGIN + 2)

static GnmValue *do_report_plugin_cell_autofit_heigth(GnmCellIter const *iter, Sheet *sheet)
{
    GnmCell        *cell;
    const GnmStyle *style;
    PangoLayout    *layout;
    GdkScreen      *screen;
    PangoContext   *context;
    PangoFontDescription *desc;
    const gchar    *text;
    const GnmValue *value;
    ColRowInfo  *col_info, *row_info;
    gint width, height;

    cell = iter->cell;
    screen = gdk_screen_get_default ();
    context = gdk_pango_context_get_for_screen (screen);
    style = gnm_cell_get_style(cell);
    value = cell->value;
    text = value_get_as_string(value);
    col_info = iter->ci;
    row_info = iter->ri;

    const char *name;
    int size;

    if ( gnm_cell_is_merged(cell) )
        return NULL;

/*    if ( iter->pp.eval.row != 17 )
        return NULL;*/

    desc = pango_font_description_new ();
    pango_font_description_set_family (desc, name = gnm_style_get_font_name(style));
    pango_font_description_set_weight (desc, gnm_style_get_font_bold(style)
                                            ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL);
    pango_font_description_set_style (desc, gnm_style_get_font_italic(style)
                                            ? PANGO_STYLE_ITALIC : PANGO_STYLE_NORMAL);
    pango_font_description_set_size (desc, size = gnm_style_get_font_size(style)*PANGO_SCALE);
                                     //size_pts * PANGO_SCALE);

    pango_context_set_language (context, gtk_get_default_language ());

    pango_context_set_font_description (context, desc);
    pango_context_set_base_dir (context, PANGO_DIRECTION_LTR);
    layout = pango_layout_new(context);


    //pango_layout_set_spacing (layout, 3 * PANGO_SCALE);
    pango_layout_set_text(layout, text, strlen(text));
    pango_layout_set_wrap(layout, gnm_style_get_wrap_text(style) ? PANGO_WRAP_WORD_CHAR : 0);
    pango_layout_set_width(layout, (col_info->size_pixels -2*COL_MARGIN - 1)*PANGO_SCALE );

    height = pango_layout_get_height(layout);
    pango_layout_get_size(layout, &width, &height);
    height = height/PANGO_SCALE + 2*ROW_MARGIN + 1;
    width = width/PANGO_SCALE;

    if ( width > 0 && height > row_info->size_pixels ) {
//        row_info->size_pixels = height;
        sheet_row_set_size_pixels(sheet, iter->pp.eval.row, height, TRUE);
    }

    g_object_unref (layout);


    return NULL;

}

