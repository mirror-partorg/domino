#ifdef EXPAT
#include <domino.h>
#include "../misc/define.h"
#include "../config/config.h"

struct _do_meta_alias_t {
    do_list_t *alias_periods;
};

/*
DO_EXPORT int do_date_cmp(struct tm *s1, struct tm *s2)
{
    int res;
    res = s1->tm_year - s2->tm_year;
    if ( res )
        return res;
    res = s1->tm_mon - s2->tm_mon;
    if ( res )
        return res;
    res = s1->tm_mday - s2->tm_mday;
    return res;
}
*/

static int cmp_period(do_alias_period_t *s1, do_alias_period_t *s2)
{
    return do_date_cmp(&s1->date_b, &s2->date_b);
}

DO_EXPORT do_meta_alias_t *domino_meta_alias_new()
{
    do_meta_alias_t *meta;
    do_config_t *config;
    char *filename;
    const do_string_list_t *aliases;
    int i, res, j;
    const char *value;
    char *path;
    do_alias_period_t item;
    do_alias_period_t *node;
    struct tm date_e, date_b;

    do_alias_t *alias;

    config = do_config_new(NULL, "meta_alias");
    filename = DOMINO_CONFIG(DOMINO_META_ALIAS_CONFIG_FILE);
    if ( !do_config_read(config, filename, FALSE) ) {
        do_free(filename);
        do_config_free(config);
        return NULL;
    }
    do_free(filename);

    meta = do_malloc0(sizeof(do_meta_alias_t));
    meta->alias_periods = do_list_new(FALSE);
    res = TRUE;
    aliases = do_config_get_properties(config, "Aliases");
    if ( !aliases ) {
        do_log(LOG_ERR, "Undefine aliases");
        res = FALSE;
    }
    for ( i = 0; res && i < aliases->count; i+=2 ) {

        path = aliases->list[i];
        value = do_config_get_property(config, path, "date_b");
        if ( !value || !do_atodate(value, &date_b) )
            do_date(0,&date_b);
        value = do_config_get_property(config, path, "date_e");
        if ( !value || !do_atodate(value, &date_e) )
            do_date(INT32_MAX-3, &date_e);

        value = do_config_get_property(config, path, "alias");
        if ( !value ) {
            do_log(LOG_ERR, "Undefine alias name in \"%s\"", path);
            res = FALSE;
            break;
        }

        if ( do_date_cmp(&date_b, &date_e) >=0 ) {
            do_log(LOG_ERR, "Alias config error: date begin great date end alias %s", value);
            res = FALSE;
            break;
        }


        for ( j = 0; j < meta->alias_periods->count; j++ ) {
            node = meta->alias_periods->list[j];
            if ( !strcmp(value, do_alias_get_name(node->alias)) ) {
                do_log(LOG_ERR, "Alias config error: dublicat name %s", value);
                res = FALSE;
                break;
            }
        }
        if ( !res ) break;


        alias = domino_alias_new(value);

        if ( !alias ) {
            res = FALSE;
            break;
        }
        if ( !do_alias_open(alias, FALSE, DO_DB_SKLAD, DO_DB_END) ) {
            res = FALSE;
            break;
        }

        int status;
        sklad_key0_t sklad_key0;
        sklad_rec_t sklad;
        do_text_set_empty(sklad_key0.code);
        do_string_list_t *units;

        status = do_sklad_get0(alias, &sklad ,&sklad_key0, DO_GET_GE);
        units = do_string_list_new();

        while ( status == DO_OK ) {
            char *type_;
            char *kind;
            type_ = do_text(alias, sklad.data.type_);
            kind = do_sklad_param(alias, &sklad, do_param(DO_PARAM_SKLAD_PARAMETR_KIND));
            if ( !strcmp(type_, do_param(DO_PARAM_SKLAD_RETAIL_TYPE)) && !strcmp(kind, do_param(DO_PARAM_SKLAD_RETAIL_KIND)) ) {
                do_string_list_add(units, do_text(alias, sklad.data.code));
            }
            status = do_sklad_get0(alias, &sklad ,&sklad_key0, DO_GET_NEXT);
        }
        do_alias_close(alias);
        if ( status == DO_ERROR ) {
            res = FALSE;
            do_string_list_free(units);
            break;
        }
        for ( j = 0; j < meta->alias_periods->count; j++ ) {
            node = meta->alias_periods->list[j];
            if (
                 do_date_cmp(&date_b, &node->date_e) <= 0 &&
                 do_date_cmp(&date_e, &node->date_b) >= 0  ) {
                int k,p,d;
                d = FALSE;
                for ( k = 0; k < units->count; k++ )
                    for ( p = 0; p < node->sklads->count; p++ )
                        if ( !strcmp(units->list[k], node->sklads->list[p]) ) {
                            d = TRUE;
                            break;
                        }
                if ( d ) {
                    do_log(LOG_ERR, "Alias config error: cros alias %s %s", do_alias_get_name(alias), do_alias_get_name(node->alias));
                    res = FALSE;
                    break;
                }
            }
        }

        item.alias = alias;
        item.date_b = date_b;
        item.date_e = date_e;
        item.sklads = units;
        do_list_add_alloc(meta->alias_periods, &item, sizeof(item));
    }
    do_config_free(config);
    if ( !res ) {
        domino_meta_alias_free(meta);
        return NULL;
    }
    return meta;
}
DO_EXPORT do_sort_list_t *domino_meta_alias_get_list_from_sklad(do_meta_alias_t *meta, const char *sklad, struct tm *date_b, struct tm *date_e)
{
    do_sort_list_t *list;
    int j;

    list = do_sort_list_new(TRUE, FALSE, (do_list_cmp_func)cmp_period, NULL);

    for ( j = 0; j < meta->alias_periods->count; j++ ) {
        int i;
        do_alias_period_t *node;
        node = meta->alias_periods->list[j];
        for ( i = 0; i < node->sklads->count; i++ ) {
            if ( !strcmp(sklad, node->sklads->list[i]) ) {
                if ( do_date_cmp(date_b, &node->date_e ) <= 0 &&
                     do_date_cmp(date_e, &node->date_b ) >= 0 ) {
                    do_alias_period_t period;
                    memcpy(&period, node, sizeof(period));
                    if ( do_date_cmp(date_b, &period.date_b ) > 0 )
                        period.date_b = *date_b;
                    if ( do_date_cmp(date_e, &period.date_e ) < 0 )
                        period.date_e = *date_e;
                    do_sort_list_add_alloc(list, &period, sizeof(period));
                }
                break;
            }
        }
    }
    return list;
}

DO_EXPORT do_list_t *domino_meta_alias_get_list_from_sklads(do_meta_alias_t *meta, do_string_list_t *sklads, struct tm *date_b, struct tm *date_e)
{
    do_sort_list_t *list;
    int j;

    list = do_sort_list_new(TRUE, FALSE, (do_list_cmp_func)cmp_period, NULL);

    for ( j = 0; j < meta->alias_periods->count; j++ ) {
        int i;
        do_alias_period_t *node;
        node = meta->alias_periods->list[j];
        for ( i = 0; i < node->sklads->count; i++ ) {
            int k,h;
            h = FALSE;
            for ( k = 0; k < sklads->count; k++ )
                if ( !strcmp(sklads->list[k], node->sklads->list[i]) ) {
                    if ( do_date_cmp(date_b, &node->date_e ) <= 0 &&
                         do_date_cmp(date_e, &node->date_b ) >= 0 ) {
                        do_alias_period_t period;
                        memcpy(&period, node, sizeof(period));
                        if ( do_date_cmp(date_b, &period.date_b ) > 0 )
                            period.date_b = *date_b;
                        if ( do_date_cmp(date_e, &period.date_e ) < 0 )
                            period.date_e = *date_e;
                        do_sort_list_add_alloc(list, &period, sizeof(period));
                    }
                    break;
                    h = TRUE;
            }
            if ( h ) break;
        }
    }
    return list;
}
DO_EXPORT void domino_meta_alias_free(do_meta_alias_t *meta)
{
    int i;
    for ( i = 0; i < meta->alias_periods->count; i++ ) {
        do_alias_period_t *node;
        node = meta->alias_periods->list[i];
        do_alias_free(node->alias);
        do_string_list_free(node->sklads);
    }
    do_list_free(meta->alias_periods);
    do_free(meta);
}
#endif
