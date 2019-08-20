#include <domino.h>
#include "../misc/define.h"
#include "../misc/iconv_.h"

static char *name_100[9] =
{"сто", "двести", "триста", "четыреста", "пятьсот",
"шестьсот", "семьсот", "восемьсот", "девятьсот"};
static char *name_11[10] =
{"десять", "одиннадцать", "двенадцать", "тринадцать", "четырнадцать",
"пятнадцать", "шестнадцать", "семнадцать", "восемнадцать", "девятнадцать"};
static char *name_10[8] =
{"двадцать", "тридцать", "сорок", "пятьдесят",
"шестьдесят", "семьдясят", "восемьдясят", "девяносто"};
static char *name_1[9] = {"один", "два", "три", "четыре", "пять", "шесть","семь", "восемь", "девять"};
static char *name_1j[9] = {"одна", "две", "три", "четыре", "пять", "шесть","семь", "восемь", "девять"};
static char *ind[5] = {"рубл", " тысяч", " миллион", " миллиард", " триллион"};
static char *suf[5][3] = {{"ь","я", "ей"},
                           {"а", "и", ""},
                           {"", "а", "ов"},
                           {"", "а", "ов"},
                           {"", "а", "ов"}};
static char *kopeck[] = {"копейка", "копейки", "копеек"};

static char *triada(const char *in, int tn, int abr)
{
    if (!strncmp(in, "   ", 3) ||
        !strncmp(in, "000", 3) ||
        !strncmp(in, "  0", 3) )
        return NULL;

    do_string_list_t *string = do_string_list_new();

    if (in[0] != ' ' && in[0] != '0')
        do_string_list_add_alloc(string, name_100[in[0] - '1']);
    if (in[1] == '1')
        do_string_list_addf_alloc(string, " %s", name_11[in[2] - '0']);
    else {
        if (in[1] != ' ' && in[1] != '0')
            do_string_list_addf_alloc(string, " %s", name_10[in[1] - '2']);
        if (in[2] != '0') {
            if (tn == 1)
                do_string_list_addf_alloc(string, " %s", name_1j[in[2] - '1']);
            else
                do_string_list_addf_alloc(string, " %s", name_1[in[2] - '1']);
        }
    }
    if (tn || abr) {
        do_string_list_addf_alloc(string, " %s", ind[tn]);
        switch (in[2]) {
            case '1':
                do_string_list_add_alloc(string, suf[tn][0]);
                break;
            case '2':
            case '3':
            case '4':
                do_string_list_add_alloc(string, suf[tn][1]);
                break;
            default :
                do_string_list_add_alloc(string, suf[tn][2]);
        }
    }
    char *val;
    val = do_string_list_to_string(string);
    do_string_list_free(string);
    return val;
}
DO_EXPORT char *do_real_in_words(do_alias_t *alias, double value, int mantissa, int abr)
{
    char buf[50], *ch, *val;
    int i;
    do_string_list_t *string = do_string_list_new();
    sprintf(buf, "%18.2f", value);
    if (!mantissa)
        buf[strlen(buf)-2] = '\0';

    for (i = 4, ch = buf; i >= 0; i--, ch+=3) {
        val = triada(ch, i, abr);
        if (val) {
            if ( string->count )
                do_string_list_add_alloc(string, " ");
            do_string_list_add_alloc(string, val);
        }
    }
    ch++;
    do_string_list_addf_alloc(string, " %s ", ch);
    if (mantissa) {
        switch (ch[1]) {
            case '1':
                do_string_list_add_alloc(string, kopeck[(ch[0] == '1') ? 2 : 0]);
                break;
            case '2':
            case '3':
            case '4':
                do_string_list_add_alloc(string, kopeck[(ch[0] == '1') ? 2 : 1]);
                break;
            default:
                do_string_list_add_alloc(string, kopeck[2]);
        }
    }
    val = do_string_list_to_string(string);
    do_string_list_free(string);
    if ( !do_alias_utf8(alias) ) {
        const char *str;
        char *retval;
        iconv_t conv;
        str = get_locale_lang();
        conv = iconv_open(str, UTF_CHARSET);
        if (conv == (iconv_t)-1) {
            do_log(LOG_ERR, "Error iconv_open (\"%s\" to \"%s\") ", str, UTF_CHARSET);
            return val;
        }
        retval = iconv_(conv, val);
        do_free(val);
        iconv_close(conv);
        val = retval;
    }
    return val;
}
static char *month_name[12] =
{"января","февраля","марта","апреля","мая","июня","июля","августа","сентября","октября","ноября","декабря"
};

DO_EXPORT char *do_date_in_words(do_alias_t *alias, struct tm *tm)
{
    char *retval;
    retval = do_malloc(100);
    if (tm->tm_mon < 0 || tm->tm_mon > 11) {
       sprintf(retval, "%d %d %d", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900);
       return retval;
    }

    sprintf(retval, "%d %s %d", tm->tm_mday, month_name[tm->tm_mon], tm->tm_year + 1900);
    if ( !do_alias_utf8(alias) ) {
        const char *str;
        char *val;
        iconv_t conv;
        str = get_locale_lang();
        conv = iconv_open(str, UTF_CHARSET);
        if (conv == (iconv_t)-1) {
            do_log(LOG_ERR, "Error iconv_open (\"%s\" to \"%s\") ", str, UTF_CHARSET);
            return retval;
        }
        val = iconv_(conv, retval);
        do_free(retval);
        iconv_close(conv);
        retval = val;
    }
    return retval;


}

