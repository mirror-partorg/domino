#include <dolib.h>
#include "../misc/define.h"
#include <errno.h>
#include <time.h>

#ifdef _WIN32

static HMODULE do_dll;

#define SURROGATE_VALUE(h,l) (((h) - 0xd800) * 0x400 + (l) - 0xdc00 + 0x10000)
#define UTF8_LENGTH(Char)              \
  ((Char) < 0x80 ? 1 :                 \
   ((Char) < 0x800 ? 2 :               \
    ((Char) < 0x10000 ? 3 :            \
     ((Char) < 0x200000 ? 4 :          \
      ((Char) < 0x4000000 ? 5 : 6)))))


static int unichar_to_utf8 (wchar_t c, char *outbuf)
{
  /* If this gets modified, also update the copy in g_string_insert_unichar() */
  unsigned int len = 0;
  int first;
  int i;

  if (c < 0x80)
    {
      first = 0;
      len = 1;
    }
  else if (c < 0x800)
    {
      first = 0xc0;
      len = 2;
    }
  else if (c < 0x10000)
    {
      first = 0xe0;
      len = 3;
    }
   else if (c < 0x200000)
    {
      first = 0xf0;
      len = 4;
    }
  else if (c < 0x4000000)
    {
      first = 0xf8;
      len = 5;
    }
  else
    {
      first = 0xfc;
     len = 6;
    }

  if (outbuf)
    {
      for (i = len - 1; i > 0; --i)
      {
         outbuf[i] = (c & 0x3f) | 0x80;
         c >>= 6;
      }
      outbuf[0] = c | first;
    }

  return len;
}


static char *utf16_to_utf8 (const wchar_t *str)
{
  const wchar_t *in;
  char *out;
  char *result = NULL;
  int n_bytes;
  wchar_t high_surrogate;

  n_bytes = 0;
  in = str;
  high_surrogate = 0;
  while (*in) {
      wchar_t c = *in;
      wchar_t wc;

      if (c >= 0xdc00 && c < 0xe000) {
          if (high_surrogate) {
              wc = SURROGATE_VALUE (high_surrogate, c);
              high_surrogate = 0;
          } else {
              do_log(LOG_ERR, "Invalid sequence in conversion input");
              goto err_out;
          }
     } else {
          if (high_surrogate) {
              do_log(LOG_ERR,"Invalid sequence in conversion input");
              goto err_out;
          }
          if (c >= 0xd800 && c < 0xdc00) {
              high_surrogate = c;
              goto next1;
          } else
             wc = c;
     }

     n_bytes += UTF8_LENGTH (wc);

next1:
      in++;
  }

  if (high_surrogate) {
      do_log(LOG_ERR, "Partial character sequence at end of input");
      goto err_out;
  }
  result = do_malloc (n_bytes + 1);

  high_surrogate = 0;
  out = result;
  in = str;
  while (out < result + n_bytes) {
      wchar_t c = *in;
      wchar_t wc;

      if (c >= 0xdc00 && c < 0xe000) {
          wc = SURROGATE_VALUE (high_surrogate, c);
          high_surrogate = 0;
      } else
          if (c >= 0xd800 && c < 0xdc00) {
             high_surrogate = c;
             goto next2;
          } else
             wc = c;

      out += unichar_to_utf8 (wc, out);

next2:
      in++;
  }
  *out = '\0';


err_out:

  return result;

}

#define _DIR_SEPARATOR '\\'

static char *do_win32_get_package_installation_directory_of_module (HMODULE hmodule)
{
    char *retval;
    char *p;
    wchar_t wc_fn[2048];

    if (!GetModuleFileNameW (hmodule, wc_fn, MAX_PATH))
        return NULL;

    retval = utf16_to_utf8 (wc_fn);

    if ((p = strrchr (retval, _DIR_SEPARATOR)) != NULL)
       *p = '\0';

    p = strrchr (retval,_DIR_SEPARATOR);
    /*if (p && (g_ascii_strcasecmp (p + 1, "bin") == 0 ||
  	g_ascii_strcasecmp (p + 1, "lib") == 0))
    *p = '\0';
    */

    return retval;
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason )
    {
        case DLL_PROCESS_ATTACH:
	    do_dll=(HMODULE) hinstDLL;
            break;

    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

DO_EXPORT const char *do_get_libdir()
{
   static char *do_libdir = NULL;
   if ( !do_libdir )
       do_libdir = do_win32_get_package_installation_directory_of_module (do_dll);
   return do_libdir;

}
#endif

DO_EXPORT int do_save_to_file(const char* filename, void *buf, size_t len)
{
    FILE *fp;
    if ((fp = fopen(filename, "wb")) == NULL)  {
        do_log(LOG_ERR, "Error opening for write file (%s): %s", filename, strerror(errno));
        return 0;
    }
    size_t n;
    if ((n = fwrite(buf, 1, len, fp)) != len ) {
        do_log(LOG_ERR, "Error writing file (%s): %s", filename, strerror(errno));
        fclose(fp);
        return 0;
    }
    fclose(fp);
    return 1;
}

DO_EXPORT char *do_read_from_file(const char* filename)
{
    FILE *fp;
    char *retval;
    if ((fp = fopen(filename, "r")) == NULL)  {
        do_log(LOG_ERR, "Error opening file (%s): %s", filename, strerror(errno));
        return 0;
    }
    fseek (fp, 0L, SEEK_END);
    long len = ftell (fp), l;
    if ((retval = do_malloc(len + 1)) == NULL && len) {
        do_log(LOG_ERR, "Could not allocate memory %d byte", len);
        return NULL;
    }
    l = fseek (fp, 0L, SEEK_SET);
    errno = 0;
    l = fread(retval, 1, len, fp);
    if (errno) {
        do_log(LOG_ERR, "Error read file (%s): %s", filename, strerror(errno));
        fclose(fp);
        do_free(retval);
        return NULL;
    }
    retval[l] = '\0';
    fclose(fp);
    return retval;
}
#ifdef _LINUX
#include <wchar.h>
#include <wctype.h>
#include <ctype.h>

DO_EXPORT char* do_strtowupper(const char* value)
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
DO_EXPORT char* do_strtowupper(const char* value)
{
    char *str;
    str = do_strdup(value);
    return AnsiUpper(str);
}
#endif
#ifdef _WIN32
#define filename "z:\\domino\\mail\flag_on_work.txt"
#else
#define filename "/work/domino/domino/mail/flag_on_work.txt"
#endif
DO_EXPORT int do_isworktime()
{
    int res;
    time_t now = time(NULL);
    struct tm tm = *localtime(&now);
    res = tm.tm_wday && tm.tm_wday < 6 && tm.tm_hour > 6 && tm.tm_hour < 19;
    do_log(LOG_INFO, "check work time wday %d hour %d %d",tm.tm_wday,tm.tm_hour, res);
    if ( !res ) {
        FILE *fp;
        if ( (fp = fopen(filename, "rt")) == NULL )  
           return FALSE;
        char line[25];
        if ( fgets(line, 24, fp) != NULL) {
            line[24] = '\0';
            struct tm tm1;
            tm1.tm_isdst = 0; // no daylight
            memset(&tm1, 0, sizeof(tm1));
            char *cp = (char*)line;
            tm1.tm_mday = atoi(cp) ;
            while (*cp && *cp != '/' && *cp != '.')  ++cp;
            if (*cp) ++cp;
            tm1.tm_mon = atoi(cp) - 1;
            while (*cp && *cp != '/' && *cp != '.')  ++cp;
            if (*cp) ++cp;
            tm1.tm_year = atoi(cp);
            if (tm1.tm_year > 1000) tm1.tm_year -= 1900;
            else tm1.tm_year += 100;
            cp++;
            while (*cp && *cp != ':')  ++cp;
            if (*cp) ++cp;
            tm1.tm_hour = atoi(cp);
            do_log(LOG_INFO, "check work time in file wday %d hour %d",tm1.tm_wday,tm1.tm_hour);
            if ( tm1.tm_mday == tm.tm_mday && tm1.tm_mon == tm.tm_mon && tm1.tm_year == tm.tm_year && (tm.tm_hour > tm1.tm_hour || tm.tm_min > tm1.tm_min || tm.tm_sec > tm1.tm_sec) && (tm.tm_hour < 19 || tm1.tm_hour > 18))
                res = TRUE;
        }
        fclose(fp);
    }
    return res; 
}
#ifdef _WIN32
#include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h>   // for nanosleep
#else
#include <unistd.h> // for usleep
#endif

DO_EXPORT void do_sleep_ms(int milliseconds) // cross-platform sleep function
{
#ifdef _WIN32
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    usleep(milliseconds * 1000);
#endif
}