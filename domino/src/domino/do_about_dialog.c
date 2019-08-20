
#include "do_about_dialog.h"
#include "do_icons.h"

static const char * LICENSE =
"Это свободное программное обеспечение; вы можете"
" распространять его и или модифицировать на условиях Универсальной"
" общественной лицензии GNU версии 2 либо любой более поздней версии."
"\n\n"
"domino распространяется в надежде, что он может быть полезен,"
" но БЕЗ КАКОГО-ЛИБО ВИДА ГАРАНТИЙ, ВЫРАЖЕННЫХ ЯВНО"
" ИЛИ ПОДРАЗУМЕВАЕМЫХ, ВКЛЮЧАЯ, НО НЕ ОГРАНИЧИВАЯСЬ"
" ПОДРАЗУМЕВАЕМЫМИ ГАРАНТИЯМИ КОММЕРЧЕСКОЙ ЦЕННОСТИ"
" И ПРИГОДНОСТИ ДЛЯ КОНКРЕТНОЙ ЦЕЛИ. Для получения"
" дополнительных сведений обратитесь к Универсальной Общественной"
" Лицензии GNU.";

void do_about_dialog(GtkWindow *win, gchar *comment, gchar *version)
{
    //const char *website_url = "http://www.glekar.ru/";
    const char *authors[] =
    {
        "Софт-Сиб Алтай (кассовый софт Артикс)",
        "Софт-Вест (софт Домино)",
        NULL
    };
    gchar *full_version;
    full_version = g_strdup_printf("%s",version);
    gtk_show_about_dialog(win,
                           "name", g_get_application_name(),
                           //"website", website_url,
                           //"website-label", website_url,
                           //"logo-icon-name", DO_STOCK_DOMINO_APPS,
                           "license", LICENSE,
                           "wrap-license", TRUE,
                           "authors", authors,
                           "translator-credits",  "translator-credits",
                           "copyright", "Права 2010-17 Ришес Юрий",
                           "comments", comment,
                           "version", full_version,
                           NULL );
    g_free(full_version);
}

