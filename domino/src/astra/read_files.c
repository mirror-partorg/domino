
#include <expat.h>
#include <errno.h>
#include <iconv.h>
#include <dolib.h>
#include <locale.h>

#include "read_files.h"
#include "date_util.h"
#include "pfizer-const.h"

typedef enum {
    ELEM_BR = 0,
    ELEM_TERRITORY_CODE,
    ELEM_VERSION,
    ELEM_EXPORT_DATE,
    ELEM_CARD_TYPE,
    ELEM_CARD_TYPE_CODE,
    ELEM_CARD_TYPE_DESC,
    ELEM_CARD_NUMBER_FROM,
    ELEM_CARD_NUMBER_TO,
    ELEM_CARD_ONE_OFF,
    ELEM_RULES,
    ELEM_SKU_RULE,
    ELEM_SKU_RULE_ID,
    ELEM_DATE_FROM,
    ELEM_DATE_TO,
    ELEM_SKU_CODE,
    ELEM_SKU_DESC,
    ELEM_SKU_QUANT,
    ELEM_SKU_MAXQUANT,
    ELEM_SKU_MAXDISCOUNT,
    ELEM_DISCOUNT,
    ELEM_STOP_CARDS,
    ELEM_CARD,
    ELEM_STOP_DATE,
    ELEM_CARD_NUMBER,
    ELEM_REACT_DATE,
    ELEM_EXPORT_DATETIME,
    ELEM_DAILY_REPORT,
    ELEM_DAILY_RULE,
    ELEM_DAILY_RULE_ID,
    ELEM_MONTHLY_REPORT,
    ELEM_MONTHLY_RULE,
    ELEM_MONTHLY_RULE_ID,
    ELEM_UNKNOWN,
} elem_type_t;

const char *elem_name[] = {
    "business_rules",
    "territory_code",
    "version",
    "export_date",
    "card_type",
    "card_type_code",
    "card_type_description",
    "card_number_from",
    "card_number_to",
    "card_one_off",
    "rules",
    "sku_rule",
    "sku_rule_id",
    "date_from",
    "date_to",
    "sku_code",
    "sku_description",
    "sku_quant",
    "sku_maxquant",
    "sku_maxdiscount",
    "discount",
    "stop_cards",
    "card",
    "stop_date",
    "card_number",
    "reactivation_date",
    "export_datetime",
    "daily_report",
    "daily_rule",
    "daily_rule_id",
    "monthly_report",
    "monthly_rule",
    "monthly_rule_id",
};

typedef struct _obj_tmp_t obj_tmp_t;
typedef struct _stack_t stack_t;

typedef struct {
    pfizer_rules_t *rules;
    pfizer_rule_t  *crnt_rule;
    pfizer_line_t  *crnt_line;

    stack_t *stack;

    char s_error[1024];
    XML_Parser parser;
} rules_context_t;

static elem_type_t get_elem_type (const char *s);
static void rules_start (void *data, const char *el, const char **attr);
static void rules_cdata (void *data, const char *s, int len);
static void rules_end (void *data, const char *el);
static char *str_cat_free(char *str, const char *text);
static stack_t *stack_new();
static elem_type_t stack_push(stack_t *stack, elem_type_t elem_type);
static elem_type_t stack_top_type(stack_t *stack);
static char *stack_top_add_str(stack_t *stack, const char* str);
static char *stack_top_str(stack_t *stack);
static void stack_pop(stack_t *stack, elem_type_t elem_type);
static void stack_free(stack_t *stack);

void pfizer_rules_free(pfizer_rules_t *rules)
{

}
static double atof_(const char *str_)
{
    char decimal_point = localeconv()->decimal_point[0];
    if ( decimal_point == '.' )
        return atof(str_);
    char *buf,*ch;
    double res;
    buf = strdup(str_);
    ch = strchr (buf, '.');
    if ( ch )
        *ch = decimal_point;
    res = atof(buf);
    do_free(buf);
    return res;
}


/*
int xml_convert_iconv(void* data, const char* s)
{
        const size_t out_initial = 4;
        size_t inbytesleft = 1, outbytesleft = out_initial;
        char *out = do_malloc(out_initial), *outnew = out;
        size_t res = iconv((iconv_t)data, (char**)&s, &inbytesleft, &outnew, &outbytesleft);
        int ret = 0;
        if ( res == (size_t)-1 )  {
                fprintf(stderr, "error in conversion\n");
                do_free(out);
                return '?';
        }
        size_t i;
        for (i = 0; i < out_initial - outbytesleft; i++ )
                ret = (ret<<8) + (unsigned char)out[i];
        char buf[10];
        strncpy(buf,s,5);
        buf[5]='\0';
        do_log(LOG_INFO, "'%s'", buf);
        do_free(out);
        return ret;
}
void xml_convert_release(void* data)
{
        iconv_close((iconv_t)data);
}
int XMLCALL EncodingHandler1251(void *userData, const XML_Char *name, XML_Encoding *info)
{
    if ( strcmp(name,"windows-1251") == 0 )
    {
        iconv_t cd;
//        if ( (cd = iconv_open("UCS-2BE","CP1251")) == (iconv_t)-1 ) {
        if ( (cd = iconv_open("UCS-2LE","CP1251")) == (iconv_t)-1 ) {
             fprintf(stderr, "can't convert %s\n", name);
             return 0;
        }
        int i;
        for (i = 0; i < 128; ++i)
            info->map[i] = i;
        for (i = 128; i < 256; ++i)
            info->map[i] = -2;

        info->data = cd;
        info->convert = xml_convert_iconv;
        info->release = xml_convert_release;
        return XML_STATUS_OK;
    }
    return XML_STATUS_ERROR;
}
*/
char* iconv_(iconv_t conv, const char *str)
{
    if (conv == (iconv_t)-1)
        return do_strdup(str);
    size_t result;
    size_t len = strlen(str);
    char ansb[len];
    char* ansa = (char*) do_malloc(len * 2 + 1);
    char *ansbptr = ansb;
    char *ansaptr = ansa;
    size_t len1 = 2 * len;
    size_t length = len1;

    strncpy(ansb, str, len);
    result = iconv(conv, (char**)&ansbptr, &len, &ansaptr, &len1);
    if (result == (size_t) - 1)
    {
        do_free(ansa);
        return do_strdup(str);
    }
    else
	  ansa[length-len1] = '\0';
    return ansa;
}

static char *read_file (const char *filename)
{
    FILE *fp;
    char *text;
    char *res;
    size_t size;

    if ( (fp = fopen(filename, "rb")) == NULL )
    {
        do_log (LOG_ERR, "Error opening file (%s): %s", filename, strerror(errno));
        return NULL;
    }
    fseek(fp, 0L, SEEK_END);
    size = ftell (fp);
    if ( (text = do_malloc (size)) == NULL )
    {
        fclose (fp);
        do_log (LOG_ERR, "Not enough memory");
        return NULL;
    }
    fseek(fp, 0L, SEEK_SET);
    if ( fread(text, size, 1, fp) != 1 )
    {
        do_log(LOG_ERR, "Error read from file (%s): %s", filename, strerror(errno));
        return NULL;
    }
    fclose(fp);
    iconv_t cd;
    if ( (cd = iconv_open("UTF-8","CP1251")) == (iconv_t)-1 )
    {
        do_log(LOG_ERR, "can't convert");
        return NULL;
    }
    res = iconv_ (cd, text);
    
    return res;
}


pfizer_rules_t *pfizer_rules_new_from_file(const char *filename)
{
    int rc;
    rules_context_t context;
    char *buf;
    //size_t len;
    int error;
    do_log(LOG_INFO, "parser \"%s\"", filename);

    memset(&context, 0, sizeof(context));

   // FILE *fp;
    //if ( (fp = fopen(filename, "rb")) == NULL )  {
     //   do_log(LOG_ERR, "Error opening file (%s): %s", filename, strerror(errno));
      //  return NULL;
	//}

	context.parser = XML_ParserCreate ("UTF-8");
	//context.parser = XML_ParserCreate (NULL);
	//XML_SetUnknownEncodingHandler(context.parser, EncodingHandler1251, 0);

	context.stack = stack_new();

	if ( context.parser == NULL ) {
        pfizer_rules_free(context.rules);
		do_log(LOG_ERR, "Couldn't allocate memory for xml parser");
		return NULL;
	}
    errno = 0;
    error = FALSE;

	XML_SetUserData (context.parser, &context);
	XML_SetElementHandler (context.parser, rules_start, rules_end);
	XML_SetCharacterDataHandler (context.parser, rules_cdata);
	buf = read_file(filename);
	if ( !buf ) return NULL;

        rc = XML_Parse (context.parser, buf, strlen(buf), TRUE);
        if (rc == 0) {
            error = TRUE;
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
                //break;
            }
        }
	//!!XML_ParserFree (context.parser);
	//!!stack_free(context.stack);

//    if (errno) {
  //      do_log(LOG_ERR, "Error read file (%s): %s", filename, strerror(errno));
      //  fclose(fp);
    //    return 0;
//    }
//    fclose(fp);
    if ( !error )
        return context.rules;
    else {
        pfizer_rules_free(context.rules);
        return NULL;
    }
}
static elem_type_t get_elem_type (const char *s)
{
    int i;
    for ( i = 0; i < ELEM_UNKNOWN; i++ )
        if ( !strcmp (s, elem_name[i]) )
            return i;
	return ELEM_UNKNOWN;
}
static void rules_start (void *data, const char *el, const char **attr)
{
	rules_context_t *context = data;
	elem_type_t elem_type;

	elem_type=stack_push(context->stack, get_elem_type(el));

	if ( elem_type == ELEM_UNKNOWN ) {
        snprintf (context->s_error, sizeof (context->s_error),
                         "Bad element %s", el);
        XML_StopParser (context->parser, 0);
	    return;
	}
	if ( !context->rules &&
         ( elem_type != ELEM_BR &&
           elem_type != ELEM_STOP_CARDS )
       ) {
        snprintf (context->s_error, sizeof (context->s_error),
                         "Tree not init but element %s", el);
        XML_StopParser (context->parser, 0);
	    return;
	}

    switch (elem_type) {
        case ELEM_STOP_CARDS:
        case ELEM_BR:
            if ( context->rules ) {
                snprintf (context->s_error, sizeof (context->s_error),
                             "Element %s redefine ", el);
                XML_StopParser (context->parser, 0);
                return;
            }
            context->rules = do_malloc0(sizeof(pfizer_rules_t));
            context->rules->rules = do_list_new(TRUE);
            context->rules->type = (elem_type == ELEM_BR) ? PFIZER_TYPE_BUSINESS_RULES : PFIZER_TYPE_STOP_LIST;
            break;
        case ELEM_CARD:
        case ELEM_CARD_TYPE:
            if ( context->crnt_rule ) {
                snprintf (context->s_error, sizeof (context->s_error),
                             "Element %s not close ", el);
                XML_StopParser (context->parser, 0);
                return;
            }
            if ( !context->rules ) {
                snprintf (context->s_error, sizeof (context->s_error),
                             "Element %s not open ", elem_name[ELEM_BR]);
                XML_StopParser (context->parser, 0);
                return;
            }
            context->crnt_rule = do_malloc0(sizeof(pfizer_rule_t));
            context->crnt_rule->lines = do_list_new(TRUE);
            break;
        case ELEM_SKU_RULE:
            if ( context->crnt_line ) {
                snprintf (context->s_error, sizeof (context->s_error),
                             "Element %s not close ", el);
                XML_StopParser (context->parser, 0);
                return;
            }
            if ( !context->rules ) {
                snprintf (context->s_error, sizeof (context->s_error),
                             "Element %s not open ", elem_name[ELEM_BR]);
                XML_StopParser (context->parser, 0);
                return;
            }
            if ( !context->crnt_rule ) {
                snprintf (context->s_error, sizeof (context->s_error),
                             "Element %s not open ", elem_name[ELEM_CARD_TYPE]);
                XML_StopParser (context->parser, 0);
                return;
            }
            context->crnt_line = do_malloc0(sizeof(pfizer_line_t));
            break;
        default:
            break;
    }
}


static void rules_cdata(void *data, const char *ch, int len)
{

    rules_context_t *context = data;
    stack_t *stack = context->stack;

    char *s;
    s = do_strndup(ch, len);

    switch ( stack_top_type(stack) ) {
        case ELEM_VERSION:
            context->rules->version = str_cat_free(context->rules->version, s);
            break;
        case ELEM_CARD_TYPE_DESC:
            if ( !context->crnt_rule ) break;
            context->crnt_rule->desc = str_cat_free(context->crnt_rule->desc, s);
            break;
        case ELEM_SKU_CODE:
            if ( !context->crnt_line ) break;
            context->crnt_line->barcode = str_cat_free(context->crnt_line->barcode, s);
            break;
        case ELEM_SKU_DESC:
            if ( !context->crnt_line ) break;
            context->crnt_line->desc = str_cat_free(context->crnt_line->desc, s);
            break;
        case ELEM_SKU_QUANT:
            if ( !context->crnt_line ) break;
            context->crnt_line->quant = atoi(s);
            break;
        case ELEM_SKU_MAXQUANT:
            if ( !context->crnt_line ) break;
            context->crnt_line->maxquant = atoi(s);
            break;
        case ELEM_SKU_MAXDISCOUNT:
            //!!do_log(LOG_INFO, "max %s", s);
            if ( !context->crnt_line ) break;
            context->crnt_line->maxdiscount = atof_(s);
            break;
        case ELEM_TERRITORY_CODE:
        case ELEM_EXPORT_DATE:
        case ELEM_CARD_TYPE_CODE:
        case ELEM_CARD_NUMBER_FROM:
        case ELEM_CARD_NUMBER_TO:
        case ELEM_CARD_ONE_OFF:
        case ELEM_SKU_RULE_ID:
        case ELEM_DATE_FROM:
        case ELEM_DATE_TO:
        case ELEM_DISCOUNT:
        case ELEM_STOP_DATE:
        case ELEM_CARD_NUMBER:
        case ELEM_REACT_DATE:
        case ELEM_EXPORT_DATETIME:
            stack_top_add_str(stack, s);
            break;
        default:
            break;
    }
    do_free(s);
}

static void rules_end (void *data, const char *el)
{
    rules_context_t *context = data;
    stack_t *stack = context->stack;
    elem_type_t elem_type;
    elem_type = get_elem_type(el);
    char *s = stack_top_str(stack);

    if ( elem_type != stack_top_type(stack) ) {
        snprintf (context->s_error, sizeof (context->s_error),
                     "Element %s close before %s", el, elem_name[stack_top_type(stack)]);
        XML_StopParser (context->parser, 0);
        return;
    }
    switch (elem_type) {
        case ELEM_CARD:
        case ELEM_CARD_TYPE:
            if ( !context->crnt_rule ) {
                snprintf (context->s_error, sizeof (context->s_error),
                             "Element %s not open", el);
                XML_StopParser (context->parser, 0);
                break;
            }
            do_list_add_alloc(context->rules->rules, context->crnt_rule, sizeof(pfizer_rule_t));
            context->crnt_rule = NULL;
            break;
        case ELEM_SKU_RULE:
            if ( !context->crnt_rule ) {
                snprintf (context->s_error, sizeof (context->s_error),
                             "Element %s not open", elem_name[ELEM_CARD_TYPE]);
                XML_StopParser (context->parser, 0);
                break;
            }
            if ( !context->crnt_line ) {
                snprintf (context->s_error, sizeof (context->s_error),
                             "Element %s not open", el);
                XML_StopParser (context->parser, 0);
                break;
            }
            do_list_add_alloc(context->crnt_rule->lines, context->crnt_line, sizeof(pfizer_line_t));
            context->crnt_line = NULL;
            break;
        case ELEM_TERRITORY_CODE:
            context->rules->territory_code = atoi(s);
            break;
        case ELEM_EXPORT_DATE:
            if ( !context->rules ) break;
            if ( !mk_date(s, &context->rules->date) ) {
                snprintf (context->s_error, sizeof (context->s_error),
                             "%s not valid date ", s);
                XML_StopParser (context->parser, 0);
                break;
            }
            break;
        case ELEM_EXPORT_DATETIME:
            if ( !context->rules ) break;
            if ( !mk_date(s, &context->rules->date) ) {
                snprintf (context->s_error, sizeof (context->s_error),
                             "%s not valid date ", s);
                XML_StopParser (context->parser, 0);
                break;
            }
            break;
        case ELEM_CARD_TYPE_CODE:
            if ( !context->crnt_rule ) break;
            context->crnt_rule->id = atoi(s);
            break;
        case ELEM_CARD_NUMBER_FROM: {
            if ( !context->crnt_rule ) break;
            long long num;
            num = atoll(s)-CARD_MASK;
            context->crnt_rule->start = num;
            break;
        }
        case ELEM_CARD_NUMBER_TO: {
            if ( !context->crnt_rule ) break;
            long long num;
            num = atoll(s)-CARD_MASK;
            context->crnt_rule->end = num;
            break;
        }
        case ELEM_CARD_ONE_OFF: {
            if ( !context->crnt_rule ) break;
            int num;
            num = atoi(s);
            context->crnt_rule->one_off = num;
            break;
        }
        case ELEM_CARD_NUMBER: {
            if ( !context->crnt_rule ) break;
            long long sad;
            /*printf("card %lld\n",atoll(s));
            printf("mark %lld\n",CARD_MASK*10);
            printf("   = %lld\n",(atoll(s)-CARD_MASK*10));//!!*/
            sad = (atoll(s)-CARD_MASK*10);
            context->crnt_rule->card = ( sad - sad %10)/10;
            //printf("   = %d\n",context->crnt_rule->card);//!!
            break;
        }
        case ELEM_SKU_RULE_ID:
            if ( !context->crnt_line ) break;
            context->crnt_line->id = atoi(s);
            break;
        case ELEM_DATE_FROM:
            if ( !context->crnt_line ) break;
            if ( !mk_date(s, &context->crnt_line->time1) ) {
                snprintf (context->s_error, sizeof (context->s_error),
                             "%s not valid date ", s);
                XML_StopParser (context->parser, 0);
                break;
            }
            break;
        case ELEM_STOP_DATE:
            if ( !context->crnt_rule ) break;
            if ( !mk_date(s, &context->crnt_rule->time1) ) {
                snprintf (context->s_error, sizeof (context->s_error),
                             "%s not valid date ", s);
                XML_StopParser (context->parser, 0);
                break;
            }
            break;
        case ELEM_REACT_DATE:
            if ( !context->crnt_rule ) break;
            if ( !mk_date(s, &context->crnt_rule->time2) ) {
                snprintf (context->s_error, sizeof (context->s_error),
                             "%s not valid date ", s);
                XML_StopParser (context->parser, 0);
                break;
            }
            break;
        case ELEM_DATE_TO:
            if ( !context->crnt_line ) break;
            if ( !mk_date(s, &context->crnt_line->time2) ) {
                snprintf (context->s_error, sizeof (context->s_error),
                             "%s not valid date ", s);
                XML_StopParser (context->parser, 0);
                break;
            }
            break;
        case ELEM_DISCOUNT:
            if ( !context->crnt_line ) break;
            context->crnt_line->discount = atof_(s);
            //printf("disc %f %s\n",context->crnt_line->discount, s);
            break;
        default:
            break;
    }
    stack_pop(stack, elem_type);
}


struct _obj_tmp_t {
    elem_type_t elem_type;
    char *data_str;
};

struct _stack_t {
    do_list_t *list;
};

static char *str_cat_free(char *str, const char *text)
{
    if ( !str )
        return do_strdup(text);
    else {
        char *ch;
        ch = do_malloc(strlen(str)+strlen(text));
        sprintf(ch, "%s%s", str, text);
        do_free(str);
        return ch;
    }
}


static stack_t *stack_new()
{
    stack_t *retval;
    retval = do_malloc0(sizeof(stack_t));
    retval->list = do_list_new(TRUE);
    return retval;
}

static elem_type_t stack_push(stack_t *stack, elem_type_t elem_type)
{
    obj_tmp_t *obj;
    obj = do_malloc0(sizeof(obj_tmp_t));
    obj->elem_type = elem_type;
    obj->data_str = NULL;
    do_list_add(stack->list, obj);
    return elem_type;
}
static elem_type_t stack_top_type(stack_t *stack)
{
    if ( stack->list->count )
        return ((obj_tmp_t*)stack->list->list[stack->list->count-1])->elem_type;

    return ELEM_UNKNOWN;
}
static char *stack_top_add_str(stack_t *stack, const char* str)
{
    obj_tmp_t *obj;
    if ( !stack->list->count )
        return NULL;
    obj = stack->list->list[stack->list->count-1];

    obj->data_str = str_cat_free(obj->data_str, str);
    return obj->data_str;
}
static char *stack_top_str(stack_t *stack)
{
    if ( stack->list->count )
        return ((obj_tmp_t*)stack->list->list[stack->list->count-1])->data_str;

    return NULL;
}
static void stack_pop(stack_t *stack, elem_type_t elem_type)
{
    int i;
    for ( i = stack->list->count-1; i >= 0; i-- ) {
        obj_tmp_t *obj;
        obj = stack->list->list[i];
        elem_type_t elem;
        elem = obj->elem_type;
        if ( obj->data_str )
            do_free(obj->data_str);
        do_list_delete(stack->list, i);
        if ( elem == elem_type )
            break;
    }
}
static void stack_free(stack_t *stack)
{
    int i;
    for ( i = 0; i < stack->list->count; i++ ) {
        obj_tmp_t *obj;
        obj = stack->list->list[i];
        if ( obj->data_str )
            do_free(obj->data_str);
    }
    do_list_free(stack->list);
    do_free(stack);
}
int pfizer_rules_add_from_file(pfizer_rules_t *rules, const char *filename)
{
    pfizer_rules_t *tuning;
    tuning = pfizer_rules_new_from_file(filename);
    if ( !tuning )
        return FALSE;
    if ( rules->type == tuning->type ) {
        int i,j;
        for ( i = 0; i < tuning->rules->count; i++ ) {
            pfizer_rule_t *rule;
            rule = tuning->rules->list[i];
            for ( j = 0; j < rules->rules->count; j++ ) {
                pfizer_rule_t *nrule;
                nrule = rules->rules->list[j];
                if ( rule->id == nrule->id ) {
                    nrule->one_off = rule->one_off;
                    int k,l;
                    for ( k = 0; k < rule->lines->count; k++ ) {
                        pfizer_line_t *line;
                        line = rule->lines->list[k];
                        for ( l = 0; l < nrule->lines->count; l++ ) {
                            pfizer_line_t *nline;
                            nline = nrule->lines->list[l];
                            if ( nline->id == line->id ) {
                                nline->quant = line->quant;
                                nline->maxquant = line->maxquant;
                                nline->maxdiscount = line->maxdiscount;
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
    pfizer_rules_free(tuning);
    return TRUE;
}
