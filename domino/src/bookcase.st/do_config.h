#ifndef DO_CONFIG_H_INCLUDED
#define DO_CONFIG_H_INCLUDED

#include <dolib.h>

typedef struct _do_config_t do_config_t;

#ifdef __cplusplus
extern "C"
{
#endif

do_config_t *do_config_new(const char *encoding, const char *name);
void do_config_free(do_config_t *config);
int   do_config_read(do_config_t *config, const char *filename, int empty);
int   do_config_write(do_config_t *config, const char *filename);
int   do_config_write_text(const char *filename, const char *text);
const char  *do_config_get_kind(do_config_t *config, const char *path);
const char  *do_config_set_kind(do_config_t *config, const char *path, const char *kind);
const char  *do_config_get_attr(do_config_t *config, const char *path, const char *attr);
int    do_config_get_attr_int(do_config_t *config, const char *path, const char *attr);
double do_config_get_attr_double(do_config_t *config, const char *path, const char *attr);
int    do_config_get_attr_bool(do_config_t *config, const char *path, const char *attr);
void   do_config_set_attr(do_config_t *config, const char *path, const char *attr, const char *value);
void   do_config_set_attr_int(do_config_t *config, const char *path, const char *attr, int value);
void   do_config_set_attr_double(do_config_t *config, const char *path, const char *attr, double value);
void   do_config_set_attr_bool(do_config_t *config, const char *path, const char *attr, int value);
const  do_string_list_t *do_config_get_properties(do_config_t *config, const char *path);
do_string_list_t *do_config_get_children(do_config_t *config, const char *path);
const char  *do_config_get_property(do_config_t *config, const char *path, const char *property);
int    do_config_get_property_int(do_config_t *config, const char *path, const char *property);
double do_config_get_property_double(do_config_t *config, const char *path, const char *property);
int    do_config_get_property_bool(do_config_t *config, const char *path, const char *property);
void   do_config_set_property(do_config_t *config, const char *path, const char *property, const char *value);
void   do_config_set_property_int(do_config_t *config, const char *path, const char *property, int value);
void   do_config_set_property_double(do_config_t *config, const char *path, const char *property, double value);
void   do_config_set_property_bool(do_config_t *config, const char *path, const char *property, int value);
void   do_config_clear_property(do_config_t *config, const char *path, const char *property);


#ifdef __cplusplus
}
#endif

#endif // DOMINO_H_INCLUDED
