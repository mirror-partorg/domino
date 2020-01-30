#include "domino.h"
#include "version.h"
#include "do_config.h"
#include "do_message.h"
#include <stdarg.h>
#include <glib.h>
#include <errno.h>
#include <gtk/gtk.h>
#include <locale.h>
#include "../config/config.h"
#ifdef _WIN32
#include <windows.h>
#endif
#ifdef USE_IM
#include <loudmouth/loudmouth.h>
#endif

#define DOMINO_NAME "domino"
#define DOMINO_DEFAULT_UI "default.ui"
#define DOMINO_APPLICATION_NAME "Справочник"

typedef enum {
    DOMINO_ETC_DIR,
    DOMINO_PROFILE_DIR
} domino_dir_enum_t;

typedef struct {
    const char *name;
    domino_dir_enum_t dir;
    gboolean save;
    gboolean empty;
} domino_config_t;

static domino_config_t domino_config[DOMINO_CONFIG_N] =
           {
            {"common.xml", DOMINO_ETC_DIR, FALSE, FALSE},
            {"profile.xml", DOMINO_PROFILE_DIR, TRUE, TRUE},
            {"local.xml", DOMINO_PROFILE_DIR, FALSE, TRUE}
           };
static do_config_t *config[DOMINO_CONFIG_N] = {NULL, };
static gchar *config_filename[DOMINO_CONFIG_N] = {NULL, };

typedef struct {
    GtkWidget *win;
    gboolean in_thread;
    char **argv;
} UpdateInfo;
static UpdateInfo update_info;

#ifdef _WIN32
static BOOL WINAPI CheckTokenMembership_Stub( HANDLE TokenHandle, PSID SidToCheck, PBOOL IsMember )
{
    typedef BOOL (WINAPI *CheckTokenMembership_PROC)( HANDLE, PSID, PBOOL );

    static HMODULE  hModule = NULL;
    static CheckTokenMembership_PROC    pCheckTokenMembership = NULL;

    if ( !hModule )
    {
        /* SAL is always linked against ADVAPI32 so we can rely on that it is already mapped */

        hModule = GetModuleHandleA( "ADVAPI32.DLL" );

        pCheckTokenMembership = (CheckTokenMembership_PROC)GetProcAddress( hModule, "CheckTokenMembership" );
    }

    if ( pCheckTokenMembership )
        return pCheckTokenMembership( TokenHandle, SidToCheck, IsMember );
    else
    {
        SetLastError( ERROR_CALL_NOT_IMPLEMENTED );
        return FALSE;
    }

}
BOOL IsRunAsAdmin()
{
    BOOL fIsRunAsAdmin = FALSE;
    DWORD dwError = ERROR_SUCCESS;
    PSID pAdministratorsGroup = NULL;

    // Allocate and initialize a SID of the administrators group.
    SID_IDENTIFIER_AUTHORITY NtAuthority = {SECURITY_NT_AUTHORITY};
    if (!AllocateAndInitializeSid(
        &NtAuthority,
        2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &pAdministratorsGroup))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Determine whether the SID of administrators group is enabled in
    // the primary access token of the process.
    if (!CheckTokenMembership_Stub(NULL, pAdministratorsGroup, &fIsRunAsAdmin))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (pAdministratorsGroup)
    {
        FreeSid(pAdministratorsGroup);
        pAdministratorsGroup = NULL;
    }

    // Throw the error if something failed in the function.
    if (ERROR_SUCCESS != dwError)
    {
        do_log(LOG_ERR, "%s", strerror(dwError));
    }

    return fIsRunAsAdmin;
}
#endif
int dominox_init()
{
    do_gtk_log_init();
    //do_message_init();
    setlocale(LC_ALL, "");
    gint i;
    for (i = 0; i < DOMINO_CONFIG_N; i++)
        domino_config_read(i);

    DOMINO_LOCAL_SET("list-goods-view", "fields", "ocnsfPR", NULL);//todo
    DOMINO_LOCAL_SET("main", "websearch", "https://go.mail.ru", NULL);//todo

    return TRUE;
}
static GtkWindow *main_window = NULL;
static GdkWindow *back_window = NULL;
static GList *windows = NULL;

inline GtkWindow *domino_get_main_window()
{
    return main_window;
}
inline GdkWindow *domino_get_back_window()
{
    return back_window;
}
static void destroy_(GtkWidget *w, GtkDialog *dialog)
{
    windows = g_list_remove(windows, dialog);
}
int domino_dialog_run(GtkDialog *dialog)
{
    int res;
    windows = g_list_append(windows, dialog);
#ifdef gtk_dialog_run
#undef gtk_dialog_run
#endif
    g_signal_connect(dialog, "destroy", G_CALLBACK(destroy_), dialog);
    res = gtk_dialog_run(dialog);
#define gtk_dialog_run domino_dialog_run
    return res;
}
/*
GtkWindow *domino_get_top_window(int *dialog)
{
    if ( !windows ) {
        *dialog = FALSE;
        return main_window;
    }
    else {
        *dialog = TRUE;
        GList *l;
        l = g_list_nth(windows, g_list_length(windows)-1);
        return l->data;
    }
}
*/
int domino_back_window_focus()
{
    if ( back_window ) {
        guint32    timestamp = GDK_CURRENT_TIME;

        gdk_window_focus(back_window,timestamp);
        return TRUE;
    }
    return FALSE;
}
#ifdef CASH
#include <X11/Xlib.h>
#include <X11/keysym.h>
void key_event(Display *display, Window *win,
               Window *win_root, int keycode,
               int is_press){
    //GdkKeyEvent event;
    XKeyEvent event;

    /*is_press != 0 - нажать клавишу (KeyPress),
       is_press == 0 - отжать (KeyRelease) :) */
    event.type        = is_press != 0 ? KeyPress : KeyRelease;
    event.display     = display;
    event.window      = *win;
    event.root        = *win_root;
    event.subwindow   = None;
    event.time        = CurrentTime;
    event.x           = 1;
    event.y           = 1;
    event.x_root      = 1;
    event.y_root      = 1;
    event.same_screen = True;
    event.keycode     = XKeysymToKeycode(display, keycode);
    event.state       = 0;

    XSendEvent(display, *win, True, KeyPressMask, (XEvent *)&event);
}
int KEYCODE[] ={XK_0,XK_1,XK_2,XK_3,XK_4,XK_5,XK_6,XK_7,XK_8,XK_9};
int domino_send_barcode_eventkey_to_crntwin(char *barcode)
{
    Display *display;
    Window win_root, win_focus;
    int revert;
    if((display = XOpenDisplay(0)) == NULL)
        return FALSE;
    win_root = XDefaultRootWindow(display);
    XGetInputFocus(display, &win_focus, &revert);

    char *ch;
    for ( ch = barcode; ch && ch[0] != '\0' ; ch++ )
        if ( (ch[0] >= '0') && (ch[0] <='9') ) {
            key_event(display, &win_focus, &win_root, KEYCODE[(int)(ch[0]-'0')], 1);
            key_event(display, &win_focus, &win_root, KEYCODE[(int)(ch[0]-'0')], 0);
        }
    key_event(display, &win_focus, &win_root, XK_Return, 1);
    key_event(display, &win_focus, &win_root, XK_Return, 0);


    XCloseDisplay(display);
    return TRUE;
}
#endif
#ifdef FULL_SCREEN
#ifdef GO_BACK
static gboolean sad(gpointer data)
{
    do_log(LOG_INFO, "go back");
    domino_back_window_focus();
    return FALSE;
}
#endif
#endif
void domino_set_main_window(GtkWindow *window)
{
    main_window = window;
#ifdef CASH
    GdkScreen *screen;
    screen = gdk_screen_get_default();
    int i;
    for ( i = 0; i < 20; i++ ) {
        back_window = gdk_screen_get_active_window(screen);
        if ( back_window ) break;
        do_log(LOG_INFO, "sleep wait back");
        DO_SEC_SLEEP(1);
    }
#endif
#ifdef FULL_SCREEN
#ifdef GO_BACK
    do_log(LOG_INFO, "go back time");
    g_timeout_add(500, sad, NULL);
#endif
#endif
}
void domino_update_main_window_title()
{
//    gchar *value, *title;
//    value = domino_get_default_location_name();
//    title = g_strdup_printf("%s %s", DOMINO_APPLICATION_NAME, value);
//    gtk_window_set_title(main_window, title);
//    do_free(value);do_free(title);
}


void dominox_finalize()
{
    gint i;
    for (i = 0; i < DOMINO_CONFIG_N; i++)
        if (domino_config[i].save)
            domino_config_save(i, TRUE);
}

gchar *domino_config_filename(gint type)
{
    return config_filename[type];
}

GFileInputStream *domino_get_config_stream(gint type)
{
    GFile *f;
    f = g_file_new_for_path(config_filename[type]);
    return g_file_read(f, NULL, NULL);
}

int domino_config_read(gint type)
{
    gchar *path;
    gboolean retval;
    switch (domino_config[type].dir) {
        case DOMINO_PROFILE_DIR:
            path = g_build_filename (g_get_user_config_dir (),
                                          DOMINO_NAME, NULL);
            g_mkdir_with_parents (g_get_user_config_dir (), 0700);
            g_mkdir_with_parents (path, 0700);
            break;
        case DOMINO_ETC_DIR:
            path = DOMINO_CONFIG(DOMINO_CONFIG_RELATIVE_PATH);
            break;
        default:
            path = DOMINO_CONFIG(DOMINO_CONFIG_RELATIVE_PATH);
            break;
    };
    config_filename[type] = g_build_filename (path, domino_config[type].name, NULL);
    config[type] = do_config_new(NULL, domino_config[type].name);
    gchar *filename;
    gsize read, write;
    filename = g_locale_from_utf8(config_filename[type], strlen(config_filename[type]),
                                    &read, &write, NULL);
    retval = do_config_read(config[type], filename, domino_config[type].empty);
    g_free(filename);
    g_free(path);
    return retval;
}
gboolean domino_config_save(gint type, gboolean now)
{
   gboolean retval;
   gchar *filename;
   gsize read, write;
   filename = g_locale_from_utf8(config_filename[type], strlen(config_filename[type]),
                                    &read, &write, NULL);

   if (!type)
      read = 1;
   retval = do_config_write(config[type], filename);
   g_free(filename);
   return retval;
}
gboolean domino_config_save_text(gint type, const char *text)
{
   gboolean retval;
   gchar *filename;
   gsize read, write;
   filename = g_locale_from_utf8(config_filename[type], strlen(config_filename[type]),
                                    &read, &write, NULL);
   retval = do_config_write_text(filename, text);
   g_free(filename);
   return retval;
}
void domino_config_property_to_object(gint type, GObject *widget, const gchar *path, ...)
{
    va_list args;
    gchar *attr, *default_value;
    const gchar *value;
    va_start (args, path);
    attr = va_arg (args, gchar *);
    while (attr != NULL) {
        default_value = va_arg (args, gchar *);
        value = do_config_get_property(config[type], path, attr);
        if (value)
            do_widget_set_property(widget, attr, value);
        else
        if (default_value)
            do_widget_set_property(widget, attr, default_value);
        attr = va_arg (args, gchar *);
    }
    va_end (args);
}
void domino_config_property_set(gint type, const gchar *path, ...)
{
    va_list args;
    gchar *attr, *value;
    va_start (args, path);
    attr = va_arg (args, gchar*);
    if ( !path ) {
        path = g_strdup("Sad");
    }
    while (attr != NULL) {
        value = va_arg (args, gchar *);
        if (value)
            do_config_set_property(config[type], path, attr, value);
        else
            do_config_clear_property(config[type], path, attr);
        attr = va_arg (args, gchar *);
    }
    va_end (args);
}
GList *domino_config_properties(gint type, const gchar *path)
{
    const do_string_list_t *st;
    st = do_config_get_properties(config[type], path);
    if (st) {
        GList *list = NULL;
        gint i;
        for (i = 0; i < st->count; i+=2)
            list = g_list_append(list, g_strdup(st->list[i]));
        return list;
    }
    else
        return NULL;
}

void domino_config_property_get(gint type, const gchar *path, ...)
{
    va_list args;
    gchar *attr;
    void **value;
    va_start (args, path);
    attr = va_arg (args, gchar *);
    while (attr != NULL) {
        value = va_arg (args, void **);
        *value = (void*)do_config_get_property(config[type], path, attr);
        attr = va_arg (args, gchar *);
    }
    va_end (args);
}
void domino_config_property_get_attr(gint type, const gchar *path, ...)
{
    va_list args;
    gchar *attr;
    void **value;
    va_start (args, path);
    attr = va_arg (args, gchar *);
    while (attr != NULL) {
        value = va_arg (args, void **);
        *value = (void*)do_config_get_attr(config[type], path, attr);
        attr = va_arg (args, gchar *);
    }
    va_end (args);
}
void domino_config_kind_set(gint type, const gchar *path, const gchar *kind)
{
    do_config_set_kind(config[type], path, kind);
}

gboolean util_save_to_file(const gchar *uri, gchar *text, gint len)
{
    GFile *f;
    GOutputStream *s;
    GError *error = NULL;
    gint size;
    gboolean retval = TRUE;
    gchar *scheme;

    f = g_file_new_for_uri(uri);
    scheme = g_file_get_uri_scheme(f);
    if ( !scheme ) {
        g_object_unref(f);
        f = g_file_new_for_path(uri);
    }
    else
        g_free(scheme);
    s = G_OUTPUT_STREAM (g_file_create(f, G_FILE_CREATE_NONE, NULL, &error));
    if ( !s ) {
        do_log(LOG_ERR, "Ошибка создания файла \"%s\":%s", uri, error->message);
        g_error_free(error);
        g_object_unref(f);
        return FALSE;
    }
    size = ( len == -1 ) ? strlen(text) : len;
    if ( g_output_stream_write(G_OUTPUT_STREAM(s), text, size, NULL, &error) != size ) {
        do_log(LOG_ERR, "Ошибка записи в файл \"%s\":%s", uri, error->message);
        g_error_free(error);
        retval = FALSE;
    }
    g_object_unref(s);
    g_object_unref(f);

    return retval;

}

gchar *util_create_uniq_filename(const gchar *path, const gchar *basename, const gchar *fileext)
{
	GRand *grand = g_rand_new();
	gchar *filename = NULL;

	gint i, num;
	for (i=0; i<1600; i++)
	{
		if (i < 1200)
			num = i;
		else
			num = g_rand_int_range(grand, 9, 99999999);

		if (i == 0)
		{
			filename = g_strdup_printf("%s/%s.%s", path, basename, fileext);
		}
		else
		{
			filename = g_strdup_printf("%s/%s-%d.%s", path, basename, num, fileext);
		}

		if (!g_file_test(filename, G_FILE_TEST_EXISTS)) break;

		g_free(filename);
	}

	g_rand_free(grand);

	/* You should g_free() the value after usage */
	return filename;
}
GFileInputStream *domino_get_template_stream(const gchar* name)
{
    gchar t_name[1024];
    GFileInputStream *retval = NULL;
    int i, j, k;

    for (i = 0; i < 4; i++) {
        gchar *path;
        switch (i) {
            case 2:
                path = g_build_filename(g_get_user_config_dir(), DOMINO_NAME, NULL);
                break;
            case 0: {
                path = g_get_current_dir();
#ifdef DEBUG_PATH
                char *val;
                val = do_strdup_path_join(NULL, path, "..", NULL);
                g_free(path);
                path = val;
#endif
                break;
            }
            case 1:
                path = do_strdup(DOMINO_PATH);
                break;
            default:
                path = g_get_current_dir();
        }
        for (j = 0; j < 2; j++) {
            gchar *f_name;
            GFile *f;

            if (!j)
                sprintf(t_name, "%s", name);
            else
                sprintf(t_name, "%s.html", name);
            for (k = 0; k < 2; k++) {
                if (!k)
                    f_name = g_build_filename(path, t_name, NULL);
                else
                    f_name = g_build_filename(path, "..", "template", t_name, NULL);
                f = g_file_new_for_path(f_name);
                g_free(f_name);
                retval = g_file_read(f, NULL, NULL);
                g_object_unref(f);

                if (retval) {
                    g_free(path);
                    return retval;
                }
            }
        }
        g_free(path);
    }
    return NULL;
}

gchar *do_share_get_ui_filename(const gchar *name)
{
    gchar *fname;
    gchar *uri = NULL;
    int i, k;
    if ( strcmp(name+strlen(name)-3, ".ui") ) {
        fname = g_strdup_printf("%s.ui", name);
    }
    else
        fname = strdup(name);

    for (i = 0; i < 3 && !uri ; i++) {
        gchar *path;
        switch (i) {
            case 0:
                path = g_get_current_dir();
                break;
            case 1:
                path = g_path_get_dirname(update_info.argv[0]);
                break;
            default:
                path = do_strdup(DOMINO_PATH);
                break;
        }

        for (k = 0; k < 2 && !uri ; k++) {
            gchar *f_name;
            if (!k)
                f_name = g_build_filename(path, "..", "share", "domino", fname, NULL);
            else
                f_name = g_build_filename(path, "..", "..", "share", "domino", fname, NULL);
            if ( g_file_test(f_name, G_FILE_TEST_EXISTS) ) {
                uri = f_name;
                break;
            }
            g_free(f_name);
        }
        g_free(path);
    }
    g_free(fname);

    return uri;

}
gboolean restart_programm(gchar *message)
{

    gboolean retval = TRUE;
//    if ( message )
//        GDK_THREADS_ENTER();
    gboolean restart = TRUE;
    if ( message ) {
        gint res;
        DOMINO_SHOW_QUESTION(res, GTK_BUTTONS_YES_NO, "%s\nПерезапутить программу?", message);
        restart = res == GTK_RESPONSE_YES;
    }
    if ( restart ) {
#ifdef _WIN32
        gchar *cmdline = NULL;
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        char *argv0 = NULL;
        char *arg;
        int i;

        cmdline = g_strdup(update_info.argv[0]);
        for (i = 1, arg = update_info.argv[1]; arg; i++, arg = update_info.argv[i])
            cmdline = g_strdup_printf("%s %s", cmdline, arg);

        memset (&si, 0, sizeof (si));
        si.cb = sizeof (&si);
        memset (&pi, 0, sizeof (pi));


        if (!CreateProcess (argv0, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            DOMINO_SHOW_ERROR("Не удалось запустить программу: %s", g_win32_error_message (GetLastError ()));
            retval = FALSE;
        }

        g_free(cmdline);

        CloseHandle (pi.hThread);
#else
        GError *error = NULL;
        if (!g_spawn_async(NULL, update_info.argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error)) {
            if ( message )
                DOMINO_SHOW_INFO("Не удалось запустить программу, перезапустите вручную. %s", error->message);
            g_error_free(error);
            retval = FALSE;
        }
#endif
        if (retval)
            exit(1);
            //!!gtk_widget_destroy (update_info.win);
    }
    if (message) {
        g_free(message);
//        GDK_THREADS_LEAVE();
    }
    return FALSE;
}
void update_action_set_arg(char **argv, gint argc)
{
    int i;
    update_info.argv = (char**)g_malloc((argc + 1)*sizeof(char*));
    for (i = 0; i < argc; i++)
        update_info.argv[i] = g_strdup(argv[i]);
    update_info.argv[argc] = NULL;
}

#if defined(BINARY_UPDATE)
static gint version_cmp(const gchar *s1, const gchar *s2)
{
    gint i1[3], i2[3];
    gchar *ch;
    gint res;
    memset(i1, 0, sizeof(i1));
    ch = (char*)s1;
    i1[0] = strtol(ch, &ch, 10);
    if ( ch[0] == '.' ) ch++;
    i1[1] = strtol(ch, &ch, 10);
    if ( ch[0] == '.' ) ch++;
    i1[2] = strtol(ch, &ch, 10);
    if ( ch[0] == '.' ) ch++;
    memset(i2, 0, sizeof(i1));
    ch = (char*)s2;
    i2[0] = strtol(ch, &ch, 10);
    if ( ch[0] == '.' ) ch++;
    i2[1] = strtol(ch, &ch, 10);
    if ( ch[0] == '.' ) ch++;
    i2[2] = strtol(ch, &ch, 10);
    if ( ch[0] == '.' ) ch++;

    res = i1[0] - i2[0];
    if ( res )
        return res;
    res = i1[1] - i2[1];
    if ( res )
        return res;
    res = i1[2] - i2[2];
    return res;
}
gboolean do_domino_update_utils(UpdateInfo *update_info)
{
    gchar *repository;
    gchar *config_name;
    gchar *temp_name;
    gchar *file_name;
    GFile *src;
    GFile *dst;
    GError *error = NULL;
    do_config_t *config = NULL;
    const gchar *version;
    gboolean retval = TRUE;
    gboolean mkdir;
    int indx, j;
    do_string_list_t *objects = NULL;
    gchar *value;
    gchar *path_prog;
    gchar *path_root;

    DOMINO_COMMON_GET("Update", "repository", &repository, "config", &config_name, NULL);
    //config_name = do_strdup("alias.xml");
    //config_name = do_strdup("alias.xml");

    temp_name = g_strdup_printf("%s/%s", g_get_tmp_dir(), config_name);
    file_name = g_strdup_printf("%s/%s", repository, config_name);

    src = g_file_new_for_uri(file_name);
    dst = g_file_new_for_path(temp_name);
    if (!g_file_copy(src,
                dst,
                G_FILE_COPY_OVERWRITE,
                NULL,
                NULL,
                NULL,
                &error)) {
                if ( !update_info->in_thread ) {
                    do_log(LOG_ERR, "Get update file %s : %s", file_name, error->message);
                }
                else {
                    do_log(LOG_INFO, "Get update file %s : %s", file_name, error->message);
                }
                g_error_free (error);
                retval = FALSE;
                goto do_domino_update_end;
    }

    config = do_config_new(NULL, "");
    if (!do_config_read(config, temp_name, FALSE)) {
        retval = FALSE;
        goto do_domino_update_end;
    }

    version = do_config_get_property(config, "Update", "version");

    if ( version_cmp(version, VERSION) <= 0 ) {
	if ( !update_info->in_thread )  {
	    DOMINO_SHOW_INFO("Обновлений не требуется");
        }
        goto do_domino_update_end;
    }
#ifdef VISTA_UPDATE
    OSVERSIONINFO osver = { sizeof(osver) };
    if ( !GetVersionEx(&osver) ) {
        do_log(LOG_ERR, "%s", strerror(GetLastError()));
        return FALSE;
    }
    if ( osver.dwMajorVersion >= 6 ) {
        if ( !IsRunAsAdmin() ) {
            gint res;
            DOMINO_SHOW_QUESTION(res, GTK_BUTTONS_YES_NO, "Программа должна быть обновленна\nПерезапутить программу?");
            do_log(LOG_ERR, "%d", res);
            if ( res == GTK_RESPONSE_YES ) {
                char szPath[MAX_PATH];
                if (GetModuleFileName(NULL, szPath, sizeof(szPath) - 1 )) {
                    // Launch itself as administrator.
                    SHELLEXECUTEINFO sei;
                    memset(&sei, 0, sizeof(sei));
                    sei.cbSize   = sizeof(sei);
                    sei.lpVerb = (LPCSTR)"runas";
                    sei.lpFile = (LPCSTR)szPath;
                    sei.nShow = SW_NORMAL;

                    if (!ShellExecuteEx(&sei))
                    {
                        do_log(LOG_ERR, "%s", strerror(GetLastError()));
                         goto do_domino_update_end;
                    }
                    else
                        exit(0);
                }
            }
            else
                 goto do_domino_update_end;
        }
    }
#endif
    const gchar *message;
    message = do_config_get_property(config, "Update", "message");

    value = g_find_program_in_path(g_get_prgname());
    path_prog = g_path_get_dirname(value);
    path_root = g_strdup_printf("%s/..", path_prog);
    g_free(path_prog);
    g_free(value);

    objects = do_config_get_children(config, "Update/Files");
    for ( indx = 0; indx < objects->count; indx++ ) {
        const gchar *object_name;
        do_string_list_t *items;
        const gchar *crnt_path;
        object_name = objects->list[indx];
        items = do_config_get_children(config, object_name);
        mkdir = do_config_get_attr_bool(config, object_name, "mkdir");
        crnt_path = do_config_get_attr(config, object_name, "path");
        for ( j = 0; j < items->count && retval; j++ ) {
            gchar *src_file_name;
            gchar *dst_file_name;
            const gchar *value_key;
            const gchar *value_file;
            gboolean update;
            GFile *src;
            GFile *dst;

            value_key = items->list[j];
            value_file = do_config_get_attr(config, value_key, "file");
            update = do_config_get_attr_bool(config, value_key, "update");

            src_file_name = g_strconcat(repository, "/", crnt_path, "/", value_file, NULL);
            dst_file_name = g_strconcat(path_root, "/", crnt_path, "/", value_file, NULL);

            src = g_file_new_for_uri(src_file_name);
            dst = g_file_new_for_path(dst_file_name);
            if ( update || !g_file_query_exists(dst, NULL) ) {
                GFile *dst_temp;
                gchar *dst_temp_name;
                dst_temp_name = g_strconcat(dst_file_name, "~", NULL);
                dst_temp = g_file_new_for_path(dst_temp_name);
                if ( !g_file_query_exists(dst, NULL) ) {
                    if ( mkdir ) {
                        GFile *parent;
                        parent = g_file_get_parent(dst);
                        gchar *value;
                        value = g_file_get_path (parent);
                        if ( g_mkdir_with_parents (value, 0700) == -1) {
                            do_log(LOG_ERR, "Create dir %s : %s", value, strerror(errno));
                            retval = FALSE;
                        }
                        g_free(value);
                        g_object_unref(parent);
                    }
                }
                else {
                     if ( !g_file_move(dst,
                            dst_temp,
                            G_FILE_COPY_OVERWRITE,
                            NULL,
                            NULL,
                            NULL,
                            &error) ) {
                        do_log(LOG_ERR, "Move file %s to %s : %s", dst_file_name, dst_temp_name, error->message);
                        g_error_free (error);
                        retval = FALSE;
                    }
                }
                if ( retval && !g_file_copy(src,
                            dst,
                            G_FILE_COPY_OVERWRITE | G_FILE_COPY_BACKUP,
                            NULL,
                            NULL,
                            NULL,
                            &error) ) {
                    do_log(LOG_ERR, "Copy file %s to %s : %s", src_file_name, dst_file_name, error->message);
                    g_error_free (error);
                    retval = FALSE;
                    g_file_move(dst_temp,
                            dst,
                            G_FILE_COPY_OVERWRITE,
                            NULL,
                            NULL,
                            NULL,
                            NULL);
                }
                g_free(dst_temp_name);
                g_object_unref(dst_temp);
            }
            g_free(dst_file_name);
            g_free(src_file_name);
            g_object_unref(src);
            g_object_unref(dst);
        }
        if (!retval)
            goto do_domino_update_end;
    }
    if (retval)
        g_timeout_add(100,  (GSourceFunc)restart_programm, g_strdup(message));

do_domino_update_end:

    if ( objects )
        do_string_list_free(objects);
    g_free(file_name);
    g_free(temp_name);
    if (config)
        do_config_free(config);
    return retval;

}


void update_action_run(GtkWidget *main_win, gboolean in_thread)
{
    if ( main_win )
        update_info.win = main_win;
    update_info.in_thread = in_thread;
    if ( in_thread ) {
#if GLIB_MINOR_VERSION < 32
		do_domino_update_utils(&update_info);
#else
        g_thread_new("update-thread", (GThreadFunc)do_domino_update_utils, &update_info);
#endif
    }
    else
        do_domino_update_utils(&update_info);
}
#endif

static int do_property_cmp(void *s1, void *s2, void *user_data)
{
    return ((DoProperyItem*)s1)->code - ((DoProperyItem*)s2)->code;
}
void do_property_list_free(do_sort_list_t *list)
{
    int i;
    for (i = 0; i < list->count; i++)
        g_free(((DoProperyItem*)list->list[i])->name);
    do_sort_list_free(list);
}

gchar *do_property_list_get_name(do_sort_list_t *list, int code)
{
    DoProperyItem item;
    int indx;

    item.code = code;
    if (do_sort_list_find(list, &indx, &item))
        return ((DoProperyItem*)list->list[indx])->name;
    if (list->count)
        return ((DoProperyItem*)list->list[0])->name;
    return "";
}
do_sort_list_t *do_property_list_create_with_store(const gchar *section, GtkListStore *stores)
{
    GList *list, *l;
    do_sort_list_t *retval;
    DoProperyItem item;
    GtkTreeIter iter;

    retval = do_sort_list_new(1, 0, do_property_cmp, NULL);

    list = DOMINO_COMMON_PROPERTIES(section);
    for (l = list; l; l = l->next) {
        gchar *value;
        DOMINO_COMMON_GET(section, l->data, &value, NULL);
        gtk_list_store_append (stores, &iter);
        gtk_list_store_set (stores, &iter,
                            0, atoi(value),
                            1, l->data,
                            -1);

        item.code = atoi(value);
        item.name = g_strdup(l->data);
        do_sort_list_add_alloc(retval, &item, sizeof(item));
    }
    g_list_foreach(list, (GFunc)g_free, NULL);
    g_list_free(list);
    return retval;
}
/*
void do_property_list_create_store(const gchar *section, GtkListStore *stores)
{
    GList *list, *l;
    GtkTreeIter iter;

    list = DOMINO_COMMON_PROPERTIES(section);
    for (l = list; l; l = l->next) {
        gchar *value;
        DOMINO_COMMON_GET(section, l->data, &value, NULL);
        gtk_list_store_append (stores, &iter);
        if ( gtk_tree_model_get_column_type(GTK_TREE_MODEL(stores), 0) == G_TYPE_INT )
            gtk_list_store_set (stores, &iter,
                                0, atoi(value),
                                1, l->data,
                                -1);
        else
            gtk_list_store_set (stores, &iter,
                                0, value,
                                1, l->data,
                                -1);
    }
    g_list_foreach(list, (GFunc)g_free, NULL);
    g_list_free(list);
}*/
GtkListStore *do_property_enum_create_store(const gchar *enum_name, gchar **default_value)
{
    GList *list, *l;
    GtkTreeIter iter;
    GtkListStore *store;

    store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    *default_value = NULL;
    DOMINO_COMMON_GET(enum_name, "default", default_value, NULL);

    list = DOMINO_COMMON_PROPERTIES(enum_name);
    for (l = list; l; l = l->next) {
        gchar *value;
       DOMINO_COMMON_GET(enum_name, l->data, &value, NULL);
        gtk_list_store_append (store, &iter);
        gtk_list_store_set (store, &iter,
                                0, value,
                                1, l->data,
                                -1);
    }
    g_list_foreach(list, (GFunc)g_free, NULL);
    g_list_free(list);
    return store;
}

do_sort_list_t *do_property_list_create(const gchar *section)
{
    GList *list, *l;
    do_sort_list_t *retval;
    DoProperyItem item;

    retval = do_sort_list_new(1, 0, do_property_cmp, NULL);

    list = DOMINO_COMMON_PROPERTIES(section);
    for (l = list; l; l = l->next) {
        gchar *value;
        DOMINO_COMMON_GET(section, l->data, &value, NULL);
        item.code = atoi(value);
        item.name = g_strdup(l->data);
        do_sort_list_add_alloc(retval, &item, sizeof(item));
    }
    g_list_foreach(list, (GFunc)g_free, NULL);
    g_list_free(list);
    return retval;
}

GtkBuilder *do_builder_new(const gchar *name)
{
    gchar *filename;
    GtkBuilder *ret = NULL;

    filename = do_share_get_ui_filename(name);
    if ( !filename ) {
        do_log(LOG_ERR, "Описание интерфейса \"%s\" не найдено",name);
        return NULL;
    }
    ret = gtk_builder_new();
    GError *error = NULL;
    if ( !gtk_builder_add_from_file(ret, filename, &error) ) {
        do_log(LOG_ERR, "Ошибка загрузки файла \"%s\":%s", filename, error->message);
        g_error_free(error);
        g_object_unref(ret);
        ret = NULL;
    }
    g_free(filename);
    return ret;
}
GtkWidget *do_builder_get_widget(GtkBuilder *builder, const gchar *name)
{
    GObject *w;
    w = gtk_builder_get_object(builder, name);
    if ( !w || !GTK_IS_WIDGET(w) ) {
        do_log(LOG_ERR, "Обьект интерфейса с именем \"%s\" не найден", name);
        return NULL;
    }
    return GTK_WIDGET(w);
}
#ifdef USE_IM


static gchar     *xmpp_server = "jabber.glekar.ru";
static gint       xmpp_port = 5222;
static gchar     *xmpp_username = NULL;
static gchar     *xmpp_password = "123";
static gchar     *xmpp_resource = "domino";
LmConnection     *xmpp_connection = NULL;
static GList     *xmpp_roster = NULL;

static int xmpp_connect();
void xmpp_send_message(const gchar *recipients, const gchar *message);

void
domino_im_init()
{
    gchar *user, *domain, *password;

    DOMINO_LOCAL_GET("Jabber", "user", &user, NULL);
    DOMINO_LOCAL_GET("Jabber", "domain", &domain, NULL);
    DOMINO_LOCAL_GET("Jabber", "password", &password, NULL);

    if ( domain ) xmpp_server = g_strdup(domain);
    if ( password ) xmpp_password = g_strdup(password);
    if ( user ) {
        if ( strchr(user, '@') )
            xmpp_username = g_strdup(user);
        else
            xmpp_username = g_strdup_printf("%s@%s", user, xmpp_server);
    }
    if ( xmpp_username )
        xmpp_connect();
}
static void  xmpp_roster_request(LmConnection *conn);
static void  xmpp_roster_parse_query(LmMessageNode *node);
const gchar* xmpp_roster_find_by_jid(const gchar *jid);

static gchar *
get_part_name (const gchar *username)
{
	const gchar *ch;

	g_return_val_if_fail (username != NULL, NULL);

	ch = strchr (username, '@');
	if (!ch) {
		return NULL;
	}

	return g_strndup (username, ch - username);
}

static void
connection_auth_cb (LmConnection *connection,
		    gboolean      success,
		    gpointer      user_data)
{
	if ( success ) {
		LmMessage *m;

		g_print ("TestLM: Authenticated successfully\n");
		m = lm_message_new_with_sub_type (NULL,
						  LM_MESSAGE_TYPE_PRESENCE,
						  LM_MESSAGE_SUB_TYPE_AVAILABLE);
		lm_connection_send (connection, m, NULL);
		g_print ("TestLM: Sent presence message:'%s'\n",
			 lm_message_node_to_string (m->node));
		lm_message_unref (m);

        xmpp_roster_request(connection);

	} else {
		g_printerr ("TestLM: Failed to authenticate\n");
	}
}

static void
connection_open_cb (LmConnection *connection,
		    gboolean      success,
		    gpointer      user_data)
{
	if ( success ) {
		gchar *user;

		user = get_part_name (xmpp_username);
		lm_connection_authenticate (connection, user,
					    xmpp_password, xmpp_resource,
					    connection_auth_cb,
					    NULL, FALSE,  NULL);
		g_free (user);

		g_print ("TestLM: Sent authentication message\n");
	} else {
		g_printerr ("TestLM: Failed to connect\n");
	}
}
static gboolean
xmpp_reconnect (gpointer user_data)
{
    if ( xmpp_connect() )
        return FALSE;
    return TRUE;
}
static void
connection_close_cb (LmConnection       *connection,
		     LmDisconnectReason  reason,
		     gpointer            user_data)
{
	const char *str;

	switch (reason) {
	case LM_DISCONNECT_REASON_OK:
		str = "LM_DISCONNECT_REASON_OK";
		break;
	case LM_DISCONNECT_REASON_PING_TIME_OUT:
		str = "LM_DISCONNECT_REASON_PING_TIME_OUT";
		break;
	case LM_DISCONNECT_REASON_HUP:
		str = "LM_DISCONNECT_REASON_HUP";
        break;
	case LM_DISCONNECT_REASON_ERROR:
		str = "LM_DISCONNECT_REASON_ERROR";
		break;
	case LM_DISCONNECT_REASON_UNKNOWN:
	default:
		str = "LM_DISCONNECT_REASON_UNKNOWN";
		break;
	}
	g_print ("TestLM: Disconnected, reason:%d->'%s'\n", reason, str);
	g_timeout_add(10000, xmpp_reconnect, NULL);
}
#define XMPP_VERSION_XMLNS         "jabber:iq:version"
#define XMPP_ROSTER_XMLNS          "jabber:iq:roster"
#define XMPP_REGISTER_XMLNS        "jabber:iq:register"
#define XMPP_NS_PING               "urn:xmpp:ping"

static LmHandlerResult
handle_iq (LmMessageHandler *handler,
           LmConnection     *connection,
           LmMessage        *m,
           gpointer          user_data)
{
	LmMessageNode    *node;
	const gchar      *xmlns;
	gint              subtype;

	subtype = lm_message_get_sub_type (m);

    g_print ("TestLM: Incoming message %d\n", subtype);

    switch ( subtype ) {
        case LM_MESSAGE_SUB_TYPE_RESULT:
            node = lm_message_node_get_child(m->node, "query");
            if( node ) {
                xmlns = lm_message_node_get_attribute (node, "xmlns");
                if( g_strcmp0(xmlns, XMPP_ROSTER_XMLNS ) == 0) {
                    xmpp_roster_parse_query(node);
                }
            }
            break;
        case LM_MESSAGE_SUB_TYPE_GET:
            node = lm_message_node_get_child (m->node, "ping");
            if ( node ) {

                xmlns = lm_message_node_get_attribute (node, "xmlns");

                if ( g_strcmp0(xmlns, XMPP_NS_PING) == 0) {
                    const char *recipient, *id;
                    LmMessage  *ping;

                    recipient = lm_message_node_get_attribute(m->node, "from");
                    id = lm_message_node_get_attribute(m->node, "id");

                    ping = lm_message_new_with_sub_type (recipient,
                                                     LM_MESSAGE_TYPE_IQ,
                                                     LM_MESSAGE_SUB_TYPE_RESULT);
                    lm_message_node_set_attribute(ping->node, "id", id);
                    lm_connection_send (connection, ping, NULL);
                    lm_message_unref (ping);

                    return LM_HANDLER_RESULT_ALLOW_MORE_HANDLERS;
                }
            }
            break;
        default:
            break;
    }
	return LM_HANDLER_RESULT_REMOVE_MESSAGE;
}
static LmHandlerResult
handle_messages (LmMessageHandler *handler,
		 LmConnection     *connection,
		 LmMessage        *m,
		 gpointer          user_data)
{
	const gchar *from;//, *to;
	const gchar *name;
	const gchar *body = NULL;
	LmMessageNode *node;
	gchar *jid, *ch;

	if ( lm_message_get_sub_type (m) != LM_MESSAGE_SUB_TYPE_CHAT )
        return LM_HANDLER_RESULT_REMOVE_MESSAGE;

	node = lm_message_node_get_child(m->node, "body");
	if ( node && (body = lm_message_node_get_value(node)) ) {
        from = lm_message_node_get_attribute (m->node, "from");
        //to = lm_message_node_get_attribute (m->node, "to");
        jid = g_strdup(from);
        ch = strchr(from, '/');
        if ( ch )
            jid = g_strndup(from, ch-from);
        else
            jid = g_strdup(from);
        name = xmpp_roster_find_by_jid(jid) ? xmpp_roster_find_by_jid(jid) : from;
        g_print ("TestLM: Incoming message from: %s\n%s\n",name,body);
        xmpp_send_message(jid, "Пшёлты");
    }
	return LM_HANDLER_RESULT_REMOVE_MESSAGE;
}

int
xmpp_connect()
{
    LmMessageHandler *handler;
	gboolean          result;
	GError           *error = NULL;

    if ( xmpp_connection ) {
        if ( lm_connection_is_open(xmpp_connection) )
            return TRUE;
        else {
            lm_connection_unref(xmpp_connection);
            xmpp_connection = NULL;
        }
    }
    if ( xmpp_connection == NULL ) {
        if ( !xmpp_username ) {
            g_printerr("Пользователь обмена сообщениями неопределен\n");
            return FALSE;
        }
        xmpp_connection = lm_connection_new (xmpp_server);
        lm_connection_set_port (xmpp_connection, xmpp_port);
        lm_connection_set_jid (xmpp_connection, xmpp_username);

        handler = lm_message_handler_new (handle_messages, NULL, NULL);
        lm_connection_register_message_handler (xmpp_connection, handler,
						LM_MESSAGE_TYPE_MESSAGE,
						LM_HANDLER_PRIORITY_NORMAL);
        lm_message_handler_unref (handler);

        handler = lm_message_handler_new (handle_iq, NULL, NULL);
        lm_connection_register_message_handler (xmpp_connection, handler,
						LM_MESSAGE_TYPE_IQ,
						LM_HANDLER_PRIORITY_NORMAL);
        lm_message_handler_unref (handler);

        lm_connection_set_disconnect_function (xmpp_connection,
					       connection_close_cb,
					       NULL, NULL);
    }
	result = lm_connection_open (xmpp_connection,
				     (LmResultFunction) connection_open_cb,
				     NULL, NULL, &error);

	if (!result) {
		g_printerr ("TestLM: Opening connection failed, error:%d->'%s'\n",
			 error->code, error->message);
		g_free (error);
		return 0;
	}
	return 1;
}
void
xmpp_roster_request(LmConnection *conn)
{
	LmMessage *req;
	LmMessageNode *query;
	GError *err = NULL;
	req = lm_message_new_with_sub_type(NULL, LM_MESSAGE_TYPE_IQ,
	                                   LM_MESSAGE_SUB_TYPE_GET);
	query = lm_message_node_add_child(req->node, "query", NULL);
	lm_message_node_set_attributes(query, "xmlns", "jabber:iq:roster", NULL);
	if(!lm_connection_send(conn, req, &err)) {
		g_printerr("Error sending roster request: %s\n", err->message);
		g_error_free(err);
	}
	lm_message_unref(req);
}
const gchar*
xmpp_roster_find_by_jid(const gchar *jid)
{
	GList *elem;
	gchar **item;
	for (elem = xmpp_roster;  elem; elem = elem->next) {
		item = (gchar **)elem->data;
		if ( g_strcmp0(item[0], jid) == 0)
            return item[1];
	}
	return NULL;
}
void
xmpp_roster_parse_query(LmMessageNode *query)
{
	LmMessageNode *item;
	gchar **rec;
	for(item = lm_message_node_get_child(query, "item"); item;  item = item->next) {
		const gchar *attr;
		//LmMessageNode *node;
		attr = lm_message_node_get_attribute(item, "subscription");
		if ( g_strcmp0(attr, "remove") == 0 ) {
			g_printerr("Removing somebody from roster\n");
			continue;
		}
		attr = lm_message_node_get_attribute(item, "jid");
		if ( xmpp_roster_find_by_jid(attr) )
			continue;
        rec = g_malloc(sizeof(char*)*2);
		rec[0] = g_strdup(attr ? attr : "");
		attr = lm_message_node_get_attribute(item, "name");
		if ( attr )
			rec[1] = g_strdup(attr);
        else
			rec[1] = g_strdup(rec[0]);
        xmpp_roster = g_list_prepend(xmpp_roster, rec);
	}
}
void
xmpp_send_message(const gchar *recipients, const gchar *message)
{
    GError       *error = NULL;
    LmMessage    *m;
    if ( !xmpp_connect() )
        return;

    gchar **val, **r;
    val = g_strsplit(recipients,"-",-1);

    for ( r = val; *r ; r++) {
        m = lm_message_new (*r, LM_MESSAGE_TYPE_MESSAGE);
        lm_message_node_add_child (m->node, "body", message);
        if ( !lm_connection_send (xmpp_connection, m, &error) ) {
            g_printerr("Error while sending message to '%s': %s\n",
                         *r, error ? error->message:"");
            lm_message_unref (m);
            break;
        }
        lm_message_unref (m);
    }
    g_strfreev(val) ;
}
#endif
