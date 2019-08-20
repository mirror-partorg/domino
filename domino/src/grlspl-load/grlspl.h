#ifndef GRLSPL_H_INCLUDED
#define GRLSPL_H_INCLUDED


#ifdef __cplusplus
extern "C"
{
#endif

int grlspl_load_from_xls(const char *alias_name,
                         const char *filename,
                         const char *configfilename,
                         int clear,
                         int use_utf8,
                         int info,
                         int sendmessage);

#ifdef __cplusplus
}
#endif


#endif // GRLSPL_H_INCLUDED
