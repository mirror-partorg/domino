#include "template.h"
#include <dolib.h>
#include <errno.h>
#include <stdarg.h>
#include "../misc/iconv_.h"
#include "../config/config.h"

typedef enum {
    DO_TEMPLATE_TYPE_TEXT,
    DO_TEMPLATE_TYPE_ELEMENT,
    DO_TEMPLATE_TYPE_SECTION,
    DO_TEMPLATE_TYPE_UNKNOW

} do_template_type_t;

typedef struct {
    do_template_type_t type;
    union {
        char *text;
        struct {
            char *name;
            union {
                char *default_value;
                do_list_t *section;
            };
        } element;
    };
} do_template_item_t;

struct _do_template_cursor_t {
    do_template_t *owner;
    do_list_t *items;
    int indx;
};

typedef struct {
    char *name;
    char *text;
} do_template_value_t;

struct _do_template_t {
    do_list_t *items;
    do_list_t *values;
    do_list_t *cursors;
    do_string_list_t *text;
    int use_utf;
    int default_value;
    iconv_t conv;
};

#define PREF_ELEM "<!-"
#define POST_ELEM "-->"
#define SECTION_END "End"

static void do_template_items_free(do_list_t *list);

static do_list_t *do_template_read_section(char *buf)
{
    do_list_t *retval;
    char *head, *ch;
    do_template_item_t item;

    retval = do_list_new(FALSE);
    ch = buf;
    head = buf;
    while ( ch[0] != '\0' ) {
        if ( !strncmp(ch, PREF_ELEM, strlen(PREF_ELEM)) &&
             ch[strlen(PREF_ELEM)] != '-' ) {
            if ( head != ch ) {
                *ch = '\0';
                item.type = DO_TEMPLATE_TYPE_TEXT;
                item.text = do_strdup(head);
                do_list_add_alloc(retval, &item, sizeof(item));
            }
            ch += 3;
            head = ch;
            while ( ch[0] != '\0' ) {
                if ( !strncmp(ch, POST_ELEM, strlen(POST_ELEM)) ) {
                    *ch = '\0';
                    ch += 3;
                    if ( head[0] == '#' ) {
                        item.type = DO_TEMPLATE_TYPE_SECTION;
                        item.element.name = do_strdup(head + 1);
                        item.element.section = NULL;
                        head = ch;
                        while ( ch[0] != '\0' ) {
                            if ( (!strncmp(ch, PREF_ELEM, strlen(PREF_ELEM)) &&
                                   ch[strlen(PREF_ELEM)] != '-') &&
                                  ch[strlen(PREF_ELEM)] == '#' &&
                                  !strncmp(ch + strlen(PREF_ELEM) + 1, item.element.name, strlen(item.element.name)) &&
                                  !strncmp(ch + strlen(PREF_ELEM) + 1 + strlen(item.element.name), SECTION_END, strlen(SECTION_END)) &&
                                  !strncmp(ch + strlen(PREF_ELEM) + 1 + strlen(item.element.name) + strlen(SECTION_END), POST_ELEM, strlen(POST_ELEM))
                                ) {
                                *ch = '\0';
                                item.element.section = do_template_read_section(head);
                                ch +=
                                strlen(PREF_ELEM) + 1 + strlen(item.element.name) +
                                    strlen(SECTION_END) + strlen(POST_ELEM);
                                break;
                            }
                            else ch++;
                        }
                        if (!item.element.section) {
                            do_log(LOG_ERR, "Section \"%s\" not end", item.element.name);
                            do_free(item.element.name);
                            do_template_items_free(retval);
                            return NULL;
                        }

                    }
                    else {
                        item.type = DO_TEMPLATE_TYPE_ELEMENT;
                        item.element.name = do_strdup(head);
                        head = ch;
                        while ( ch[0] != '\0' && ch[0] != '\n') ch++;
                        if ( ch[0] == '\n' ) {
                            *ch = '\0';
                            ch++;
                        }
                        item.element.default_value = do_strdup(head);
                    }
                    head = ch;
                    do_list_add_alloc(retval, &item, sizeof(item));
                    break;
                }
                else
                    ch++;
            }
        }
        else ch++;
    }
    if ( head != ch ) {
        item.type = DO_TEMPLATE_TYPE_TEXT;
        item.text = do_strdup(head);
        do_list_add_alloc(retval, &item, sizeof(item));
    }
    return retval;
}

static do_list_t *do_template_read_file(const char *filename)
{
    FILE *fp;
    long len, l;
    char *buf;
    do_list_t *retval;

    if ( (fp = fopen(filename, "r")) == NULL )  {
        do_log(LOG_ERR, "Error opening templation file (%s): %s\n", filename, strerror(errno));
        return NULL;
    }
    fseek (fp, 0L, SEEK_END);
    len = ftell (fp);
    if ( !len )
        return do_list_new(FALSE);

    if ( (buf = do_malloc(len + 1)) == NULL ) {
        do_log(LOG_ERR, "Could not allocate memory %d byte", len);
        return NULL;
    }
    errno = 0;
    l = fseek (fp, 0L, SEEK_SET);
    if ( ((l = fread(buf, 1, len, fp)) != len ) && (errno) ) {
        do_log(LOG_ERR, "Error read file (%s): %s", filename, strerror(errno));
        fclose(fp);
        do_free(buf);
        return NULL;
    }
    fclose(fp);
    buf[l] = '\0';
    retval = do_template_read_section(buf);
    do_free(buf);
    return retval;
}

static void do_template_init(do_template_t *templ)
{
    templ->values = do_list_new(FALSE);
    templ->cursors = do_list_new(TRUE);
    do_template_cursor_t *cursor;

    cursor = do_malloc0(sizeof(do_template_cursor_t));
    cursor->owner = templ;
    cursor->items = templ->items;
    cursor->indx = 0;
    do_list_add(templ->cursors, cursor);

}
static char *do_template_file_name(const char* name)
{
    char t_name[1024];
    int i, j, k;

    for (i = 0; i < 3; i++) {
        char *path;
        switch (i) {
            case 2:
                path = do_strdup(DOMINO_PATH);
                break;
            case 1:
                path = do_strdup("../.");
                break;
            case 0:
                path = do_strdup(".");
                break;
            default:
                path = do_strdup(".");
        }
        for (j = 0; j < 2; j++) {
            char *f_name;

            if (!j)
                sprintf(t_name, "%s", name);
            else
                sprintf(t_name, "%s.html", name);

            for (k = 0; k < 2; k++) {
                if ( !k ) {
                    f_name = do_malloc(strlen(path)+strlen(t_name)+10);
                    sprintf(f_name, "%s/%s", path,  t_name);
                }
                else {
                    f_name = do_malloc(strlen(path)+strlen(t_name)+10+strlen("template"));
                    sprintf(f_name, "%s/../template/%s", path,  t_name);
                }
                FILE *fp;
                if ( (fp = fopen(f_name, "r")) != NULL )  {
                    do_free(path);
                    fclose(fp);
                    return f_name;
                }
            }
        }
        do_free(path);
    }
    return NULL;
}
do_template_t *do_template_new(const char *filename, int use_utf)
{
    do_template_t *templ;
    templ = do_malloc0(sizeof(do_template_t));
    if (!use_utf) {
        const char *str;
        str = get_locale_lang();
        templ->conv = iconv_open(UTF_CHARSET, str);
        if (templ->conv == (iconv_t)-1) {
            do_log(LOG_ERR, "Error iconv_open (\"%s\" to \"%s\") ", str, UTF_CHARSET);
            do_free(templ);
            return NULL;
        }
    }
    templ->use_utf = use_utf;
    char *f_name;

    f_name = do_template_file_name(filename);
    if ( !f_name ) {
        do_log(LOG_ERR, "template \"%s\" not found", filename);
        do_free(templ);
        return NULL;
    }

    templ->items = do_template_read_file(f_name);
    do_free(f_name);

    if ( !templ->items ) {
        do_free(templ);
        return NULL;
    }
    templ->text = do_string_list_new();
    do_template_init(templ);
    return templ;
}
static void do_template_items_free(do_list_t *list)
{
    int i;
    do_template_item_t *item;
    for (i = 0; i < list->count; i++) {
        item = list->list[i];
        switch (item->type) {
            case DO_TEMPLATE_TYPE_TEXT:
                do_free(item->text);
                break;
            case DO_TEMPLATE_TYPE_ELEMENT:
                do_free(item->element.name);
                do_free(item->element.default_value);
                break;
            case DO_TEMPLATE_TYPE_SECTION:
                do_template_items_free(item->element.section);
                do_free(item->element.name);
                break;
            case DO_TEMPLATE_TYPE_UNKNOW:
                break;
        }
        do_free(item);
    }
    do_list_free(list);
}
static void do_template_values_free(do_list_t *list)
{
    int i;
    do_template_value_t *item;
    for (i = 0; i < list->count; i++) {
        item = list->list[i];
        do_free(item->text);
        do_free(item->name);
        do_free(item);
    }
    do_list_free(list);

}
void do_template_free(do_template_t *templ)
{
    do_template_items_free(templ->items);
    do_list_free(templ->cursors);
    do_template_values_free(templ->values);
    do_string_list_free(templ->text);
    if (!templ->use_utf && templ->conv != (iconv_t) -1)
        iconv_close(templ->conv);
    do_free(templ);
}

void do_template_clear(do_template_t *templ)
{
    do_list_free(templ->cursors);
    do_template_values_free(templ->values);
    do_template_init(templ);
    do_string_list_clear(templ->text);
}
static void do_template_value_set(do_template_t *templ, do_list_t *list, const char *name, const char *value)
{
    int i;
    do_template_value_t *item;
    for (i = 0; i < list->count; i++) {
        item = list->list[i];
        if (!strcmp(item->name, name)) {
            do_free(item->text);
            if (templ->use_utf)
                item->text = do_strdup(value);
            else
                item->text = iconv_(templ->conv, value);
            return;
        }
    }
    item = do_malloc(sizeof(do_template_value_t));
    item->name = do_strdup(name);
    if ( value ) {
    if (templ->use_utf)
        item->text = do_strdup(value);
    else
        item->text = iconv_(templ->conv, value);
    }
    else
        item->text = do_strdup("");
    do_list_add(list, item);
}

void  do_template_set(do_template_t *templ, ...)
{
    va_list args;
    char *name, *value;
    va_start (args, templ);
    name = va_arg (args, char *);
    while ( name != NULL ) {
        value = va_arg (args, char *);
        do_template_value_set(templ, templ->values, name, value);
        name = va_arg (args, char *);
    }
    va_end (args);
}

void do_template_default_value_enable(do_template_t *templ, int value)
{
    templ->default_value = value;
}

do_template_cursor_t *do_template_cursor(do_template_t *templ)
{
    return templ->cursors->list[0];
}
do_template_cursor_t *do_template_section(do_template_t *templ, const char *name)
{
    int i;
    do_template_item_t *item;
    for ( i = 0; i < templ->items->count; i++ ) {
        item = templ->items->list[i];
        if ( item->type == DO_TEMPLATE_TYPE_SECTION &&
             !strcmp(item->element.name, name) ) {
            do_template_cursor_t *cursor;
            cursor = do_malloc0(sizeof(do_template_cursor_t));
            cursor->owner = templ;
            cursor->items = item->element.section;
            cursor->indx = 0;
            do_list_add(templ->cursors, cursor);
            return cursor;
        }
    }
    return NULL;
}
void do_template_cursor_first(do_template_cursor_t *cursor)
{
    cursor->indx = 0;
}
int do_template_cursor_step(do_template_cursor_t *cursor, int write)
{
    if (!cursor)
        return FALSE;

    int i, j, k;
    do_template_item_t *item;
    do_template_value_t *v;
    do_template_t *templ;

    templ = cursor->owner;

    for ( i = cursor->indx; i < cursor->items->count; i++ ) {
        item = cursor->items->list[i];
        switch ( item->type ) {
            case DO_TEMPLATE_TYPE_TEXT:
                if ( write )
                    do_string_list_add_alloc(templ->text, item->text);
                break;
            case DO_TEMPLATE_TYPE_ELEMENT:
                k = 0;
                for ( j = 0; j < templ->values->count; j++ ) {
                    v = templ->values->list[j];
                    if ( !strcmp(v->name, item->element.name) ) {
                        if ( write )
                            do_string_list_add_alloc(templ->text, v->text);
                        k = 1;
                        break;
                    }
                }
                if ( !k && write ) {
                    if ( templ->default_value )
                        do_string_list_add_alloc(templ->text, item->element.default_value);
                    else
                        do_string_list_add_alloc(templ->text, "&nbsp;");
                }
                do_string_list_add_alloc(templ->text, "\n");
                break;
            case DO_TEMPLATE_TYPE_SECTION:
                if ( i != cursor->indx ) {
                    cursor->indx = i;
                    return TRUE;
                }
                if ( write ) {
                    do_template_cursor_t *sub;
                    sub = do_malloc0(sizeof(do_template_cursor_t));
                    sub->owner = templ;
                    sub->items = item->element.section;
                    do_template_cursor_run(sub);
                    do_free(sub);
                }
                break;
            case DO_TEMPLATE_TYPE_UNKNOW:
                return FALSE;
        }
    }
    cursor->indx = i;
    return cursor->indx < cursor->items->count;
}

void do_template_cursor_first_run(do_template_cursor_t *cursor)
{
    do_template_cursor_first(cursor);
    do_template_cursor_run(cursor);
}
void do_template_cursor_run(do_template_cursor_t *cursor)
{
    while ( do_template_cursor_step(cursor, TRUE) );
}
int do_template_cursor_run_step(do_template_cursor_t *cursor)
{
    return do_template_cursor_step(cursor, TRUE);
}
int do_template_cursor_run_step_for(do_template_cursor_t *cursor, const char *name)
{
    if ( !cursor )
        return FALSE;

    do_template_item_t *item;
    while ( cursor->indx < cursor->items->count ) {
        item = cursor->items->list[cursor->indx];
        if ( item->type != DO_TEMPLATE_TYPE_SECTION ||
             strcmp(item->element.name, name) ) {
            do_template_cursor_step(cursor, TRUE);
        }
        else
            return TRUE;
    }
    return FALSE;
}
void do_template_cursor_skip(do_template_cursor_t *cursor)
{
    do_template_cursor_step(cursor, FALSE);
}
int do_template_cursor_skip_for(do_template_cursor_t *cursor, const char *name)
{
    if ( !cursor )
        return FALSE;

    do_template_item_t *item;
    while ( cursor->indx < cursor->items->count ) {
        item = cursor->items->list[cursor->indx];
        if ( item->type != DO_TEMPLATE_TYPE_SECTION ||
             strcmp(item->element.name, name) ) {
            do_template_cursor_step(cursor, FALSE);
        }
        else
            return TRUE;
    }
    return FALSE;
}

char *do_template_text(do_template_t *templ)
{
    int i;
    long len;
    char *retval;

    for ( i = 0, len = 0; i < templ->text->count; i++ )
	len+=strlen(templ->text->list[i]);

    retval = do_malloc(len + 1);
    if ( !retval ) {
        do_log(LOG_ERR, "Could not allocate memory %d byte", len + 1);
        return NULL;
    }

    for ( i = 0, len = 0; i < templ->text->count; i++ ) {
	int size = strlen(templ->text->list[i]);
        memcpy(retval + len, templ->text->list[i], size);
        len += size;
    }

    retval[len] = '\0';
    return retval;
}
int do_template_save_to_file(do_template_t *templ, const char *filename)
{
    FILE *fp;
    if ((fp = fopen(filename, "w")) == NULL)  {
        do_log(LOG_ERR, "Error opening for write file (%s): %s", filename, strerror(errno));
        return 0;
    }
    int i;
    size_t n, size;
    for ( i = 0; i < templ->text->count; i++ ) {
        size = strlen(templ->text->list[i]);
        if ((n = fwrite(templ->text->list[i], 1, size, fp)) != size ) {
            do_log(LOG_ERR, "Error writing file (%s): %s", filename, strerror(errno));
            fclose(fp);
            return FALSE;
        }
    }
    fclose(fp);
    return 1;
}
void do_template_add_page_break(do_template_t *templ)
{
    do_string_list_add_alloc(templ->text, "<br clear=all style='mso-special-character:line-break;page-break-before:always'>");
}
