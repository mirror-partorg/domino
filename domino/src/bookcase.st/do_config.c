#ifdef EXPAT
#include "do_config.h"
#include <dolib.h>
#include <expat.h>
#include <errno.h>
#include <glib.h>
#include "../../misc/define.h"

typedef struct _object_t object_t;
typedef struct _private_t private_t;

struct _object_t {
    char *id;
    char *kind;
    do_string_list_t *attrs;
    do_string_list_t *properties;
    do_sort_list_t *children;
    char *data;
};

struct _private_t {
    char *encoding;
    do_sort_list_t *objs;
};
typedef enum {
    ELEM_UNKNOWN,
    ELEM_CONFIG,
    ELEM_OBJECT,
    ELEM_PROPERTY,
}elem_type_t;
struct _do_config_t {
    char *name;
};

#define get_priv(a) ((private_t*)(((do_config_t*)a) + 1))
#ifdef _LINUX
#include <wchar.h>
#include <wctype.h>
#include <ctype.h>

static char* atowupper(const char* value)
{
    char *str;
    str = do_strdup(value);
    if( MB_CUR_MAX > 1 )
    {
        int i,len = strlen(str);
        wchar_t wc;
        mbstate_t cur_state;
        memset(&cur_state, 0, sizeof(mbstate_t));
        for(i = 0; i <= len;)
        {
            size_t mbclen = mbrtowc(&wc, str + i, len - i, &cur_state);
            if (mbclen == (size_t) -1 || mbclen == (size_t) -2 || mbclen == 0 )
               mbclen = 1;
            else
            {
                if (iswlower((wint_t)wc))
                {
                    wc = towupper((wint_t)wc);
                    if (wcrtomb(str + i, wc, &cur_state) == -1)
                	return str;
                }
            }
            i += mbclen;
        }
    }
    else {
        char *cp;
        for(cp=str; *cp; cp++) if (islower(*cp)) *cp = toupper(*cp);
    }
    return str;
}
#elif defined(_WIN32)
#include <windows.h>
static char* atowupper(const char* value)
{
    char *str;
    str = do_strdup(value);
    return AnsiUpper(str);
}
#endif

static int object_free(object_t *obj)
{
    if (obj->id)
        do_free(obj->id);
    if (obj->kind)
        do_free(obj->kind);
    if (obj->attrs)
        do_string_list_free(obj->attrs);
    if (obj->properties)
        do_string_list_free(obj->properties);
    if (obj->data)
        do_free(obj->data);
    if (obj->children) {
        do_list_foreach((do_list_t*)obj->children, (do_list_foreach_func)object_free, NULL);
        do_sort_list_free(obj->children);
    }
    return TRUE;
}
static int objcmp(void *a, void *b, void *user_data)
{
    return strcmp(((object_t*)a)->id, ((object_t*)b)->id);
}


DO_EXPORT do_config_t *do_config_new(const char *encoding, const char *name)
{
    do_config_t *retval;
    private_t *priv;
    retval = (do_config_t *)do_malloc0(sizeof(retval) +
                                             sizeof(private_t));
    retval->name = do_strdup(name);
    priv = get_priv(retval);
    if (encoding)
        priv->encoding = do_strdup(encoding);
    priv->objs = do_sort_list_new(0, 1, objcmp, NULL);
    return retval;
}

DO_EXPORT void do_config_free(do_config_t *config)
{
    do_free(config->name);
    private_t *priv = get_priv(config);
    if (priv->encoding)
        do_free(priv->encoding);
    int i;
    for(i = 0; i < priv->objs->count; i++)
        object_free(priv->objs->list[i]);
    do_sort_list_free(priv->objs);
}

static elem_type_t get_elem_type (const char *s);
static void start (void *data, const char *el, const char **attr);
static void cdata (void *data, const char *s, int len);
static void end (void *data, const char *el);

typedef struct {
    int config_tag;
    do_list_t *stack_obj;
	elem_type_t elem_type;

    private_t *priv;
    char s_error[1024];
    XML_Parser parser;
} context_t;

DO_EXPORT int do_config_read(do_config_t *config, const char *filename, int empty)
{
    private_t *priv = get_priv(config);
    int rc;
    context_t context;
    char buf[1024];
    size_t len;

    FILE *fp;
    if ((fp = fopen(filename, "rb")) == NULL)  {
	if ( !empty ) {
	        do_log(LOG_ERR, "Error opening file (%s): %s", filename, strerror(errno));
        	return 0;
	}
	return 1;
    }
    context.config_tag = 0;
    context.priv = priv;
    context.elem_type = ELEM_UNKNOWN;
    context.stack_obj = do_list_new(0);
    *context.s_error = '\0';

	context.parser = XML_ParserCreate (NULL);

	if (context.parser == NULL) {
        do_list_free(context.stack_obj);
		do_log(LOG_ERR, "Couldn't allocate memory for xml parser");
		return 0;
	}
    errno = 0;

	XML_SetUserData (context.parser, &context);
	XML_SetElementHandler (context.parser, start, end);
	XML_SetCharacterDataHandler (context.parser, cdata);

	while ((len = fread(buf, 1, sizeof(buf), fp)) > 0) {

        rc = XML_Parse (context.parser, buf, len, feof(fp));
        if (rc == 0) {
            if (XML_GetErrorCode (context.parser) == XML_ERROR_ABORTED) {
                do_log(LOG_ERR, "%s:%d: semantic error: %s",
                        filename,
                        (int) XML_GetCurrentLineNumber (context.parser),
                        context.s_error);
            } else {
                do_log(LOG_ERR,
                    "error in fdi file %s:%d: %s",
                    filename,
                    (int) XML_GetCurrentLineNumber (context.parser),
                    XML_ErrorString (XML_GetErrorCode (context.parser)));
            }
        }
	}
	XML_ParserFree (context.parser);
	do_list_free(context.stack_obj);

    if (errno) {
        do_log(LOG_ERR, "Error read file (%s): %s", filename, strerror(errno));
        fclose(fp);
        return 0;
    }
    fclose(fp);
	return 1;
};

static void start (void *data, const char *el, const char **attr)
{
	context_t *context = data;
	object_t *crnt_obj = NULL;
	int indx;

	context->elem_type = get_elem_type(el);
    if (context->stack_obj->count)
        crnt_obj = context->stack_obj->list[context->stack_obj->count-1];

    if (!context->config_tag && context->elem_type != ELEM_CONFIG)
        return;


    switch (context->elem_type) {
        case ELEM_UNKNOWN:
            snprintf (context->s_error, sizeof (context->s_error),
                         "Bad element %s", el);
            XML_StopParser (context->parser, 0);
            return;
        case ELEM_CONFIG:
            if (context->config_tag) {
                snprintf (context->s_error, sizeof (context->s_error),
                             "session not close");
                XML_StopParser (context->parser, 0);
                return;
            }
            context->config_tag = 1;
            return;
        case ELEM_OBJECT: {
            object_t *new_obj = do_malloc0(sizeof(context_t));
            for (indx = 0; attr[indx] != NULL; indx+=2) {
                if (!strcmp(attr[indx], "id")) {
                    if (new_obj->id) {
                        snprintf (context->s_error, sizeof (context->s_error), "Bad attribute: id already defined");
                        XML_StopParser (context->parser, 0);
                        object_free(new_obj);
                        return;
                    }
                    new_obj->id = do_strdup(attr[indx+1]);
                    continue;
                }
                if (!strcmp(attr[indx], "kind")) {
                    if (new_obj->kind) {
                        snprintf (context->s_error, sizeof (context->s_error), "Bad attribute: kind already defined");
                        XML_StopParser (context->parser, 0);
                        object_free(new_obj);
                        return;
                    }
                    new_obj->kind = do_strdup(attr[indx+1]);
                    continue;
                }
                if (!new_obj->attrs)
                    new_obj->attrs = do_string_list_new();
                do_string_list_add_alloc(new_obj->attrs, attr[indx]);
                do_string_list_add_alloc(new_obj->attrs, attr[indx+1]);
            }
            if (!new_obj->id) {
                snprintf (context->s_error, sizeof (context->s_error), "Bad object: value without a id");
                XML_StopParser (context->parser, 0);
                object_free(new_obj);
                return;
            }
            do_list_add(context->stack_obj, new_obj);
            if (!crnt_obj)
                do_sort_list_add(context->priv->objs, new_obj);
            else {
                if (!crnt_obj->children)
                    crnt_obj->children = do_sort_list_new(0, 1, objcmp, NULL);
                do_sort_list_add(crnt_obj->children, new_obj);
            }
            break;
        }
        case ELEM_PROPERTY:
            if (!crnt_obj) {
                snprintf (context->s_error, sizeof (context->s_error), "Bad: property without a object");
                XML_StopParser (context->parser, 0);
                return;
            }
            if (!crnt_obj->properties)
                crnt_obj->properties = do_string_list_new();
            for (indx = 0; attr[indx] != NULL; indx+=2)
                if (!strcmp(attr[indx], "name")) {

                    do_string_list_add_alloc(crnt_obj->properties, attr[indx+1]);
                    do_string_list_add_alloc(crnt_obj->properties, "");
                    return;
                }
            snprintf (context->s_error, sizeof (context->s_error), "Bad property: value without a name");
            XML_StopParser (context->parser, 0);
            return;
    }
}

static void
cdata (void *data, const char *s, int len){

    context_t *context = data;
    do_string_list_t *properties;

    if (context->stack_obj->count &&
        context->elem_type == ELEM_PROPERTY) {
        properties = ((object_t*)(context->stack_obj->list[context->stack_obj->count-1]))->properties;
        if (properties->count) {
            int old_len = strlen(properties->list[properties->count - 1]);
            properties->list[properties->count - 1] = do_realloc(properties->list[properties->count - 1], old_len + len + 1);
            strncpy(properties->list[properties->count - 1] + old_len , s, len);
            ((char*)(properties->list[properties->count - 1]))[old_len + len] = '\0';
        }
    }
}

static void
end (void *data, const char *el){
    context_t *context = data;
    if (context->config_tag) {
        context->elem_type = ELEM_UNKNOWN;
        switch (get_elem_type(el)) {
            case ELEM_OBJECT:
                do_list_delete(context->stack_obj, context->stack_obj->count - 1);
                break;
            case ELEM_CONFIG:
                context->config_tag = 0;
                break;
            default:
                break;
        }
    }
}

static elem_type_t get_elem_type (const char *s)
{
	if (strcmp (s, "config") == 0)
		return ELEM_CONFIG;
	if (strcmp (s, "object") == 0)
		return ELEM_OBJECT;
	if (strcmp (s, "property") == 0)
		return ELEM_PROPERTY;
	return ELEM_UNKNOWN;
}

static int object_save(object_t *obj, FILE *fp, const char *filename, char *tab)
{
    char buf[1024];
    int i;
    size_t size, n;

    if (obj->kind)
        sprintf(buf, "%s<object id=\"%s\" kind=\"%s\" ", tab, obj->id, obj->kind);
    else
        sprintf(buf, "%s<object id=\"%s\" ", tab, obj->id);
    for (i = 0; obj->attrs && i < obj->attrs->count; i+=2) {
        char b[1024];
        sprintf(b, "%s=\"%s\" ", (char*)obj->attrs->list[i], (char*)obj->attrs->list[i+1]);
        strcat(buf, b);
    }

    strcat(buf, ">\n");
    size = strlen(buf);
    if ((n = fwrite(buf, 1, size, fp)) != size ) {
        do_log(LOG_ERR, "Error writing file (%s): %s", filename, strerror(errno));
        fclose(fp);
        return 0;
    }
    tab[strlen(tab)] = ' ';
    tab[strlen(tab)] = ' ';
    for (i = 0; obj->properties && i < obj->properties->count; i+=2) {
        sprintf(buf, "%s<property name=\"%s\">%s</property>\n", tab, (char*)obj->properties->list[i],
                                                                   (char*)obj->properties->list[i+1]);
        size = strlen(buf);
        if ((n = fwrite(buf, 1, size, fp)) != size ) {
            do_log(LOG_ERR, "Error writing file (%s): %s", filename, strerror(errno));
            fclose(fp);
            return 0;
        }
    }
    for (i = 0; obj->children && i < obj->children->count; i++)
        if (!object_save(obj->children->list[i], fp, filename, tab))
            return 0;
    tab[strlen(tab) - 1] = '\0';
    tab[strlen(tab) - 1] = '\0';
    if (obj->data) {
        size = strlen(obj->data);
        if ((n = fwrite(obj->data, 1, size, fp)) != size ) {
            do_log(LOG_ERR, "Error writing file (%s): %s", filename, strerror(errno));
            fclose(fp);
            return 0;
        }
    }
    sprintf(buf, "%s</object>\n", tab);
    size = strlen(buf);
    if ((n = fwrite(buf, 1, size, fp)) != size ) {
        do_log(LOG_ERR, "Error writing file (%s): %s", filename, strerror(errno));
        fclose(fp);
        return 0;
    }
    return 1;
}

DO_EXPORT int do_config_write(do_config_t *config, const char *filename)
{
    private_t *priv = get_priv(config);
    FILE *fp;
    char buf[1024];
    char buftab[1024];
    const char *en = DO_UTF_CHARSET;
    size_t n, size;
    int i;

    if ((fp = fopen(filename, "wb")) == NULL)  {
        do_log(LOG_ERR, "Error opening for write file (%s): %s", filename, strerror(errno));
        return 0;
    }
    memset(buftab, 0, sizeof(buftab));
    if (priv->encoding)
        en = priv->encoding;

    sprintf(buf, "<?xml version=\"1.0\" encoding=\"%s\"?>\n<config>\n", en);
    size = strlen(buf);
    if ((n = fwrite(buf, 1, size, fp)) != size ) {
        do_log(LOG_ERR, "Error writing file (%s): %s", filename, strerror(errno));
        fclose(fp);
        return 0;
    }
    buftab[0] = ' ';
    buftab[1] = ' ';
    for (i = 0; i < priv->objs->count; i++)
        if (!object_save(priv->objs->list[i], fp, filename, buftab)) {
            fclose(fp);
            return 0;
        }

    sprintf(buf, "</config>");
    size = strlen(buf);
    if ((n = fwrite(buf, 1, size, fp)) != size ) {
        do_log(LOG_ERR, "Error writing file (%s): %s", filename, strerror(errno));
        fclose(fp);
        return 0;
    }
    fclose(fp);
    return 1;

}
DO_EXPORT int do_config_write_text(const char *filename, const char *text)
{
    FILE *fp;
    size_t n, size;
    size = strlen(text);
    if ((fp = fopen(filename, "wb")) == NULL)  {
        do_log(LOG_ERR, "Error opening for write file (%s): %s", filename, strerror(errno));
        return 0;
    }
    if ((n = fwrite(text, 1, size, fp)) != size ) {
        do_log(LOG_ERR, "Error writing file (%s): %s", filename, strerror(errno));
        fclose(fp);
        return 0;
    }
    fclose(fp);
    return 1;

}

static object_t *get_obj(do_config_t *config, const char *path)
{
    private_t *priv = get_priv(config);
    object_t *obj = NULL;
    do_sort_list_t *list = priv->objs;
    const char *path_tail, *ch;
    char name[1024];
    int indx;
    object_t new_obj;
    new_obj.id = name;

    path_tail = path;

    do  {
        ch = strchr (path_tail, '/');
        if (ch) {
            strncpy(name, path_tail, ch - path_tail);
            name[ch - path_tail] =  '\0';
            path_tail = ch + 1;
        }
        else {
            strcpy(name, path_tail);
            path_tail += strlen(path_tail);
        }
        if (!list || !do_sort_list_find(list, &indx, &new_obj))
            return NULL;
        obj = list->list[indx];
        list = obj->children;
 	} while (*path_tail != '\0');

 	return obj;
}
static object_t *set_obj(do_config_t *config, const char *path)
{
    private_t *priv = get_priv(config);
    object_t *obj = NULL;
    do_sort_list_t *list = priv->objs;
    const char *path_tail, *ch;
    char name[1024];
    int indx;
    object_t new_obj;
    new_obj.id = name;

    path_tail = path;
    do  {
        ch = strchr (path_tail, '/');
        if (ch) {
            strncpy(name, path_tail, ch - path_tail);
            name[ch - path_tail] =  '\0';
            path_tail = ch + 1;
        }
        else {
            strcpy(name, path_tail);
            path_tail += strlen(path_tail);
        }
        if (!do_sort_list_find(list, &indx, &new_obj)) {
            obj = do_malloc0(sizeof(context_t));
            obj->id = do_strdup(name);
            do_sort_list_add(list, obj);
        }
        else
            obj = list->list[indx];
        if (!obj->children)
            obj->children = do_sort_list_new(0, 1, objcmp, NULL);
        list = obj->children;
 	} while (*path_tail != '\0');

 	return obj;
}

DO_EXPORT const char  *do_config_get_kind(do_config_t *config, const char *path)
{
    object_t *obj;
    obj = get_obj(config, path);
    if (obj)
        return obj->kind;
    return NULL;
}
DO_EXPORT const char  *do_config_set_kind(do_config_t *config, const char *path, const char *kind)
{
    object_t *obj;
    obj = set_obj(config, path);
    if (obj->kind)
        do_free(obj->kind);
    obj->kind = do_strdup(kind);
    return NULL;
}

DO_EXPORT const char  *do_config_get_attr(do_config_t *config, const char *path, const char *attr)
{
    object_t *obj;
    obj = get_obj(config, path);
    if ( !obj || !obj->attrs )
        return NULL;
    int i;
    for (i = 0; i < obj->attrs->count; i+=2)
        if (!strcmp((char*)obj->attrs->list[i], attr))
            return (char*)obj->attrs->list[i+1];
    return NULL;
}
DO_EXPORT int    do_config_get_attr_int(do_config_t *config, const char *path, const char *attr)
{
    const char *value;
    value = do_config_get_attr(config, path, attr);
    if (value)
        return atoi(value);
    else
        return 0;
}
DO_EXPORT double do_config_get_attr_double(do_config_t *config, const char *path, const char *attr)
{
    const char *value;
    value = do_config_get_attr(config, path, attr);
    if (value)
        return atof(value);
    else
        return 0;
}
DO_EXPORT int    do_config_get_attr_bool(do_config_t *config, const char *path, const char *attr)
{
    const char *value;
    int retval = FALSE;
    value = do_config_get_attr(config, path, attr);
    if ( value ) {
        char *upper;
        upper = atowupper(value);
        retval = !strcmp(upper, "TRUE") ||
                 !strcmp(upper, "YES") ||
                 !strcmp(upper, "1");
        do_free(upper);
    }

    return retval;
}
DO_EXPORT void   do_config_set_attr(do_config_t *config, const char *path, const char *attr, const char *value)
{
    object_t *obj;
    int i;
    obj = set_obj(config, path);
    if (!obj->attrs)
        obj->attrs = do_string_list_new();
    for (i = 0; i < obj->attrs->count; i+=2)
        if (!strcmp(obj->attrs->list[i], attr)) {
            do_free(obj->attrs->list[i+1]);
            obj->attrs->list[i+1] = do_strdup(value);
            return;
        }
    do_string_list_add_alloc(obj->attrs, attr);
    do_string_list_add_alloc(obj->attrs, value);
}

DO_EXPORT void   do_config_set_attr_int(do_config_t *config, const char *path, const char *attr, int value)
{
    char buff[100];
    sprintf(buff, "%d", value);
    do_config_set_attr(config, path, attr, buff);
}
DO_EXPORT void   do_config_set_attr_double(do_config_t *config, const char *path, const char *attr, double value)
{
    char buff[100];
    sprintf(buff, "%f", value);
    do_config_set_attr(config, path, attr, buff);
}
DO_EXPORT void   do_config_set_attr_bool(do_config_t *config, const char *path, const char *attr, int value)
{
    char buff[100];
    if (value)
        sprintf(buff, "TRUE");
    else
        sprintf(buff, "FALSE");
    do_config_set_attr(config, path, attr, buff);
}

DO_EXPORT const do_string_list_t *do_config_get_properties(do_config_t *config, const char *path)
{
    object_t *obj;
    obj = get_obj(config, path);
    if (!obj)
        return NULL;
    return obj->properties;
}
DO_EXPORT do_string_list_t *do_config_get_children(do_config_t *config, const char *path)
{
    do_string_list_t *retval;
    retval = do_string_list_new();
    object_t *obj;
    obj = get_obj(config, path);
    if ( obj && obj->children ) {
        int i;
        object_t *child;
        char *full_path;

        for ( i = 0; i < obj->children->count; i++ ) {
            child = obj->children->list[i];
            full_path = do_malloc(strlen(path) + strlen(child->id) + 5);
            sprintf(full_path, "%s/%s", path, child->id);
            do_string_list_add(retval, full_path);
        }
    }
    return retval;
}

DO_EXPORT const char  *do_config_get_property(do_config_t *config, const char *path, const char *property)
{
    object_t *obj;
    obj = get_obj(config, path);
    if (!obj || !obj->properties)
        return NULL;
    int i;
    for (i = 0; i < obj->properties->count; i+=2)
        if (!strcmp((char*)obj->properties->list[i], property))
            return (char*)obj->properties->list[i+1];
    return NULL;
}
DO_EXPORT int    do_config_get_property_int(do_config_t *config, const char *path, const char *property)
{
    const char *value;
    value = do_config_get_property(config, path, property);
    if (value)
        return atoi(value);
    else
        return 0;
}
DO_EXPORT double do_config_get_property_double(do_config_t *config, const char *path, const char *property)
{
    const char *value;
    value = do_config_get_property(config, path, property);
    if (value)
        return atof(value);
    else
        return 0;
}
DO_EXPORT int    do_config_get_property_bool(do_config_t *config, const char *path, const char *property)
{
    const char *value;
    value = do_config_get_property(config, path, property);
    if (value)
        return !strcmp(value, "TRUE");
    else
        return 0;
}
DO_EXPORT void   do_config_set_property(do_config_t *config, const char *path, const char *property, const char *value)
{
    object_t *obj;
    int i;
    obj = set_obj(config, path);
    if (!obj->properties)
        obj->properties = do_string_list_new();
    for (i = 0; i < obj->properties->count; i+=2)
        if (!strcmp(obj->properties->list[i], property)) {
            do_free(obj->properties->list[i+1]);
            obj->properties->list[i+1] = do_strdup(value);
            return;
        }
    do_string_list_add_alloc(obj->properties, property);
    do_string_list_add_alloc(obj->properties, value);
}
DO_EXPORT void   do_config_clear_property(do_config_t *config, const char *path, const char *property)
{
    object_t *obj;
    int i;
    obj = get_obj(config, path);
    if (obj && obj->properties)
        for (i = 0; i < obj->properties->count; i+=2)
            if (!strcmp(obj->properties->list[i], property)) {
                do_string_list_delete(obj->properties, i);
                do_string_list_delete(obj->properties, i);
                return;
            }
}

DO_EXPORT void   do_config_set_property_int(do_config_t *config, const char *path, const char *property, int value)
{
    char buff[100];
    sprintf(buff, "%d", value);
    do_config_set_property(config, path, property, buff);
}
DO_EXPORT void   do_config_set_property_double(do_config_t *config, const char *path, const char *property, double value)
{
    char buff[100];
    sprintf(buff, "%f", value);
    do_config_set_property(config, path, property, buff);
}
DO_EXPORT void   do_config_set_property_bool(do_config_t *config, const char *path, const char *property, int value)
{
    char buff[100];
    if (value)
        sprintf(buff, "TRUE");
    else
        sprintf(buff, "FALSE");
    do_config_set_property(config, path, property, buff);
}

#endif
