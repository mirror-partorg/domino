#include "testp.h"
#include <math.h>
#include <stdlib.h>
#include "../misc/define.h"

struct _testp_t
{
    double radius;
};

DO_EXPORT testp_t *testp_new(double radius)
{
    testp_t *testp;
    testp = (testp_t*)do_malloc(sizeof(testp_t));
    testp->radius = radius;
    return testp;
}
DO_EXPORT void testp_free(testp_t *testp)
{
    do_free(testp);
}
DO_EXPORT double testp_get_area(testp_t *testp)
{
    return testp->radius*testp->radius*M_PI;
}

