#ifndef TEMPLATE_H_INCLUDED
#define TEMPLATE_H_INCLUDED

typedef struct _do_template_t do_template_t;
typedef struct _do_template_cursor_t do_template_cursor_t;

#ifdef __cplusplus
extern "C"
{
#endif

do_template_t        *do_template_new(const char *filename, int use_utf);
void                  do_template_free(do_template_t *templ);

void                  do_template_clear(do_template_t *templ);
void                  do_template_set(do_template_t *templ, ...);
void                  do_template_default_value_enable(do_template_t *templ, int value);

do_template_cursor_t *do_template_cursor(do_template_t *templ);
do_template_cursor_t *do_template_section(do_template_t *templ, const char *name);

void                  do_template_cursor_first(do_template_cursor_t *cursor);
void                  do_template_cursor_first_run(do_template_cursor_t *cursor);
void                  do_template_cursor_run(do_template_cursor_t *cursor);
int                   do_template_cursor_run_step(do_template_cursor_t *cursor);
int                   do_template_cursor_run_step_for(do_template_cursor_t *cursor, const char *name);
void                  do_template_cursor_skip(do_template_cursor_t *cursor);
int                   do_template_cursor_skip_for(do_template_cursor_t *cursor, const char *name);

char                 *do_template_text(do_template_t *templ);
int                   do_template_save_to_file(do_template_t *templ, const char *filename);
void                  do_template_add_page_break(do_template_t *templ);

#ifdef __cplusplus
}
#endif



#endif // TEMPLATE_H_INCLUDED
