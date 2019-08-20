#ifndef CSV_UTIL_H_INCLUDED
#define CSV_UTIL_H_INCLUDED

#define CSV_DELSPACE(cp)  while (cp[0] == ' ' || cp[0] == '\t') ++cp
#define CSV_EOS(cp) (cp[0] == '\0' || cp[0] == '\n' || cp[0] == '\r')
#define CSV_SPACE(cp) (cp[0] == ' ' || cp[0] == '\t')
#define CSV_COMMENT(cp) (cp[0] == '#' || cp[0] == ';' || cp[0] == '\0' || cp[0] == '\n' ||  cp[0] == '\r')

#ifdef __cplusplus
extern "C"
{
#endif
char* csv_getelem(char **cp);
int   csv_isname(char** cp, const char* cmd);
#ifdef __cplusplus
}
#endif

#endif // CSV_UTIL_H_INCLUDED
