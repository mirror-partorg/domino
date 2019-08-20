#ifndef ARTIX_SEND_H_INCLUDED
#define ARTIX_SEND_H_INCLUDED

#include <artix.h>
#include <domino.h>
#include <dolib.h>

#define ARTIX_SEND_PRODUCT  1
#define ARTIX_SEND_CASHIER  2
#define ARTIX_SEND_KEYBOARD 4

#ifdef __cplusplus
extern "C"
{
#endif

int artix_send(artix_t *artix,
               do_alias_t *alias,
               do_proc_state_t *proc_state,
               const char *sklad,
/*!!               int cash_code,*/
               int objs,
               do_string_list_t *product_list);

#ifdef __cplusplus
}
#endif


#endif // ARTIX-CASH_H_INCLUDED
