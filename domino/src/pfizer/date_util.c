#include <time.h>
#include <dolib.h>
#include "date_util.h"

static const unsigned short int __mon_yday[2][13] = {
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};
#define ISLEAP(year) ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))
#define SECS_PER_HOUR (60 * 60)
static void day_of_the_week (struct tm *tm)
{
  int corr_year = 1900 + tm->tm_year - (tm->tm_mon < 2);
  int wday = (-473
      + (365 * (tm->tm_year - 70))
      + (corr_year / 4)
      - ((corr_year / 4) / 25) + ((corr_year / 4) % 25 < 0)
     + (((corr_year / 4) / 25) / 4)
     + __mon_yday[0][tm->tm_mon]
     + tm->tm_mday - 1);
  tm->tm_wday = ((wday % 7) + 7) % 7;
}

static void day_of_the_year (struct tm *tm)
{
    tm->tm_yday = (__mon_yday[ISLEAP(1900 + tm->tm_year)][tm->tm_mon] + (tm->tm_mday - 1));
}

int mk_date(const char *s, struct tm *tm)
{
    tm->tm_isdst = 0; // no daylight
    memset(tm, 0, sizeof(*tm));
    char buf[10];

    char *cp = (char*)s;
    strncpy(buf, cp , 2);
    tm->tm_mday = atoi(buf);
    strncpy(buf, cp + 2 , 2);
    tm->tm_mon = atoi(buf) - 1;
    tm->tm_year = atoi(cp + 4) - 1900;
    //if ( tm->tm_year > 200 )
      //  tm->tm_year = 200;
    day_of_the_week(tm);
    day_of_the_year(tm);
    return (tm->tm_year > -70 && tm->tm_year < 10000) && (tm->tm_mon >=0 && tm->tm_mon <12) &&
              (tm->tm_mday > 0 && tm->tm_mday < 32);
}
