#include <stdio.h>
#include <stdlib.h>
#include <dolib.h>
#include <locale.h>
#include <libgnumeric.h>
#include <gtk/gtk.h>
#include <gnm-plugin.h>
#include <workbook-view.h>
#include <command-context-stderr.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

typedef struct {
    GOCmdContext *cc;
    GOIOContext *io_context;
    WorkbookView *wbv;
    WBCGtk *wc;
    Workbook *wb;
    char *ssconvert_import_encoding;
} plugin_t;

xmlDoc *sad()
{
    xmlDoc  *doc;
    xmlNode *root, *node;

    doc = xmlNewDoc(BAD_CAST "1.0");
    root = xmlNewNode(NULL, BAD_CAST "template");
    xmlDocSetRootElement(doc, root);

    xmlNewChild(root, NULL, BAD_CAST "node1",
                BAD_CAST "content of node 1");

    xmlNewChild(root, NULL, BAD_CAST "node2",
                BAD_CAST "content of node 2");


    node =
        xmlNewChild(root, NULL, BAD_CAST "node3",
                    BAD_CAST "this node has attributes");
    xmlNewProp(node, BAD_CAST "attribute", BAD_CAST "yes");
    xmlNewProp(node, BAD_CAST "foo", BAD_CAST "bar");


    xmlSaveFormatFileEnc("file.xml", doc, "UTF-8", 1);

    //xmlFreeDoc(doc);
    return doc;
};

plugin_t *do_template_make_and_show(const gchar *filename, xmlDoc *values, const gchar *app_name);
static plugin_t *plugin_init(const gchar *app_name)
{
    plugin_t *plugin;

    plugin=g_malloc0(sizeof(plugin_t));

    gnm_init();

	GOErrorInfo	*plugin_errs;

	gnm_pre_parse_init (1, &app_name);

    plugin->cc = cmd_context_stderr_new ();
    plugin->io_context = go_io_context_new (plugin->cc);

	gnm_plugins_init (GO_CMD_CONTEXT (plugin->cc));
	go_plugin_db_activate_plugin_list (
		go_plugins_get_available_plugins (), &plugin_errs);

	if ( plugin_errs ) {
		/* FIXME: What do we want to do here? */
		go_error_info_free (plugin_errs);
		g_free(plugin);
		return NULL;
	}

    return plugin;
};

void plugin_free(plugin_t *plugin)
{
    if ( plugin->wb )
        g_object_unref(plugin->wb);
    g_object_unref (plugin->io_context);
	g_object_unref (plugin->cc);
	gnm_shutdown ();
	gnm_pre_parse_shutdown ();
}

int main(int argc, gchar *argv[])
{

    setlocale(LC_ALL,"");
    do_log_set_stdout();

    /* Initialize GTK+ */
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
    gtk_init (&argc, &argv);
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

    /* Create the main window */
    plugin_t *plugin;

    plugin = do_template_make_and_show("torg-12.xls", sad(), argv[0]);

    gtk_main ();

    //plugin_free(plugin);

    return 0;

}


static void plugin_destroy(GObject *object, plugin_t *plugin)
{
	plugin_free(plugin);
	gtk_main_quit();
}

plugin_t *do_template_make_and_show(const gchar *filename, xmlDoc *values, const gchar *app_name)
{
    plugin_t *plugin;
    GOFileOpener *fo = NULL;
	//GOErrorInfo	*plugin_errs;

    plugin = plugin_init(app_name);
    if ( !plugin )
        return NULL;

	char *uri=go_shell_arg_to_uri(filename);

    plugin->wbv = wb_view_new_from_uri  (uri, fo,
                        plugin->io_context, plugin->ssconvert_import_encoding);
    g_free(uri);

    if ( !plugin->wbv ) {
        go_io_error_display (plugin->io_context);
        plugin_free(plugin);
        return NULL;
    }

    plugin->wb = wb_view_get_workbook (plugin->wbv);

	//Sheet *sheet = wb_view_cur_sheet (wbv);

	plugin->wc = wbc_gtk_new (NULL, plugin->wb, NULL, NULL);

	//plugin->win=wbcg_toplevel(plugin->wc);

    g_signal_connect (wbcg_toplevel(plugin->wc), "destroy", G_CALLBACK(plugin_destroy), plugin);



  //vbox = gtk_vbox_new (TRUE, 6);
 // gtk_container_add (GTK_CONTAINER (win), vbox);

  //gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0);

  /* Enter the main loop */
  //gtk_widget_show_all (win);

	return plugin;

}
