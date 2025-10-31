#include <stdlib.h>
#include <string.h>
#include <dolib.h>
#include <stdarg.h>

#include "../../misc/define.h"
#ifdef MEMORY_DEBUG

#include <errno.h>

typedef struct {
    void *ptr;
    int  total;
    int  indx;
    char *file;
    int  line;
    char *error;
} mem_node_t;

typedef struct {
    mem_node_t *list;
    int count;
    int capacity;
} mem_list_t;

static int indx = 0;
static mem_list_t *list = NULL;

#ifdef W32_THREADS
#include <windows.h>
static CRITICAL_SECTION alloc_cs;
#elif defined(POSIX_THREADS)
#include <pthread.h>
static pthread_mutex_t  alloc_mutex;
#endif


DO_EXPORT void do_initmemdebug()
{
    list = (mem_list_t*)malloc(sizeof(mem_list_t));
    list->list = NULL;
    list->count = 0;
    list->capacity = 0;
#ifdef W32_THREADS
    InitializeCriticalSection(&alloc_cs);
#elif defined(POSIX_THREADS)
    pthread_mutex_init(&alloc_mutex, NULL);
#endif
}

static void setCapacity(int newCapacity)
{
    void *ptr;
    ptr = malloc (newCapacity * sizeof(mem_node_t));
    list->list=(mem_node_t*) realloc (list->list, newCapacity * sizeof(mem_node_t));
    if (!list->list)
        do_log(LOG_ERR, "Out of memory: %db ERR=%s", newCapacity * sizeof(mem_node_t), strerror(errno));
    list->capacity = newCapacity;
}

static void grow()
{
    if (list->capacity > 64)
        setCapacity(list->capacity + (list->capacity >> 2));
    else
        if (list->capacity > 8) setCapacity(list->capacity + 16);
        else
            setCapacity(list->capacity + 4);
}

static void addlist(const char *file, int line, void* ptr, int indx)
{
    int i;
    for (i = 0; i < list->count; i++)
        if (list->list[i].ptr == ptr) {
            if (list->list[i].total > 0) {
                if (!list->list[i].error) {
                    list->list[i].error = (char*) malloc(1024);
                    sprintf(list->list[i].error, "double alloc %s %d %d", file, line, indx);
                }
            }
            else {
                list->list[i].file = strdup(file);
                list->list[i].total++;
                list->list[i].line = line;
                list->list[i].indx = indx;
            }
            return;
        }
    if (list->count == list->capacity)
        grow();
    list->list[list->count].ptr = ptr;
    list->list[list->count].file = strdup(file);
    list->list[list->count].indx = indx;
    list->list[list->count].total = 1;
    list->list[list->count].line = line;
    list->list[list->count].error = NULL;
    list->count++;
}

static void dellist(const char *file, int line, void* ptr, int indx)
{
    int i;
    for (i = 0; i < list->count; i++)
        if (list->list[i].ptr == ptr) {
            if (list->list[i].total <= 0) {
                if (!list->list[i].error) {
                    list->list[i].error = (char*) malloc(1024);
                    sprintf(list->list[i].error, "double free %s %d %d", file, line, indx);
                }
            }
            else
                list->list[i].total--;
            return;
        }
    if (list->count == list->capacity)
        grow();
    list->list[i].ptr = ptr;
    list->list[i].file = strdup(file);
    list->list[i].indx = indx;
    list->list[i].total = 0;
    list->list[i].line = line;
    list->list[i].error = (char*) malloc(1024);
    sprintf(list->list[i].error, "free but no alloc %s %d %d", file, line, indx);
}

DO_EXPORT void *do_malloc0_(const char *file, int line, size_t size)
{
    void *retval = do_malloc_(file, line, size);
    memset(retval, 0, size);
    return retval;
}
DO_EXPORT void *do_malloc_(const char *file, int line, size_t size)
{
#ifdef W32_THREADS
   EnterCriticalSection(&alloc_cs);
#elif defined(POSIX_THREADS)
   pthread_mutex_lock(&alloc_mutex);
#endif
   void *buf;

   buf = malloc(size);
   if (buf && size > 0)
      addlist(file, line, buf, indx);
   else
      buf = NULL;
   indx++;
   if (buf == NULL && size > 0)
      do_log(LOG_ERR, "Out of memory: %db ERR=%s", size, strerror(errno));
#ifdef W32_THREADS
   LeaveCriticalSection(&alloc_cs);
#elif defined(POSIX_THREADS)
   pthread_mutex_unlock(&alloc_mutex);
#endif
   return buf;
}

DO_EXPORT void *do_realloc_(const char *file, int line, void *buf, size_t size)
{
#ifdef W32_THREADS
   EnterCriticalSection(&alloc_cs);
#elif defined(POSIX_THREADS)
   pthread_mutex_lock(&alloc_mutex);
#endif
   if (buf)
      dellist(file, line, buf, indx);
   buf = realloc(buf, size);
   if (buf && size > 0)
      addlist(file, line, buf, indx);
   else
      buf = NULL;
   indx++;
   if (buf == NULL && size > 0)
      do_log(LOG_ERR, "Out of memory: %db ERR=%s", size, strerror(errno));
#ifdef W32_THREADS
   LeaveCriticalSection(&alloc_cs);
#elif defined(POSIX_THREADS)
   pthread_mutex_unlock(&alloc_mutex);
#endif
   return buf;
}


DO_EXPORT void *do_calloc_(const char *file, int line, size_t size1, size_t size2)
{
#ifdef W32_THREADS
   EnterCriticalSection(&alloc_cs);
#elif defined(POSIX_THREADS)
   pthread_mutex_lock(&alloc_mutex);
#endif
   void *buf;

   buf = calloc(size1, size2);
   if (buf && size1 > 0)
       addlist(file, line, buf, indx);
   else
       buf = NULL;
   indx++;
   if (buf == NULL && size1 > 0)
      do_log(LOG_ERR, "Out of memory: %db ERR=%s", size1, strerror(errno));
#ifdef W32_THREADS
   LeaveCriticalSection(&alloc_cs);
#elif defined(POSIX_THREADS)
   pthread_mutex_unlock(&alloc_mutex);
#endif
   return buf;
}
DO_EXPORT void do_free_(const char *file, int line, void *ptr)
{
#ifdef W32_THREADS
   EnterCriticalSection(&alloc_cs);
#elif defined(POSIX_THREADS)
   pthread_mutex_lock(&alloc_mutex);
#endif
   dellist(file, line, ptr, indx);
   indx++;
#ifdef W32_THREADS
   LeaveCriticalSection(&alloc_cs);
#elif defined(POSIX_THREADS)
   pthread_mutex_unlock(&alloc_mutex);
#endif
    free(ptr);
}

DO_EXPORT char *do_strdup_(const char *file, int line, const char* str)
{
    char *res = (char*) do_malloc_(file, line, strlen(str) + 1);
    strcpy(res, str);
    return res;
}

DO_EXPORT void do_showmemdebug()
{
    do_log(LOG_DEBUG, "START MEMORY ERROR!!!!!!!!!!!");
    int i;
    for (i = 0; i < list->count; i++)  {
        if (list->list[i].total > 0)
            do_log(LOG_DEBUG, "Not free %s %d %d %d:%d", list->list[i].file, list->list[i].line, list->list[i].total, i, list->list[i].indx);
        if (list->list[i].error)
            do_log(LOG_DEBUG, "Error %s %s %d %d:%d", list->list[i].error, list->list[i].file, list->list[i].line, i, list->list[i].indx);
    }
    do_log(LOG_DEBUG, "END MEMORY ERROR!!!!!!!!!!!");
}
#else
#ifdef _WIN32
DO_EXPORT char *do_strndup_(const char* str, size_t size)
{
    char *res = (char*) do_malloc(size + 1);
    memcpy(res, str, size);
    res[size] = '\0';
    return res;
}
#endif
/*
static char *do_strdup_vprintf(const char *msgfmt, va_list args)
{
   //char c[2];
   va_list args2;
   int len;
   char *res;
   va_copy(args2, args);
   res = malloc(10);
   len = vsnprintf(res, 1, msgfmt, args2) + 1;
   res = do_malloc(len);
   va_copy(args2, args);
   vsnprintf(res, len, msgfmt, args2);
   res[len] = '\0';
   //va_end(args2);
   return res;

}

DO_EXPORT char *do_strdup_printf(const char *msgfmt, ...)
{

   char *buffer;
   va_list args;

   va_start (args, msgfmt);
   buffer = do_strdup_vprintf (msgfmt, args);
   va_end (args);
   return buffer;
}
*/

static int vasprintf_(char **ret, const char *format, va_list ap)
{
   va_list ap2;
   int len= 2;
   if ((*ret= do_malloc(len)) == NULL) return -1;
   while (1) {
       int nchar;
       va_copy(ap2,
                ap);
       nchar= vsnprintf(*ret, len, format, ap2);
       if ( nchar > -1 && nchar < len ) return nchar;
       if ( nchar > len )
           len= nchar+1;
       else
           len*= 2;
       if ((*ret= do_realloc(*ret, len)) == NULL )  {
		free(*ret);
		return -1;
  }
      }
  }

DO_EXPORT char *do_strdup_printf(const char *format, ...)
{
   char *ret;
   va_list ap;
   va_start (ap, format);
   vasprintf_(&ret, format, ap);
   va_end(ap);
   return ret;
}

#ifdef _WIN32
#define DEFAULT_PATH_SEPARATOR "\\"
#else
#define DEFAULT_PATH_SEPARATOR "/"
#endif
static char *build_path_va (const char *separator, const char *first_element, va_list *args, char **str_array)
{
   do_string_list_t *result;
   int separator_len = strlen (separator);
   int is_first = TRUE;
   int have_leading = FALSE;
   const char *single_element = NULL;
   const char *next_element;
   const char *last_trailing = NULL;
   int i = 0;

   result = do_string_list_new();

   if ( str_array )
       next_element = str_array[i++];
   else
       next_element = first_element;

   while ( TRUE ) {
      const char *element;
      const char *start;
      const char *end;

      if ( next_element ) {
          element = next_element;
          if ( str_array )
              next_element = str_array[i++];
          else
              next_element = va_arg (*args, char *);
      } else
          break;

      /* Ignore empty elements */
      if ( !*element )
          continue;
      start = element;

      if ( separator_len ) {
          while ( strncmp(start, separator, separator_len) == 0 )
              start += separator_len;
      }

      end = start + strlen (start);

      if ( separator_len ) {
          while ( end >= start + separator_len &&
                  strncmp (end - separator_len, separator, separator_len) == 0 )
              end -= separator_len;
          last_trailing = end;
          while ( last_trailing >= element + separator_len &&
                  strncmp (last_trailing - separator_len, separator, separator_len) == 0 )
              last_trailing -= separator_len;

          if ( !have_leading ) {
              if ( last_trailing <= start )
                  single_element = element;
              do_string_list_addn_alloc(result, element, start - element);
              have_leading = TRUE;
          } else
              single_element = NULL;
      }

      if ( end == start )
          continue;

      if ( !is_first )
          do_string_list_add_alloc(result, separator);

      do_string_list_addn_alloc(result, start, end - start);
      is_first = FALSE;
   }

   if ( single_element ) {
       do_string_list_free (result);
       return do_strdup(single_element);
   } else {
       char *res;
       if ( last_trailing )
           do_string_list_add_alloc(result, last_trailing);
       res = do_string_list_to_string(result);
       do_string_list_free (result);
       return res;
   }
}


DO_EXPORT char *do_strdup_path_join(const char *separator, const char *first_element, ...)
{
  char *str;
  const char *s;
  va_list args;

  if ( separator )
      s = separator;
  else
      s = DEFAULT_PATH_SEPARATOR;

  va_start (args, first_element);
  str = build_path_va (s, first_element, &args, NULL);
  va_end (args);

  return str;
}

#endif
