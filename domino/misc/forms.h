#ifndef FORMS_H_INCLUDED
#define FORMS_H_INCLUDED

#include "template.h"
#include <domino.h>

#ifdef __cplusplus
extern "C"
{
#endif

do_template_t  *do_form_torg12(do_alias_t *alias, document_key0_t *document_key0);
do_template_t  *do_form_sf(do_alias_t *alias, document_key0_t *document_key0);
do_template_t  *do_form_protocol(do_alias_t *alias, document_key0_t *document_key0, int *found);
do_template_t  *do_form_invoice(do_alias_t *alias, document_key0_t *document_key0, int number);

#ifdef __cplusplus
}
#endif



#endif // FORMS_H_INCLUDED
