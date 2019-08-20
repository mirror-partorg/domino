
#include "csv_util.h"
#include <string.h>

char* csv_getelem(char **cp)
{
    char *result;
    int len;
    CSV_DELSPACE(*cp);
    if (**cp == '"')
    {
        result = ++(*cp);
        while (!CSV_EOS(*cp) && (**cp != '"' || (*cp)[1] == '"'))
            if (**cp == '"') (*cp) +=2; else (*cp)++;
        len = (*cp) - result;
        while (!CSV_EOS(*cp) && **cp != ',') (*cp)++;
    }
    else
    {
        result = *cp;
        while (!CSV_EOS(*cp) && **cp != ',') (*cp)++;
        char *tail = (*cp) - 1;
        while(tail > result && CSV_SPACE(tail)) tail--;
        len = tail - result + 1;
    }
    if ( !CSV_EOS(*cp) ) (*cp)++;
    result[len] = '\0';
    return result;
}

int csv_isname(char** cp, const char* cmd)
{
    char *h = *cp;
    if (!strncmp(*cp, cmd, strlen(cmd)))    {
        *cp += strlen (cmd);
        CSV_DELSPACE(*cp);
        if (*cp[0] == '=')
        {
            ++*cp;
            return 1;
        }
        else
        {
            *cp = h;
            return 0;
        }
    }
    else
        return 0;
}

