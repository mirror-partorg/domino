#ifndef UNIL_H_INCLUDED
#define UNIL_H_INCLUDED
#include <domino.h>

#ifdef __cplusplus
extern "C"
{
#endif

int util_clean_realization(do_alias_t *alias, const char *sklad, int date_start, int date_end, do_extended_break_func break_func);
int util_make_realization(do_alias_t *alias, const char *sklad, int date_start, int date_end, do_extended_break_func break_func);

#ifdef __cplusplus
}
#endif


#endif // UNIL_H_INCLUDED
