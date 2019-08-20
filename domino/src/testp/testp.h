#ifndef TESTP_H_INCLUDED
#define TESTP_H_INCLUDED
#include <dolib.h>

typedef struct _testp_t testp_t;

#ifdef __cplusplus
extern "C"
{
#endif

testp_t *testp_new(double radius);
void     testp_free(testp_t *testp);
double   testp_get_area(testp_t *testp);

#ifdef __cplusplus
}
#endif


#endif // TESTP_H_INCLUDED
