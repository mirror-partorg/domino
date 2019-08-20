#ifndef READ_FILES_H_INCLUDED
#define READ_FILES_H_INCLUDED

#include <dolib.h>

typedef enum {
    PFIZER_TYPE_BUSINESS_RULES,
    PFIZER_TYPE_STOP_LIST,
} pfizer_type_t;

typedef struct {
    pfizer_type_t type;
    int territory_code;
    char *version;
    struct tm date;
    do_list_t *rules;
} pfizer_rules_t;

typedef struct {
    int   id;
    char *desc;
    int start;
    int end;
    do_list_t *lines;
    struct tm time1;
    struct tm time2;
    int card;
    // tuning
    int one_off;
} pfizer_rule_t;

typedef struct {
    int       id;
    struct tm time1;
    struct tm time2;
    char     *barcode;
    char     *desc;
    double    discount;
    // tuning
    int  quant;
    int  maxquant;
    double maxdiscount;
} pfizer_line_t;

#ifdef __cplusplus
extern "C"
{
#endif

pfizer_rules_t *pfizer_rules_new_from_file(const char *filename_xml);
int pfizer_rules_add_from_file(pfizer_rules_t *rules, const char *filename_xml);
do_list_t *pfizer_read_blacklist(const char*filename);

void pfizer_rules_free(pfizer_rules_t *rules);

#ifdef __cplusplus
}
#endif

#endif // READ_FILES_H_INCLUDED
