#include <stdio.h>
#include <stdlib.h>
#include "price-scan.h"
#include "storage-db.h"
#include <glib.h>
#include <errno.h>
#include <string.h>
#include <locale.h>

static int read_file(const char *filename);
StorageDb *db;

int main(int argv, char **argc)
{
    int res;
    setlocale(LC_ALL, "");
    g_type_init();
    if ( argv != 2 ) {
        fprintf(stderr, "Использование: price-parser <text filename>");
        exit(1);
    }
    db = storage_db_new("host=localhost dbname=glekar user=mirror");
    if ( !storage_db_connect(db) )
        exit(2);

    res = read_file(argc[1]);

    return (res) ? 0 : 1;
}

static int read_file(const char *filename)
{
    FILE *fp;
    if ( (fp = fopen(filename, "r")) == NULL )  {
        g_log(NULL, G_LOG_LEVEL_ERROR, "Ошибка открытия файла (%s): %s\n", filename, strerror(errno));
        return FALSE;
    }
    int res;
    size_t len = 1024;
    char* line = (char*)g_malloc(len + 1);
    PriceScan *scan;
    PriceScanLexemType lex;
    int n=0;
    int slen;

    res = TRUE;
    scan = price_scan_new(NULL);

    while ( !feof(fp) && res )  {
        if ( fgets(line, len+1, fp) == NULL ) break;

        while ( (strlen(line) == len) && !feof(fp) ) {
            line = (char *)g_realloc(line, len * 2 + 1);
            if ( !fgets(line+len, len+1, fp) ) {
                g_free(line);
                g_log(NULL, G_LOG_LEVEL_ERROR, "Ошибка чтения файла (%s): %s\n", filename, strerror(errno));
                res = FALSE;
                break;
            }
            len *= 2;
        }
        slen = strlen(line);
        if ( slen > 0 && line[slen-1] == '\n' )
            line[slen-1] = '\0';

        char *repl;
        repl = NULL;//storage_db_get_error_replace(db, line);
        if ( repl )
            price_scan_set_string(scan, repl, n);
        else
            price_scan_set_string(scan, line, n);
        while ( res && (lex = price_scan_step(scan)) != PRICE_SCAN_LEXEM_END ) {
            switch (lex) {
                case PRICE_SCAN_LEXEM_ERROR:
                    res = FALSE;
                    break;
                case PRICE_SCAN_LEXEM_WORD_COMBINATION:
                    //fprintf(stderr, "%s\n", price_scan_get_lexem(scan));
                    break;
                default:
                    break;
            }
        }
        if ( repl )
            g_free(repl);

        n++;
        //if ( (n % 1000) == 0 )
          //  fprintf(stderr, "line %d\n", n);
    }
    fprintf(stderr, "line %d\n", n);
    g_free(line);
    fclose(fp);
    return res;
}

