#ifndef DB_H_INCLUDED
#define DB_H_INCLUDED

typedef struct _do_time_t do_time_t;
#ifdef __cplusplus
extern "C"
{
#endif


char	 *do_partner_get_region(void *data, int size);
int	  do_partner_get_code(void *data, int size);
char	 *do_partner_get_name(void *data, int size);
char	 *do_partner_get_sklad(void *data, int size);
char	 *do_partner_get_param(void *data, int size, const char *param);
char	 *do_product_get_class(void *data, int size);
char	 *do_product_get_group(void *data, int size);
char	 *do_product_get_subgroup(void *data, int size);
char	 *do_product_get_code(void *data, int size);
char	 *do_product_get_name(void *data, int size);
char	 *do_product_get_unit(void *data, int size);
char	 *do_product_get_texh_unit(void *data, int size);
char	 *do_product_get_supplier_region(void *data, int size);
int	  do_product_get_supplier_code(void *data, int size);
char	 *do_product_get_mfr_region(void *data, int size);
int	  do_product_get_mfr_code(void *data, int size);
int	  do_product_get_price(void *data, int size, int n);
int	  do_product_get_type(void *data, int size);
double	  do_product_get_coef(void *data, int size);
do_time_t *do_product_get_time(void *data, int size);
int	  do_product_get_is_parcel(void *data, int size);
do_time_t *do_product_get_lifetime(void *data, int size);
char	 *do_product_get_param(void *data, int size, const char *param);
int       do_time_year(do_time_t *t);
int       do_time_month(do_time_t *t);
int       do_time_day(do_time_t *t);
int       do_time_hour(do_time_t *t);
int       do_time_minute(do_time_t *t);
int       do_time_second(do_time_t *t);
void      do_time_free(do_time_t *t);


#ifdef __cplusplus
}
#endif

#endif 
