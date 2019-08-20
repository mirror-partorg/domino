#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "cash_key_command.h"
#include "define.h"


typedef struct {
    char *command;
    char *desc;
}com_t;
#define TOTAL_COMMAND 38

static const com_t commands[TOTAL_COMMAND] = {
    {"COMMAND_BCODETEST","Проверка товара по штрихкоду"},
    {"COMMAND_QUANT","Ввод количества"},
    {"COMMAND_PRICE","Ввод цены"},
    {"COMMAND_DEPT","Ввод отдела"},
    {"COMMAND_RESET","Сброс ввода"},
    {"COMMAND_TOTAL","Итог"},
    {"COMMAND_SUBTOTAL","Подитог"},
    {"COMMAND_EXIT","Выход"},
    {"COMMAND_LOGIN","Регистрация кассира"},
    {"COMMAND_STORNOLAST","Сторно последней позиции в чеке"},
    {"COMMAND_STORNOALL","Сторно всех позиций в чеке"},
    {"COMMAND_STORNOSELECT","Сторно выбранной позиции в чеке"},
    {"COMMAND_SALE","Переход в режим продажи"},
    {"COMMAND_BACK","Переход в режим возврата"},
    {"COMMAND_OPENDRAWER","Открыть денежный ящик"},
    {"COMMAND_PRINTCOPY","Печать копии чека"},
    {"COMMAND_SELECTTMC","Выбор товара из справочника по ШК"},
    {"COMMAND_DISCOUNT","Запрос ввода карты"},
    {"COMMAND_VALSELECT","Выбор валюты"},
    {"COMMAND_CALC","Калькулятор"},
    {"COMMAND_UNDOCARD","Отмена карты"},
    {"COMMAND_CHECK","Чек (мягкий, плат. системы)"},
    {"COMMAND_PACKING","Расфасовка"},
    {"COMMAND_SELECTPRICE","Выбор цены из списка"},
    {"COMMAND_PRINTGOODSCH","Товарный чек"},
    {"COMMAND_ONEBACK","Возврат в чеке продажи"},
    {"COMMAND_INDEX","Индекс цены"},
    {"COMMAND_APPLY","Применить модификаторы к текущей выделенной позиции"},
    {"COMMAND_DEPTN","Отдел"},
    {"COMMAND_CARDINFO","Вывод информации по карте"},
    {"COMMAND_FIXEDDISC","Фиксированная скидка по кнопке"},
    {"COMMAND_CODELIST","Выбор из списка кодов"},
    {"COMMAND_ВCODELIST","Выбор из списка штрихкодов"},
    {"COMMAND_REPEAT","Повтор позиции"},
    {"COMMAND_BACKBYSALE","Возврат по чеку продажи"},
    {"COMMAND_VALUTDETECTOR","Детектор валют"},
    {"COMMAND_TESTBYNAME","Проверка по имени"},
    {"COMMAND_BCODE","Штрих-Код"}};

DO_EXPORT char *cash_key_command_desc(const char *command)
{
    int i;
    for (i = 0; i < TOTAL_COMMAND; i++)
        if (!strcmp(command, commands[i].command))
            return do_strdup(commands[i].desc);
    return do_strdup(command);
}
DO_EXPORT do_string_list_t *cash_key_commands()
{
    do_string_list_t *list = do_string_list_new();
    int i;
    for (i = 0; i < TOTAL_COMMAND; i++)
        do_string_list_add_alloc(list, commands[i].command);
    return list;
}
#define MAX_KEY_CODE 300
#define MIN_KEY_CODE 32

DO_EXPORT char *cash_key_code_to_string(int code)
{
    char *res = (char*)do_malloc(15);
    res[0] = '\0';

    if (code >=MIN_KEY_CODE && code < 127) {
        res[0] = code;
        res[1] = '\0';
    }
    else
    if (code > 264 && code < 277) {
        sprintf(res, "F%d", code - 264);
    }
    else
    if (code > 276 && code < 289) {
        sprintf(res, "Shift+F%d", code - 276);
    }
    else
    if (code > 288 && code <= 301) {
        sprintf(res, "Ctrl+F%d", code - 288);
    }
    return res;
}
#ifdef _LINUX
#include <wchar.h>
#include <wctype.h>
static  char* atowupper(char* str)
{
    if( MB_CUR_MAX > 1 )
    {
        int i,len = strlen(str);
        wchar_t wc;
        mbstate_t cur_state;
        memset(&cur_state, 0, sizeof(mbstate_t));
        for(i = 0; i <= len;)
        {
            size_t mbclen = mbrtowc(&wc, str + i, len - i, &cur_state);
            if (mbclen == (size_t) -1 || mbclen == (size_t) -2 || mbclen == 0 )
               mbclen = 1;
            else
            {
                if (iswlower((wint_t)wc))
                {
                    wc = towupper((wint_t)wc);
                    if (wcrtomb(str + i, wc, &cur_state) == -1) {
                	return str;
                    }
                }
            }
            i += mbclen;
        }
    }
    else {
        char *cp;
        for(cp=str; *cp; cp++) if (islower(*cp)) *cp = toupper(*cp);
    }
    return str;
}
#elif defined(_WIN32)
static char* atowupper(char* str)
{
    return AnsiUpper(str);
}
#endif

DO_EXPORT int cash_key_code_from_string(const char *name)
{
    int code = 0;
    if (strlen(name) == 1) {
        code = name[0];
        if (code < MIN_KEY_CODE || code > 126)
            code = 0;
    }
    else {
        char buf[100];
        char *buf1 = do_strdup(name);
        buf1 = atowupper(buf1);
        int key;
        for (key = 1; key <= 12; key++)
        {
            sprintf(buf, "F%d", key);
            if (!strcmp(buf, buf1)) {
                code = key + 264;
                break;
            }
            sprintf(buf, "SHIFT+F%d", key);
            if (!strcmp(buf, buf1)) {
                code = key + 276;
                break;
            }
            sprintf(buf, "CTRL+F%d", key);
            if (!strcmp(buf, buf1)) {
                code = key + 288;
                break;
            }
        }
        do_free(buf1);

    }
    return code;
}

DO_EXPORT int cash_key_code_max()
{
    return MAX_KEY_CODE;
}
DO_EXPORT int cash_key_code_min()
{
    return MIN_KEY_CODE;
}
