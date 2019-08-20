
#include <dolib.h>
#include <domino_db.h>
#include <string.h>
#include "../../misc/define.h"

typedef struct {
    int year;
    int month;
    int day;
    int wday;
    int yday;
    int hour;
    int minute;
    int second;
} do_time_t;


static char* do_fromparam(const char* src, const char* name)
{
    if (!name)
        return do_strdup("");
    char *value, *cp = (char*)src, *head;
    int len = strlen(name);
    char buf[1024];
    char *Uname,*buf1;
    Uname = do_strtowupper(name);
    while (*cp != '\0') {
        head = cp;
        while (*cp != '\0' && *cp != '\n' && *cp !='\r' && *cp != '=') cp++;
        if (*cp != '=' ) {
            while (*cp == '\n' || *cp =='\r') cp++;
            continue;
        }
        strncpy(buf, head, cp - head);
        buf[cp-head]='\0';
        buf1=do_strtowupper(buf);
        if (!strcmp(Uname, buf1)) {
            do_free(buf1);
            head = ++cp;
            while (*cp != '\0' && *cp != '\n' && *cp !='\r') cp++;
            value = (char*)do_malloc(cp - head + 1);
            strncpy(value, head, cp - head);
            value[cp - head] = '\0';
            do_free(Uname);
            return value;
        }
        do_free(buf1);
        while (*cp != '\0' && *cp != '\n' && *cp !='\r') cp++;
        while (*cp == '\n' || *cp =='\r') cp++;
    }
    do_free(Uname);
    return do_strdup("");
}

/*
static char* do_fromparam(const char* src, const char* name)
{
    if (!name)
        return do_strdup("");
    char *value, *cp = (char*)src, *head;
    int len = strlen(name);
    char buf[1024];
    char *Uname
    while (*cp != '\0') {
        head = cp;
        while (*cp != '\0' && *cp != '\n' && *cp !='\r' && *cp != '=') cp++;
        if (*cp != '=' ) {
            while (*cp == '\n' || *cp =='\r') cp++;
            continue;
        }
        if (!strncmp(name, head, cp - head) && (len == cp - head)) {
            head = ++cp;
            while (*cp != '\0' && *cp != '\n' && *cp !='\r') cp++;
            value = (char*)do_malloc(cp - head + 1);
            strncpy(value, head, cp - head);
            value[cp - head] = '\0';
            return value;
        }
        while (*cp != '\0' && *cp != '\n' && *cp !='\r') cp++;
        while (*cp == '\n' || *cp =='\r') cp++;
    }
    return do_strdup("");
}
*/
#define _text(src) _text_(src,sizeof(src))
static char* _text_(void* src, size_t size)
{
    char* buf = (char*)do_malloc(size + 1);
    buf[size] = '\0';
    strncpy(buf, (char*)src, size);
    char* cp = buf + size - 1;
    while ((cp >= buf) && (*cp == 32)) --cp;
    *(cp+1) = '\0';
    return buf;
}
DO_EXPORT char *do_partner_get_region(void *data, int size)
{
    partner_struct_t *partner=data;
    return _text(partner->g_code);
}
DO_EXPORT int do_partner_get_code(void *data, int size)
{
    int retval;
    partner_struct_t *partner=data;
    retval = partner->code;
    return retval;
}
DO_EXPORT char *do_partner_get_name(void *data, int size)
{
    partner_struct_t *partner=data;
    return _text(partner->name);
}
DO_EXPORT char *do_partner_get_sklad(void *data, int size)
{
    partner_struct_t *partner=data;
    return _text(partner->sklad);
}
DO_EXPORT char *do_partner_get_param(void *data, int size, const char *param)
{
    partner_struct_t *partner=data;
    char *params = _text_(partner->params, size - (sizeof(partner_struct_t) - sizeof(partner->params)));
    char *res = do_fromparam(params, param);
    do_free(params);
    return res;
}
DO_EXPORT char *do_product_get_class(void *data, int size)
{
    product_struct_t *product=data;
    return _text(product->class_id);
}
DO_EXPORT char	 *do_product_get_group(void *data, int size)
{
    product_struct_t *product=data;
    return _text(product->group_id);
}
DO_EXPORT char	 *do_product_get_subgroup(void *data, int size)
{
    product_struct_t *product=data;
    return _text(product->subgroup_id);
}
DO_EXPORT char	 *do_product_get_code(void *data, int size)
{
    product_struct_t *product=data;
    return _text(product->code);
}
DO_EXPORT char *do_product_get_name(void *data, int size)
{
    product_struct_t *product=data;
    return _text(product->name);
}
DO_EXPORT char *do_product_get_unit(void *data, int size)
{
    product_struct_t *product=data;
    return _text(product->unit);
}
DO_EXPORT char	 *do_product_get_texh_unit(void *data, int size)
{
    product_struct_t *product=data;
    return _text(product->tech_unit);
}
DO_EXPORT char	 *do_product_get_supplier_region(void *data, int size)
{
    product_struct_t *product=data;
    return _text(product->supplier_region);
}
DO_EXPORT int	  do_product_get_supplier_code(void *data, int size)
{
    product_struct_t *product=data;
    return product->supplier_code;
}
DO_EXPORT char	 *do_product_get_mfr_region(void *data, int size)
{
    product_struct_t *product=data;
    return _text(product->mfr_region);
}
DO_EXPORT int	  do_product_get_mfr_code(void *data, int size)
{
    product_struct_t *product=data;
    return product->mfr_code;
}
DO_EXPORT int	  do_product_get_price(void *data, int size, int n)
{
    product_struct_t *product=data;
    int i;
    for (i = 0; i < product->total_price; i++)
        if (product->nprice[i] == n)
            return product->price[i];
    return 0;
}
DO_EXPORT int	  do_product_get_type(void *data, int size)
{
    product_struct_t *product=data;
    return product->ptype;
}


static double do_pdecimal(unsigned char *value, size_t size, int fracplaces)
{
    double result;
    int i;
    result = 0;
    double dr = 1;
    for (i = 1; i < size*2; i++) {
        if (i < size*2 - fracplaces)
            result *=10;
        else
            dr *=10;
        if (i & 1)
            result += (value[i / 2] & 0x0F) / dr;
        else
            result += ((value[i / 2] & 0xF0) >> 4) / dr;
    }
    return (value[0] & 0xF0) ? -result : result;
}

DO_EXPORT double  do_product_get_coef(void *data, int size)
{
    product_struct_t *product=data;
    return do_pdecimal((unsigned char*)&product->tech_unit_rate, 4, 3);
}

# define ISLEAP(year)	((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))
static const unsigned short int __mon_yday[2][13] = {
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

static int do_date(BTI_LONG date, do_time_t *tm)
{
    int days = date - 4; //

    memset(tm, 0, sizeof(*tm));

    // January 1, 1801 was a Thursday.
    tm->wday = (4 + days) % 7; // :) 18010101 = 4
    if (tm->wday < 0)
        tm->wday += 7;
    int y = 1801;

#define DIV(a, b) ((a) / (b) - ((a) % (b) < 0))
#define LEAPS_THRU_END_OF(y) (DIV (y, 4) - DIV (y, 100) + DIV (y, 400))

    while (days < 0 || days >= (ISLEAP(y) ? 366 : 365)) {
      // Guess a corrected year, assuming 365 days per year.
      long int yg = y + days / 365 - (days % 365 < 0);
      /* Adjust DAYS and Y to match the guessed year.  */
      days -= ((yg - y) * 365
	       + LEAPS_THRU_END_OF (yg - 1)
	       - LEAPS_THRU_END_OF (y - 1));
      y = yg;
    }
    tm->year = y;
    tm->yday = days;
    const unsigned short int *ip;
    ip = __mon_yday[ISLEAP(y)];
    for (y = 11; days < (long int) ip[y]; --y);
    days -= ip[y];
    tm->month = y + 1;
    tm->day = days + 1;
    if ( !((tm->year > 1970 && tm->year < 10000) && (tm->month >0 && tm->month <=12) && (tm->day > 0 && tm->day < 32)) ) {
	do_log(LOG_ALERT, "date invalide %d/%d/%d", tm->year, tm->month, tm->day);
    }
    return (tm->year > 1970 && tm->year < 10000) && (tm->month >0 && tm->month <=12) && (tm->day > 0 && tm->day < 32);
}
#define	SECS_PER_HOUR (60 * 60)
DO_EXPORT int do_time(BTI_LONG time, do_time_t *tm)
{
    long rem = time / 100;
    tm->hour = rem / SECS_PER_HOUR;
    rem %= SECS_PER_HOUR;
    tm->minute = rem / 60;
    tm->second = rem % 60;
    if ( !((tm->hour >= 0 && tm->hour < 24) && (tm->minute >=0 && tm->minute < 60) && (tm->second >= 0 && tm->second < 60)) ) {
	do_log(LOG_ALERT, "time invalide %d/%d/%d", tm->hour, tm->minute, tm->second);
    }
    return (tm->hour >= 0 && tm->hour < 24) && (tm->minute >=0 && tm->minute < 60) && (tm->second >= 0 && tm->second < 60);
}
static void day_of_the_week (do_time_t *tm)
{
  int corr_year = tm->year - (tm->month < 3);
  int wday = (-473
	      + (365 * (tm->year - 1970))
	      + (corr_year / 4)
	      - ((corr_year / 4) / 25) + ((corr_year / 4) % 25 < 0)
	      + (((corr_year / 4) / 25) / 4)
	      + __mon_yday[0][tm->month-1]
	      + tm->day - 1);
  tm->wday = ((wday % 7) + 7) % 7;
}

static void day_of_the_year (do_time_t *tm)
{
  tm->yday = (__mon_yday[ISLEAP(tm->year)][tm->month-1]
		 + (tm->day - 1));
}
static int do_atodate(const char *str, do_time_t *tm)
{
    memset(tm, 0, sizeof(*tm));

    char *cp = (char*)str;
    tm->day = atoi(cp) ;
    while (*cp && *cp != '/' && *cp != '.')
      ++cp;
    if (*cp) ++cp;
    tm->month = atoi(cp);
    while (*cp && *cp != '/' && *cp != '.')
      ++cp;
    if (*cp) ++cp;
    tm->year = atoi(cp);
    if (tm->year < 1000)
        tm->year += 2000;

    day_of_the_week(tm);
    day_of_the_year(tm);
    if ( !((tm->year > 1970 && tm->year < 10000) && (tm->month >0 && tm->month <=12) && (tm->day > 0 && tm->day < 32)) ) {
	do_log(LOG_ALERT, "date invalide %d/%d/%d from %s", tm->year, tm->month, tm->day, str);
    }
    return (tm->year > 1970 && tm->year < 10000) && (tm->month > 0 && tm->month <=12) && (tm->day > 0 && tm->day < 32);
}

DO_EXPORT do_time_t *do_product_get_time(void *data, int size)
{
    product_struct_t *product=data;
    do_time_t *t = do_malloc(sizeof(do_time_t));
    if ( !do_date(product->date, t) || !do_time(product->time, t) ) {
	do_free(t);
	return NULL;
    }
    return t;
}
DO_EXPORT int	do_product_get_is_parcel(void *data, int size)
{
    product_struct_t *product=data;
    return product->base_parcel;
}

DO_EXPORT do_time_t *do_product_get_lifetime(void *data, int size, int *year, int *month, int *day, int *hour, int *minute,int *second)
{
    product_struct_t *product=data;
    do_time_t *t=do_malloc(sizeof(do_time_t));
    int retval = 0;
    
    if (product->life_time) 
        retval = do_date(product->life_time, t);
    else {
        char *params = _text_(product->params, size - (sizeof(product_struct_t) - sizeof(product->params)));
	char *life_time = do_fromparam(params, "Life_Time");
	if (*life_time != '\0') {
	    retval = do_atodate(life_time, t);
	}
	do_free(life_time);
    }
    if ( !retval ) {
	do_free(t);
	return NULL;
    }
    return t;
}
DO_EXPORT char	 *do_product_get_param(void *data, int size, const char *param)
{
    product_struct_t *product=data;
    char *params = _text_(product->params, size - (sizeof(product_struct_t) - sizeof(product->params)));
    char *retval = do_fromparam(params, param);
    return retval;
}
DO_EXPORT int do_time_year(do_time_t *t)
{
    if ( t == NULL ) return 2012;
    return t->year;
}
DO_EXPORT int do_time_month(do_time_t *t)
{
    return t->month;
}
DO_EXPORT int do_time_day(do_time_t *t)
{
    return t->day;
}
DO_EXPORT int do_time_hour(do_time_t *t)
{
    return t->hour;
}
DO_EXPORT int do_time_minute(do_time_t *t)
{
    return t->minute;
}
DO_EXPORT int do_time_second(do_time_t *t)
{
    return t->second;
}
DO_EXPORT void do_time_free(do_time_t *t)
{
    do_free(t);
}

