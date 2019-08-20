
#include "do_input_csv.h"
#include <string.h>
#include <stdlib.h>

static GObjectClass *parent_class;

struct _DoInputCsv {

	GObject        parent;
    GInputStream  *stream;

	gchar         *line;
	gchar         *crnt_line_pos;
	gchar         *buff;
	gchar         *remaining;
	unsigned       buff_len;
	unsigned       remaining_len;
	gint           line_indx;
	/* int		 current_line; */
};

typedef struct {
	GObjectClass input_class;
} DoInputCsvClass;

#define BUFF_LEN 65000

G_DEFINE_TYPE (DoInputCsv, do_input_csv, G_TYPE_OBJECT)

static gboolean do_input_csv_read(DoInputCsv *csv, GError **error)
{
    gssize size;
    gboolean r = FALSE;

    if (!csv->buff_len) {
        csv->buff = g_malloc(csv->buff_len = BUFF_LEN);
        csv->remaining = csv->buff;
    }
    else {
        r = csv->buff[csv->buff_len - 1] == '\r';
        if (csv->remaining) {
            if (csv->buff == csv->remaining)
                csv->buff = g_realloc(csv->buff, csv->buff_len *= 2);
            else
                memmove(csv->buff, csv->remaining, csv->buff_len - (csv->remaining - csv->buff));
        }
        csv->remaining = csv->buff;

    }

    size = g_input_stream_read(csv->stream, csv->remaining + csv->remaining_len,
                                 csv->buff_len - (csv->remaining - csv->buff) - csv->remaining_len,
                                 NULL, error);
    if (size == -1)
        return FALSE;
    if (r && (csv->remaining[csv->remaining_len] == '\n'))
        csv->remaining_len++;
    if (size == 0)
        csv->remaining[csv->remaining_len + size] = '\0';
    if (size < csv->buff_len - (csv->remaining - csv->buff) - csv->remaining_len)
        csv->remaining[csv->remaining_len + size] = '\0';
    return TRUE;
}

DoInputCsv *do_input_csv_new_from_uri(const char *uri, GError **error)
{
    GFile *file;
	DoInputCsv *input;

	input = g_object_new (DO_INPUT_CSV_TYPE, NULL);
	if (G_UNLIKELY (NULL == input)) return NULL;

    file = g_file_new_for_uri(uri);
    input->stream = G_INPUT_STREAM(g_file_read (file, NULL, error));
	return input;
}

static void do_input_csv_finalize (GObject *obj)
{
	DoInputCsv *input = (DoInputCsv *)obj;

	if (input->stream != NULL) {
		g_object_unref (G_OBJECT (input->stream));
		input->stream = NULL;
	}
	if (input->buff)
        g_free (input->buff);
	if (input->line)
        g_free (input->line);
	input->buff  = NULL;
	input->line  = NULL;
	input->buff_len = 0;
	input->line_indx = 0;
	parent_class->finalize (obj);
}


static void do_input_csv_init (DoInputCsv *obj)
{
	DoInputCsv *csv = DO_INPUT_CSV (obj);

	csv->stream	 = NULL;
	csv->line	 = NULL;
	csv->buff	 = NULL;
	csv->remaining = NULL;
	csv->buff_len = 0;
	csv->remaining_len = 0;
}

static void do_input_csv_class_init (DoInputCsvClass *gobject_class)
{
	GObjectClass *input_class =   G_OBJECT_CLASS (gobject_class);

	input_class->finalize = do_input_csv_finalize;

	parent_class = g_type_class_peek_parent (gobject_class);
}
static void do_input_csv_del_space(DoInputCsv *csv)
{
    gunichar out;
    char *p;
    for (p = (gchar*)csv->crnt_line_pos;
          p && *p !='\0'; p = (gchar*)g_utf8_next_char(p)) {
        out = g_utf8_get_char(p);
        if (!g_unichar_isspace(out))
            break;
    }
    if (p)
        csv->crnt_line_pos = p;
}
static gchar *do_input_csv_del_left_space(char *value)
{
    gunichar out;
    char *p;
    for (p = value;
          p && *p !='\0'; p = (gchar*)g_utf8_next_char(p)) {
        out = g_utf8_get_char(p);
        if (!g_unichar_isspace(out))
            break;
    }
    return p;
}
static void do_input_csv_del_right_space(char *value)
{
    gunichar out;
    char *p;
    char *tail = NULL;
    for (p = value;
          p && *p !='\0'; p = (gchar*)g_utf8_next_char(p)) {
        out = g_utf8_get_char(p);
        if (g_unichar_isspace(out) && !tail)
            tail = p;
        else
            tail = NULL;
    }
    if (tail)
        *tail = '\0';
}
gboolean do_input_csv_read_line(DoInputCsv *csv, GError **error)
{
    gchar *ptr;
    gchar *end;
    gboolean comment;

    if (csv->line) {
        g_free(csv->line);
        csv->line = NULL;
    }

    while (1) {
        if (!csv->remaining ||
             csv->remaining + csv->remaining_len - csv->buff >= csv->buff_len )
             if (!do_input_csv_read(csv, error))
                return FALSE;

        if (*csv->buff == '\0') // eof
            return FALSE;

        ptr = csv->remaining + csv->remaining_len;
        end = csv->buff + csv->buff_len;
        if (ptr[0] == '\0')
            return FALSE;

		for (; ptr < end ; ptr++)
			if (*ptr == '\n' || *ptr == '\r' || *ptr == '\0')
				break;
        if (ptr < end) {

            csv->line_indx++;

            gsize bytes_read;
            gsize bytes_written;

            csv->line = g_locale_to_utf8(csv->remaining,
                                            ptr - csv->remaining,
                                            &bytes_read, &bytes_written,
                                            error);
            if (error && *error != NULL)
                return FALSE;
            if (!csv->line)
                return FALSE;

            csv->crnt_line_pos = csv->line;
            do_input_csv_del_space(csv);
            comment = csv->crnt_line_pos[0] == '#' ||
                      csv->crnt_line_pos[0] == ';' ||
                      csv->crnt_line_pos[0] == '\0';

            if (comment) {
                g_free(csv->line);
                csv->line = NULL;
            }

            if (ptr == '\0') //eof
                csv->remaining = NULL;
            else {
                ptr++;
                if (ptr < end) {
                    if (*ptr == '\n' || *ptr == '\r')
                       ptr++;
                    csv->remaining = ptr;
                }
                else {
                    csv->remaining = NULL;
                }
                csv->remaining_len = 0;
            }
            if (!comment)
                break;
        }
        else
            csv->remaining_len = ptr - csv->remaining;
    }
    return TRUE;
}
static gchar *do_input_csv_get_elemen(DoInputCsv *csv)
{
    do_input_csv_del_space(csv);
    char *h = csv->crnt_line_pos;
    gchar *retval = NULL;
    //int len;
    if (h[0] == '"')
    {
        h++;
        while (h[0] != '\0' && (h[0] != '"' || h[1] == '"'))
            if (h[0] == '"') h+=2;
            else h++;

        retval = g_strndup(csv->crnt_line_pos + 1, h - csv->crnt_line_pos - 1);
        //len = strlen(retval);
        while (h[0] != '\0' && h[0] != ',') h++;
        if (h[0] == ',') h++;
        csv->crnt_line_pos = h;
    }
    else
    {
        while (h[0] != '\0' && h[0] != ',') h++;
        retval = g_strndup(csv->crnt_line_pos, h - csv->crnt_line_pos);
        do_input_csv_del_right_space(retval);
        if (h[0] == ',') h++;
        csv->crnt_line_pos = h;
    }

    if (!retval)
        return g_strdup("");
    else
        return retval;
}
inline gchar *do_input_csv_get_string(DoInputCsv *csv)
{
    return do_input_csv_get_elemen(csv);
}
gint do_input_csv_get_int(DoInputCsv *csv)
{
    gchar *val;
    gint retval;
    val = do_input_csv_get_elemen(csv);
    retval = atoi(val);
    g_free(val);
    return retval;
}
/*static double atof_(const char *str)
{
        g_
    char decimal_point = localeconv()->decimal_point[0];
    if (decimal_point !=  '.')
    {
        char* str1 = astrdup(str);
        char* cp = str1;
        for (; *cp; cp++)
            if (*cp == '.')
            {
                *cp = decimal_point;
                break;
            };
        double result = atof(str1);
        afree(str1);
        return result;
    }
    else
        return atof(str);
}
*/
gdouble do_input_csv_get_double(DoInputCsv *csv, gchar **end_value)
{
    gchar *val;
    gdouble retval;
    gchar *end;
    val = do_input_csv_get_elemen(csv);
    retval = g_ascii_strtod(val, &end);
    if (end[0] == ',') {
        end[0] = '.';
        retval = g_ascii_strtod(val, &end);
    }
    if (end_value) {
        end = do_input_csv_del_left_space(end);
        if (end && *end != '\0')
            *end_value = g_strdup(end);
        else
            *end_value = NULL;
    }
    g_free(val);
    return retval;
}
gboolean do_input_csv_is_name(DoInputCsv *csv, const gchar *name)
{
    do_input_csv_del_space(csv);
    char *h = csv->crnt_line_pos;
    if (!strncmp(h, name, strlen(name))) {
        csv->crnt_line_pos +=strlen(name);
        do_input_csv_del_space(csv);
        if (csv->crnt_line_pos[0] == '=') {
            csv->crnt_line_pos++;
            return TRUE;
        }
        csv->crnt_line_pos = h;
    }
    return FALSE;
}
inline gint do_input_csv_crnt_line(DoInputCsv *csv)
{
    return csv->line_indx;
}

