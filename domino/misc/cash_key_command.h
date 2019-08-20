#ifndef CSV_UTIL_H_INCLUDED
#define CSV_UTIL_H_INCLUDED
#include <dolib.h>


#ifdef __cplusplus
extern "C"
{
#endif

char *cash_key_code_to_string(int code);
int cash_key_code_from_string(const char *name);
char *cash_key_command_desc(const char *command);
do_string_list_t *cash_key_commands();
int cash_key_code_max();



#ifdef __cplusplus
}
#endif

#endif // CSV_UTIL_H_INCLUDED
