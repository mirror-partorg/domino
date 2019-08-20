#include <domino.h>
#include <domino_action.h>

int main()
{
	do_alias_t *alias = domino_alias_new("local.tta");
	do_alias_open(alias, TRUE);
    struct tm tm1;
    struct tm tm2;
    do_list_t *list;
    char *store = "5";
    char *code = "40485";
    do_log_set_stdout();

    do_date_set_ymd(&tm1, 2019, 1, 7);
    do_date_set_ymd(&tm2, 2019, 1, 14);
    
    //do_log(LOG_INFO, "%d", domino_order_after_traffic_in(alias, store, code, &tm1));
    
    //return 1;

    list = domino_order_get_marked_list(alias, store, &tm1, &tm2, TRUE);
    
    int i;
    
    for ( i = 0; i < list->count; i++ ) {
        domino_order_marked_list_item_t *item;
    	item = list->list[i];
   		//do_log(LOG_INFO, "%s %d", item->code, item->marked );
    	if ( !strcmp(item->code, "40485") ) {
    		do_log(LOG_INFO, "%d %d %s %s", item->marked,	domino_order_after_traffic_in(alias, store, item->code, &item->tm), do_datetoa(item->tm), do_timetoa(item->tm));
    	}
    }

	
	
}
