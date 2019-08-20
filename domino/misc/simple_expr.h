#ifndef _SIMPLE_EXPR_H_
#define _SIMPLE_EXPR_H_

#ifdef __cplusplus
extern "C"
{
#endif
double simple_expr_calc(const char *expression, int *error);
double simple_expr_tof(const char *expression);
#ifdef __cplusplus
}
#endif

#endif // _SIMPLE_EXPR_H_
