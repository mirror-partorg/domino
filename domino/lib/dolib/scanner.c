#include <string.h>
#include <dolib.h>
#include "../misc/iconv_.h"
#include "../misc/define.h"
#include <ctype.h>

DO_EXPORT do_scanner_t *do_scanner_init(do_scanner_t *do_scanner, char *text, iconv_t conv)
{
    do_scanner->crnt = 0;
    do_scanner->crntln = 1;
    do_scanner->text = text;
    do_scanner->lexem[0] = '\0';
    do_scanner->conv = conv;
    return do_scanner;
}
DO_EXPORT do_scanner_t *do_scanner_copy(do_scanner_t *dest, do_scanner_t *src)
{
    dest->crnt = src->crnt;
    dest->crntln = src->crntln;
    dest->text = src->text;
    strcpy(dest->lexem, src->lexem);
    return dest;
}
DO_EXPORT char        *do_scanner_pos(do_scanner_t *do_scanner)
{
    return do_scanner->text + do_scanner->crnt;
}

DO_EXPORT do_lex_t do_scanner_step(do_scanner_t *do_scanner)
{
    int len = 0;
    for (; isspace(do_scanner->text[do_scanner->crnt]) || do_scanner->text[do_scanner->crnt] == '\r' || do_scanner->text[do_scanner->crnt] == '\n' ; (do_scanner->crnt)++ )
        if ( do_scanner->text[do_scanner->crnt] == '\n' ) (do_scanner->crntln)++;

    if (do_scanner->text[do_scanner->crnt] == '\0')
        return do_scanner->lex = do_lx_end;

    if (do_scanner->text[do_scanner->crnt] == '=') {
        do_scanner->lexem[0] = do_scanner->text[do_scanner->crnt];
        do_scanner->lexem[1] = '\0';
        (do_scanner->crnt)++;
        return do_scanner->lex = do_lx_equal;
    }

    if (do_scanner->text[do_scanner->crnt] == ':') {
        do_scanner->lexem[0] = do_scanner->text[do_scanner->crnt];
        do_scanner->lexem[1] = '\0';
        (do_scanner->crnt)++;
        return do_scanner->lex = do_lx_colon;
    }

    if (do_scanner->text[do_scanner->crnt] == ',') {
        do_scanner->lexem[0] = do_scanner->text[do_scanner->crnt];
        do_scanner->lexem[1] = '\0';
        (do_scanner->crnt)++;
        return do_scanner->lex = do_lx_comma;
    }

    if (do_scanner->text[do_scanner->crnt] == '?') {
        do_scanner->lexem[0] = do_scanner->text[do_scanner->crnt];
        do_scanner->lexem[1] = '\0';
        (do_scanner->crnt)++;
        return do_scanner->lex = do_lx_question;
    }

    if (do_scanner->text[do_scanner->crnt] == '-') {
        do_scanner->lexem[0] = do_scanner->text[do_scanner->crnt];
        do_scanner->lexem[1] = '\0';
        (do_scanner->crnt)++;
        len++;
        if (!isdigit(do_scanner->text[do_scanner->crnt]))
            return do_scanner->lex = do_lx_minus;
    }

    if (do_scanner->text[do_scanner->crnt] == '+') {
        do_scanner->lexem[0] = do_scanner->text[do_scanner->crnt];
        do_scanner->lexem[1] = '\0';
        (do_scanner->crnt)++;
        if (!isdigit(do_scanner->text[do_scanner->crnt]))
            return do_scanner->lex = do_lx_plus;
    }

    if (isdigit(do_scanner->text[do_scanner->crnt])) {
        for (; isdigit(do_scanner->text[do_scanner->crnt]); (do_scanner->crnt)++)
            do_scanner->lexem[len++] = do_scanner->text[do_scanner->crnt];
        if (do_scanner->text[do_scanner->crnt] != '.' &&
            !isalpha(do_scanner->text[do_scanner->crnt]) &&
            do_scanner->text[do_scanner->crnt] != '/') {
            do_scanner->lexem[len] = '\0';
            return do_scanner->lex = do_lx_cinteger;
        }
        if (do_scanner->text[do_scanner->crnt] == '.') {
            do_scanner->lexem[len++] = do_scanner->text[(do_scanner->crnt)++];
            for (; isdigit(do_scanner->text[do_scanner->crnt]); (do_scanner->crnt)++)
                do_scanner->lexem[len++] = do_scanner->text[do_scanner->crnt];
            do_scanner->lexem[len] = '\0';
            return do_scanner->lex = do_lx_creal;
        }
        if (do_scanner->text[do_scanner->crnt] == '/') {
            do_scanner->lexem[len++] = do_scanner->text[(do_scanner->crnt)++];
            for (; isdigit(do_scanner->text[do_scanner->crnt]); (do_scanner->crnt)++)
                do_scanner->lexem[len++] = do_scanner->text[do_scanner->crnt];
            if (do_scanner->text[do_scanner->crnt] != '/')
                return do_scanner->lex = do_lx_error;
            do_scanner->lexem[len++] = do_scanner->text[(do_scanner->crnt)++];
            for (; isdigit(do_scanner->text[do_scanner->crnt]); (do_scanner->crnt)++)
                do_scanner->lexem[len++] = do_scanner->text[do_scanner->crnt];
            do_scanner->lexem[len] = '\0';
            return do_scanner->lex = do_lx_cdate;
        }
        for (; isalpha(do_scanner->text[do_scanner->crnt]); (do_scanner->crnt)++)
            do_scanner->lexem[len++] = do_scanner->text[do_scanner->crnt];
        do_scanner->lexem[len] = '\0';
        return do_scanner->lex = do_lx_ident;
    }

    if (isalpha(do_scanner->text[do_scanner->crnt])) {
        for (; isalpha(do_scanner->text[do_scanner->crnt]) || isdigit(do_scanner->text[do_scanner->crnt]) || do_scanner->text[do_scanner->crnt] == '_'; (do_scanner->crnt)++)
            do_scanner->lexem[len++] = do_scanner->text[do_scanner->crnt];
        do_scanner->lexem[len] = '\0';
        return do_scanner->lex = do_lx_ident;
    }
    if (do_scanner->text[do_scanner->crnt] == '"') {
        (do_scanner->crnt) ++;
        char *buf;
        for (;  (do_scanner->text[do_scanner->crnt] != '\0') && (do_scanner->text[do_scanner->crnt] != '"' || do_scanner->text[(do_scanner->crnt) + 1] == '"') ; (do_scanner->crnt)++) {
            if (do_scanner->text[do_scanner->crnt] == '"')
                (do_scanner->crnt)++;
            do_scanner->lexem[len++] = do_scanner->text[do_scanner->crnt];
        }
        if (do_scanner->text[do_scanner->crnt] != '"')
            return do_scanner->lex = do_lx_error;
        do_scanner->lexem[len] = '\0';
        if (do_scanner->conv != (iconv_t)-1) {
            buf = iconv_(do_scanner->conv, do_scanner->lexem);
            if (strlen(buf) > DO_MAX_LEXEM_LEN)
                len = DO_MAX_LEXEM_LEN;
            else
                len = strlen(buf);
            strncpy(do_scanner->lexem, buf, len);
            do_scanner->lexem[len] = '\0';
        }
        (do_scanner->crnt)++;
        return do_scanner->lex = do_lx_cstring;
    }
    (do_scanner->crnt) ++;
    return do_scanner->lex = do_lx_error;
}
