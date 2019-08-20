
static gboolean window_state_event(GtkWidget* window, GdkEventWindowState* event);
static void do_common_open_web(GtkWidget *window, const gchar *uri);


    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

static void do_common_open_web_title_change(WebKitWebView  *web_view,
                                                        WebKitWebFrame *frame,
                                                        gchar          *title,
                                                        DoView *window)
{
    g_object_set(window, "title", title, NULL);
}
static gboolean do_common_open_web_new_tab_request(WebKitWebView             *web_view,
                                                        WebKitWebFrame            *frame,
                                                        WebKitNetworkRequest      *request,
                                                        WebKitWebNavigationAction *navigation_action,
                                                        WebKitWebPolicyDecision   *policy_decision,
                                                        GtkWidget *window)
{
    const gchar *uri;
    uri =  webkit_network_request_get_uri (request);
    do_common_open_web(window, uri);
    return TRUE;
}
void do_open_web(const gchar *uri)
{
   do_common_open_web(GTK_WIDGET(domino_get_main_window()), uri);

}

static void do_common_open_web(GtkWidget *window, const gchar *uri)
{
    GtkNotebook *nb;
    DoView *view;

    nb = GTK_NOTEBOOK (do_window_get_notebook (window));
    g_return_if_fail (nb != NULL);

    view = DO_VIEW(do_html_view_new());
    do_end_long_operation(GTK_WIDGET(window));
    if ( !view )
        return;
    do_html_set_entry(DO_HTML_VIEW(view), entry);
    do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
    do_view_do_grab_focus(DO_VIEW(view));
    g_object_set(view, "uri", uri, "title", "Поиск в интернет", NULL);
    /*g_signal_connect(WEBKIT_WEB_VIEW(do_html_get_view(DO_HTML_VIEW(view))),
                     "navigation-requested",
                      G_CALLBACK(do_common_open_web_navi_response), window);*/
    g_signal_connect(WEBKIT_WEB_VIEW(do_html_get_view(DO_HTML_VIEW(view))),
                      "title-changed",
                      G_CALLBACK(do_common_open_web_title_change), view);

    g_signal_connect(WEBKIT_WEB_VIEW(do_html_get_view(DO_HTML_VIEW(view))),
    "new-window-policy-decision-requested",
                      G_CALLBACK(do_common_open_web_new_tab_request), window);
}

gboolean idle(GtkWidget *window)
{
    DOMINO_LOCAL_SET("Webkit", "proxy-string", "http://proxy.maria-ra.ru:3128", NULL);
    do_common_open_web(window, "https://www.google.ru/");
    //do_common_open_web(window, "http://go.mail.ru/");
    return FALSE;
}
static void
activate (GtkApplication *app,
          gpointer        user_data)
{
    if ( !DOMINO_INIT ) {
        DOMINO_SHOW_ERROR("Окружение не инициализировано");
        return;
    }

  GtkWidget *window;

  window = gtk_application_window_new (app);
  domino_set_main_window(GTK_WINDOW(window));
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_window_set_icon_name (GTK_WINDOW (window), "bookcase");
  g_signal_connect (window, "window-state-event",
                      G_CALLBACK (window_state_event), NULL);
  g_signal_connect (window, "configure-event",
                      G_CALLBACK (do_window_configure_event_cb), OBJECT_ROOT_PATH);
  DOMINO_PROFILE_OBJECT_INIT(G_OBJECT(window), OBJECT_ROOT_PATH,
              "window-state", DEFAULT_WINDOW_STATE,
              "window-size", DEFAULT_WINDOW_SIZE,
              "window-position", DEFAULT_WINDOW_POSITION,
               NULL);
  GtkWidget *header;
  header = gtk_header_bar_new();
  gtk_window_set_titlebar(GTK_WINDOW(window), header);
  gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);
  //GtkWidget *entry;
  entry = gtk_entry_new();
  gtk_widget_set_hexpand(GTK_WIDGET(entry), TRUE);
  gtk_entry_set_icon_from_icon_name(GTK_ENTRY(entry), GTK_ENTRY_ICON_PRIMARY, "edit-find-symbolic");
  gtk_header_bar_set_custom_title(GTK_HEADER_BAR(header), entry);

  //button_box = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
  //gtk_container_add (GTK_CONTAINER (window), button_box);

  //button = gtk_button_new_with_label ("Hello World");
  //g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
  //g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);
  //gtk_container_add (GTK_CONTAINER (button_box), button);

  DoNotebook *notebook = DO_NOTEBOOK (g_object_new (DO_TYPE_NOTEBOOK, NULL));
  gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET(notebook));
  do_notebook_set_show_tabs(DO_NOTEBOOK(notebook), FALSE);

//surface = gdk_window_create_similar_surface(gdkwin, cairo_content,
//width, heigth)
//ctx = cairo_create(surface)
//rsvg_handle_render_cairo(svg, ctx)
//image = gtk_image_new_from_surface(surface)
  gtk_widget_show_all (window);
  g_idle_add((GSourceFunc)idle, window);
}
static gboolean window_state_event(GtkWidget* window, GdkEventWindowState* event)
{
    gchar *new_state = NULL;
    if (event->new_window_state & GDK_WINDOW_STATE_ICONIFIED)
        new_state = g_strdup("Minimized");
    else if (event->new_window_state & GDK_WINDOW_STATE_MAXIMIZED)
        new_state = g_strdup("Maximized");
    else if (event->new_window_state & GDK_WINDOW_STATE_FULLSCREEN)
        new_state = g_strdup("Fullscreen");
    else
        new_state = g_strdup("Normal");
    DOMINO_PROFILE_SET(OBJECT_ROOT_PATH, "DoWindow", "window-state", new_state, NULL);
    g_free(new_state);
    return FALSE;
}
