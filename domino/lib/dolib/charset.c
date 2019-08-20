#include "charset.h"
#include <stdio.h>

struct cp_map {
	int codepage;
	char *charset_name;
};


struct cp_map cp_to_charset [] = {
	{10000,"mac-roman"},
	{10001,"mac-japanese"},
	{10002,"mac-tchinese"},
	{10003,"mac-korean"},
	{10004,"mac-arabic"},
	{10005,"mac-hebrew"},
	{10006,"mac-greek1"},
	{10007,"mac-cyrillic"},
	{10008,"mac-schinese"},
	{10010,"mac-romania"},
	{10017,"mac-ukraine"},
	{10021,"mac-thai"},
	{10029,"mac-centeuro"},
	{10079,"mac-iselandic"},
	{10081,"mac-turkish"},
	{10082,"mac-croatia"},
	{20866,"koi8-r"},
	{28591,"8859-1"},
	{28592,"8859-2"},
	{28593,"8859-3"},
	{28594,"8859-4"},
	{28595,"8859-5"},
	{28596,"8859-6"},
	{28597,"8859-7"},
	{28598,"8859-8"},
	{28599,"8859-9"},
	{28605,"8859-15"},
	{65001,"utf-8"},
    {0,NULL}};
const char *charset_from_codepage(unsigned int codepage) {

	static char buffer[7];
	struct cp_map *cp;
	if (codepage==1200||codepage==1201) {
		/* For UCS2 */
		return "";
	} else
	if (codepage<10000) {
		sprintf(buffer,"cp%d",codepage);
		return buffer;
	} else {
		for (cp = cp_to_charset;cp->codepage!=0&& cp->codepage!=codepage;cp++);
		return cp->charset_name;
	}
}
