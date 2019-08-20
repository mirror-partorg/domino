#include <gio/gio.h>
int main()
{
        const gchar *uri = "https://stackoverflow.com/questions/5758770/";
        GFile *in;
        GFile *out;
        GError *error = NULL;
        gboolean ret;
        g_type_init();
        in = g_file_new_for_uri(uri);
        out = g_file_new_for_path("/tmp/a");
        ret = g_file_copy(in, out, G_FILE_COPY_OVERWRITE,
                          NULL, NULL, NULL, &error);
        if (!ret)
                g_message("%s", error->message);
        return 0;
}
