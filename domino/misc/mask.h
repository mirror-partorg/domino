#ifndef OPT_H_INCLUDED
#define OPT_H_INCLUDED

#define MAX_VARIABLE 10

typedef struct {
    char  *name[MAX_VARIABLE + 1];
    char   value[MAX_VARIABLE + 1][256];
    char   format[256];
    int    total;
    int    uppercase;
} mask_t;

#ifdef __cplusplus
extern "C"
{
#endif
mask_t  *mask_new(const char* mask, int uppercase);
void     mask_free(mask_t *mask);
int      mask_valid(mask_t *mask, const char* name);
char    *mask_out(mask_t *mask, const char *format, int total_value);
#ifdef __cplusplus
}
#endif
#endif // OPT_H_INCLUDED
