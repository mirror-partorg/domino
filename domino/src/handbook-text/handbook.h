#ifndef HANDBOOK_H_INCLUDED
#define HANDBOOK_H_INCLUDED

#include <domino.h>

typedef struct _handbook_t handbook_t;

#ifdef __cplusplus
extern "C"
{
#endif

handbook_t       *handbook_new(const char *host, const char *cs, const char *user, const char *passwod);
void              handbook_free(handbook_t *h);
void              handbook_set_marked_name(handbook_t *h, domino_marked_t marked, const char *name);
do_string_list_t *handbook_get_product_list(handbook_t *h, const char *name);
do_string_list_t *handbook_get_product_info(handbook_t *h, int index, int width, int *index_top, int *clickable, char **footer);
do_string_list_t *handbook_get_continue(handbook_t *h);
char             *handbook_get_product_row(handbook_t *h, int index, const char *text, int max_text_len);
int               handbook_row_seleted(int index, int unselected, handbook_t *h);
int               handbook_mode_changed(int mode, handbook_t *h);
int               handbook_stack_pop(handbook_t *h);
int               handbook_stack_push(handbook_t *h);
int               handbook_stack_clear(handbook_t *h);


#ifdef __cplusplus
}
#endif


#endif // HANDBOOK_H_INCLUDED
