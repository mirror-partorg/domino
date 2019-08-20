#include <dolib.h>
#include <errno.h>
#include "../misc/define.h"
#include "ole.h"
#include "xltypes.h"
#include "numutils.h"
#include "../misc/iconv_.h"
#include "charset.h"
#include <math.h>

#define NUMOFDATEFORMATS 13
#define FLT_FORMAT(a,b,c) a #b c
#define MK_FORMAT(x) FLT_FORMAT("%.",x,"g")

struct _do_xls_t {
    char         *filename;
    do_list_t    *sheets;
    int           use_utf8;
};

typedef struct {
    char *name;
    do_list_t *rows;
} do_sheet_t;

typedef struct {

// read
    iconv_t       uconv;
    iconv_t       cconv;
    do_sheet_t   *sheet;
    short int    *formatTable;
    char         *forced_date_format;
    size_t        formatTableIndex;
    size_t        formatTableSize;
    char          number_format[8];
    double        date_shift;
    int           prev_rectype;
    int           codepage;
    char          FormatIdxUsed[NUMOFDATEFORMATS];

// unparce sst
    do_list_t *sst;
    int        sst_indx;
    char      *sst_rest;
    int        sst_rest_size;

    int sref_row;
    int sref_col;

} crnt_t;


static crnt_t *crnt_new(int use_utf8);
static void    crnt_free(crnt_t *crnt);

static int do_xls_cell_set(do_xls_t *xls, crnt_t *crnt, int row, int col, char *str);
static int do_xls_read(do_xls_t *xls);
static int do_xls_table(do_xls_t *xls, crnt_t *crnt, ole_t *ole, FILE *input);
static int do_xls_process_item(do_xls_t *xls, crnt_t *crnt, int rectype, int reclen, char *rec);
static int do_xls_parse_sst(do_xls_t *xls, crnt_t *crnt, int reclen, char *rec);
static char *do_xls_read_string (do_xls_t *xls, crnt_t *crnt, unsigned char **src, int max_len);
static void SetFormatIdxUsed(do_xls_t *xls, crnt_t *crnt, int format_code);
static void SetCodePage(do_xls_t *xls, crnt_t *crnt, int code_page);
static char *number2string(do_xls_t *xls, crnt_t *crnt, double d,short int format_code);
static char *format_double(do_xls_t *xls, crnt_t *crnt, char *rec,int offset,int format_code);
static char *format_int(do_xls_t *xls, crnt_t *crnt, int value,int format_code);
static char* format_rk(do_xls_t *xls, crnt_t *crnt, char *rec,short int format_code);

#define UNICODE "UCS2"

DO_EXPORT do_xls_t *do_xls_new(const char *filename, int use_utf8)
{
    do_xls_t *xls;
    xls = do_malloc0(sizeof(do_xls_t));
    xls->filename = do_strdup(filename);
    xls->sheets = do_list_new(FALSE);
    xls->use_utf8 = use_utf8;
    if ( !do_xls_read(xls) ) {
        do_xls_free(xls);
        xls = NULL;
    }
    return xls;
}
static crnt_t *crnt_new(int use_utf8)
{
    crnt_t *crnt;
    crnt = do_malloc0(sizeof(crnt_t));

    crnt->date_shift = 25569.0;
    strcpy(crnt->number_format, MK_FORMAT(__DBL_DIG__));

    const char *str;
    if (use_utf8)
        str = UTF_CHARSET;
    else
        str = get_locale_lang();

    crnt->uconv = iconv_open(str, UNICODE);
    if (crnt->uconv == (iconv_t)-1)
        do_log(LOG_ERR, "Error iconv_open (\"%s\" to \"%s\") ", UNICODE, str);
    crnt->cconv = (iconv_t)-1;

    return crnt;
}
DO_EXPORT void do_xls_free(do_xls_t *xls)
{
    int i, j;
    do_free(xls->filename);
    for ( i = 0; i < xls->sheets->count; i++ ) {
        do_sheet_t *sheet;
        sheet = xls->sheets->list[i];
        do_free(sheet->name);
        for ( j = 0; j < sheet->rows->count; j++ ) {
            if ( sheet->rows->list[j] )
                do_list_free(sheet->rows->list[j]);
        }
        do_list_free(sheet->rows);
    }
    do_list_free(xls->sheets);
    do_free(xls);
}
static void crnt_free(crnt_t *crnt)
{
    if (crnt->uconv != (iconv_t)-1)
        iconv_close(crnt->uconv);
    if (crnt->cconv != (iconv_t)-1)
        iconv_close(crnt->cconv);
    if ( crnt->formatTable ) do_free(crnt->formatTable);
    if ( crnt->forced_date_format ) do_free(crnt->forced_date_format);
    if ( crnt->sst_rest ) do_free(crnt->sst_rest);
    if ( crnt->sst ) do_list_free(crnt->sst);
    do_free(crnt);
}
DO_EXPORT int do_xls_worksheet_count(do_xls_t *xls)
{
    return xls->sheets->count;
}
DO_EXPORT int do_xls_row_count(do_xls_t *xls, int worksheet)
{
    if ( worksheet < xls->sheets->count )
        return ((do_sheet_t*)xls->sheets->list[worksheet])->rows->count;
    return 0;
}
static char *empty = "";
DO_EXPORT const char *do_xls_cell_str(do_xls_t *xls, int worksheet, int row, int col)
{
    if ( worksheet < xls->sheets->count &&
         row < ((do_sheet_t*)xls->sheets->list[worksheet])->rows->count &&
        ((do_sheet_t*)xls->sheets->list[worksheet])->rows->list[row] != NULL &&
         col < ((do_list_t*)((do_sheet_t*)xls->sheets->list[worksheet])->rows->list[row])->count &&
        ((do_list_t*)((do_sheet_t*)xls->sheets->list[worksheet])->rows->list[row])->list[col]
         )
        return ((do_list_t*)((do_sheet_t*)xls->sheets->list[worksheet])->rows->list[row])->list[col];
    return empty;
}

static int do_xls_read(do_xls_t *xls)
{
    ole_t *ole;
    FILE *ole_file;
    FILE *input;
    int res;


    input = fopen(xls->filename, "rb");
    if ( !input ) {
        do_log(LOG_ERR, "Error opening excel file (%s): %s", xls->filename, strerror(errno));
        return FALSE;
    }
    res = TRUE;
    if ( (ole = ole_init(input)) != NULL )  {
        while ( (ole_file = ole_readdir(ole)) != NULL ) {
            int res=ole_open(ole_file);
// 				do_log(LOG_INFO, "name = %s\n", ((oleEntry*)ole_file)->name);
            if ( res >= 0 ) {
                if ( !strcasecmp(((oleEntry*)ole_file)->name , "Workbook" ) ||
                     !strcasecmp(((oleEntry*)ole_file)->name, "Book")  ) {
                    crnt_t *crnt;
                    crnt = crnt_new(xls->use_utf8);
                    if ( !do_xls_table(xls, crnt, ole, ole_file) ) {
                        res = FALSE;
                        break;
                    }
                    crnt_free(crnt);
                }
            }
            ole_close(ole_file);
        }
        ole_finish(ole);
    }
    else {
        do_log(LOG_INFO, "%s is not OLE file or Error", xls->filename);
        res = FALSE;
    }
    fclose(input);
    return res;
}
static int do_xls_table(do_xls_t *xls, crnt_t *crnt, ole_t *ole, FILE *input)
{
	long rectype;
	long reclen;
	long build_year = 0;
	long build_rel = 0;
	long offset = 0;
	int  eof_flag = 0;
	int  itemsread = TRUE;
    char rec[MAX_MS_RECSIZE];
    int  biff_version;

	crnt->date_shift = 25569.0; // Windows 1900 date system

	while ( itemsread ) {
		ole_read(ole, rec, 2, 1, input);
		biff_version = get_uint16(rec, 0);
		ole_read(ole, rec, 2, 1, input);
		reclen=get_uint16(rec,0);
		if ( biff_version == 0x0809 || biff_version == 0x0409 ||
				 biff_version == 0x0209 || biff_version == 0x0009 ) {
			if ( reclen==8 || reclen==16 ) {
				if ( biff_version == 0x0809 ) {
					itemsread = ole_read(ole, rec, 4, 1, input);
					build_year = get_uint16(rec+2, 0);
					build_rel = get_uint16(rec, 0);
					if ( build_year > 5 ) {
						itemsread=ole_read(ole, rec, 8, 1, input);
						biff_version=8;
						offset=12;
					}
					else {
						biff_version=7;
						offset=4;
					}
				}
				else if ( biff_version == 0x0209 ) {
					biff_version=3;
					offset=2;
				}
				else if ( biff_version == 0x0409 ) {
					offset=2;
					biff_version=4;
				}
				else {
					biff_version=2;
				}
				itemsread = ole_read(ole, rec, reclen-offset, 1, input);
				break;
			} else {
				do_log(LOG_ERR,"%s: Invalid BOF record", xls->filename);
				return FALSE;
			}
		} else {
			itemsread=ole_read(ole, rec, 126, 1, input);
		}
	}
	if ( ole_eof(input) ) {
		do_log(LOG_ERR,"%s: No BOF record found", xls->filename);
		return FALSE;
	}
	while ( itemsread ) {
		char buffer[2];
		rectype = 0;
		itemsread = ole_read(ole, buffer, 2, 1, input);
		if ( ole_eof(input) ) {
			do_xls_process_item(xls, crnt, MSEOF, 0, NULL);
			return TRUE;
		}

		rectype = get_uint16(buffer, 0);
		if( !itemsread )
			break;

		reclen = 0;

		itemsread = ole_read(ole, buffer, 2, 1, input);
		reclen = get_uint16(buffer,0);
		if ( reclen && reclen < MAX_MS_RECSIZE && reclen > 0 ){
			itemsread = ole_read(ole, rec, 1, reclen, input);
			rec[reclen] = '\0';
		}
		if ( eof_flag ) {
			if ( rectype != BOF ) {
				break;
			}
		}
// 		do_log(LOG_INFO,"Rectype 0x%04X reclen=%d\n",rectype, reclen);
		if ( !do_xls_process_item(xls, crnt, rectype, reclen, (char*)rec) )
            return FALSE;
		eof_flag = rectype == MSEOF;
	}
	return TRUE;
}
static int do_xls_process_item(do_xls_t *xls, crnt_t *crnt, int rectype, int reclen, char *src)
{
    uint16_t row = 0;
    uint16_t col = 0;
    uint16_t ixf;
    uint16_t indx;
    char *rec;
    rec = src;
    if ( reclen > 4 ) {
        row = get_uint16(rec,0);
        col = get_uint16(rec,2);
    }
    if ( row == 7641 ) {
        row = 7641;
    }
	switch ( rectype ) {
        case FILEPASS:
            do_log(LOG_ERR, "File is encrypted");
            return FALSE;
        case WRITEPROT:
            do_log(LOG_INFO,"File is write protected");
            break;
        case CODEPAGE:
            SetCodePage(xls, crnt, get_uint16(rec, 0));
            break;
        case FORMAT:
            SetFormatIdxUsed(xls, crnt, get_uint16(rec, 0));
            break;
        case SST:
//     		do_log(LOG_INFO,"SST");
            if ( crnt->sst_rest != NULL ) {
                do_free(crnt->sst_rest);
                crnt->sst_rest = NULL;
            }
            if ( crnt->sst ) {
                do_list_free(crnt->sst);
                crnt->sst = NULL;
            }

            if ( !do_xls_parse_sst(xls, crnt, reclen, rec) )
                return FALSE;

            break;
        case CONTINUE:
            if ( crnt->prev_rectype == SST ) {
                if ( !do_xls_parse_sst(xls, crnt, reclen, rec) )
                    return FALSE;
            }
            return TRUE;
        case LABEL:
            row = get_uint16(rec, 0);
            col = get_uint16(rec, 2);
            ixf = get_uint16(rec, 4);
            rec += 6;
            do_xls_cell_set(xls, crnt, row, col,
                             do_xls_read_string(xls, crnt, (unsigned char**)&rec, -1)
                            );
            break;
        case BLANK:
            row = get_uint16(rec, 0);
            col = get_uint16(rec, 2);
            do_xls_cell_set(xls, crnt, row, col, NULL);
            break;
        case MULBLANK:
            row = get_uint16(rec, 0);
            col = get_uint16(rec, reclen - 2);
            do_xls_cell_set(xls, crnt, row, col, NULL);
            break;
        case CONSTANT_STRING:
            row = get_uint16(rec,0);
            col = get_uint16(rec,2);
            indx = get_uint16(rec,6);
            if ( !crnt->sst ) {
                do_log(LOG_ERR, "CONSTANT_STRING before SST parsed");
                return FALSE;
            }
            // do_log(LOG_INFO,"col=%d row=%d no=%d\n",col,row,string_no);
            crnt->sref_row = -1;
            if ( indx >=  crnt->sst->count ) {
                do_log(LOG_ERR, "string index out of boundary");
                return FALSE;
            }
            if ( crnt->sst->list[indx] ) {
                do_xls_cell_set(xls, crnt, row, col, do_strdup(crnt->sst->list[indx]));
            }
            else
                do_xls_cell_set(xls, crnt, row, col, NULL);
            break;
        case 0x03:
        case 0x103:
        case 0x303:
        case NUMBER:
            row = get_uint16(rec, 0);
            col = get_uint16(rec, 2);
            ixf = get_uint16(rec, 4);
            crnt->sref_row = -1;
            do_xls_cell_set(xls, crnt, row, col, do_strdup(format_double(xls, crnt, rec, 6,
                                                                   get_uint16(rec,4))));
            break;
        case INTEGER_CELL:
            row = get_uint16(rec,0);
            col = get_uint16(rec,2);
            do_xls_cell_set(xls, crnt, row, col, do_strdup(format_int(xls, crnt, get_uint16(rec,7),
                                                                get_uint16(rec,4))));
            break;
        case RK: {
            int format_code;

            crnt->sref_row = -1;
            row = get_uint16(rec,0);
            col = get_uint16(rec,2);
            format_code = get_uint16(rec,4);
            do_xls_cell_set(xls, crnt, row, col, do_strdup(format_rk(xls, crnt, rec+6,format_code)));
            break;
        }
        case MULRK: {
            int startcol,endcol,offset,format_code;
            row = get_uint16(rec,0);
            startcol = get_uint16(rec,2);
            endcol = get_uint16(rec,reclen-2);
            crnt->sref_row = -1;

            for (offset=4,col=startcol;col<=endcol;offset+=6,col++) {
                format_code=get_uint16(rec,offset);
                do_xls_cell_set(xls, crnt, row, col, do_strdup(format_rk(xls ,crnt, rec+offset+2,format_code)));

            }
            break;
        }
        case FORMULA: {
            crnt->sref_row = -1;
            row = get_uint16(rec,0);
            col = get_uint16(rec,2);
            if ( ((unsigned char)rec[12]==0xFF) && (unsigned char)rec[13]==0xFF ) {
                /* not a floating point value */
                if (rec[6]==1) {
                    /*boolean*/
                    char buf[2]="0";
                    buf[0]+=rec[9];
                    do_xls_cell_set(xls, crnt, row, col, do_strdup(buf));
                } else if (rec[6]==2) {
                    /*error*/
                    char buf[6]="ERROR";
                    do_xls_cell_set(xls, crnt, row, col, do_strdup(buf));
                } else if (rec[6]==0) {
                    crnt->sref_row = row;
                    crnt->sref_col = col;
                }
            } else {
                int format_code=get_uint16(rec,4);

                do_xls_cell_set(xls, crnt, row, col, do_strdup(format_double(xls, crnt, rec,6,format_code)));
            }
            break;
        }
        case STRING:
            if ( crnt->sref_row == -1 ) {
                do_log(LOG_ERR, "String record without preceeding string formula");
                break;
            }
            do_xls_cell_set(xls, crnt, crnt->sref_row, crnt->sref_col,
                             do_xls_read_string(xls, crnt, (unsigned char**)&rec, -1)
                            );
            break;
        case BOF: {
            if ( crnt->sheet ) {
                do_log(LOG_ERR, "BOF when current sheet is not flushed");
                crnt->sheet = NULL;
            }
            break;
        }
        case XF:
        case 0x43: // from perl module Spreadsheet::ParseExecel
            {
                short int formatIndex = get_uint16(rec,2);
                // we are interested only in format index here
                if (crnt->formatTableIndex >= crnt->formatTableSize) {
                    crnt->formatTable = do_realloc(crnt->formatTable,
                                            (crnt->formatTableSize+=16)*sizeof(short int));
                    if (!crnt->formatTable) {
                        do_log(LOG_ERR, "Out of memory for format table");
                        return FALSE;
                    }
                }
                crnt->formatTable[crnt->formatTableIndex++] = formatIndex;
                break;
            }
        case MS1904: // Macintosh 1904 date system
            crnt->date_shift=24107.0;
            break;


        case MSEOF:
            crnt->sheet = NULL;
            break;
        case ROW: {
            //  		do_log(LOG_INFO,"Row! %d %d %d\n",get_uint16(rec,0), get_uint16(rec+2,0),get_uint16(rec+4,0));
            break;
        }
        case INDEX: {
            // 		do_log(LOG_INFO,"INDEX! %d %d\n", get_int32(rec+4,0), get_int32(rec+8,0));
            break;
        }
        default: {
            //do_log(LOG_INFO, "Unknown record 0x%x\n length %d\n",rectype,reclen);
        }
	}
	crnt->prev_rectype=rectype;
	return TRUE;
}
static int D = 0;
static int sad  = 0;

static int do_xls_parse_sst(do_xls_t *xls, crnt_t *crnt, int reclen, char *rec)
{
	int32_t count;
	char *cp;
	char *tail_cp;
	char *buff = NULL;
	char *str;

    if ( !crnt->sst ) {
        cp = rec;
        tail_cp = rec + reclen;
        count = get_int32(cp, 0);
        count = get_int32(cp, 4);
        if ( count <= 0 ) { // just in case
            do_log(LOG_ALERT, "count of sst error %d", count);
            return TRUE;
        }
        crnt->sst = do_list_new(TRUE);
        do_list_insert(crnt->sst, count - 1, NULL);
        crnt->sst_indx = 0;
        cp += 8;
    }
    else {
        if ( crnt->sst_rest ) {
                //D=!D;

            buff = do_malloc(crnt->sst_rest_size + reclen);
            if ( !buff ) {
                do_log(LOG_ERR, "Out of memory: %db ERR=%s", crnt->sst_rest_size + reclen, strerror(errno));
                return FALSE;
            }
            memcpy(buff, crnt->sst_rest, crnt->sst_rest_size);
            memcpy(buff + crnt->sst_rest_size, rec + D, reclen - D);
            cp = buff;
            tail_cp = buff + crnt->sst_rest_size + reclen - D;
            do_save_to_file("./sad", cp + 3, crnt->sst_rest_size + reclen - 3);
            sad = 1;
        }
        else {
            cp = rec;
            tail_cp = rec + reclen;
        }
    }
	while ( cp < tail_cp && crnt->sst_indx < crnt->sst->count ) {
	    if ( crnt->sst_indx == 789 )
            crnt->sst_indx = 789;
	    char *h;
	    h = cp;
		str = do_xls_read_string(xls, crnt, (unsigned char**)&cp, tail_cp - cp);
		if ( !str ) {
		    cp = h;
		    break;
		}
        do_list_insert(crnt->sst, crnt->sst_indx++, str);
	}
	if ( crnt->sst_rest ) {
        do_free(crnt->sst_rest);
        crnt->sst_rest = NULL;
	}
	if ( cp < tail_cp ) {
	    crnt->sst_rest_size = tail_cp - cp;
	    crnt->sst_rest = do_malloc(crnt->sst_rest_size);
	    memcpy(crnt->sst_rest, cp, crnt->sst_rest_size);
	}
	if ( buff )
        do_free(buff);


	// 	do_log(LOG_INFO, "end sst i=%d sstsize=%d\n",i,sstsize);
    return TRUE;

}

static int BuiltInDateFormatIdx (int index) {
	int offset;
	offset=1; /* offset of date formats */
	/* 0 is used as false -- format not found */
	if ((index>= 0x0E) && (index<=0x16)) {
		return offset+index-0x0E;
	} else
		if ((index>=0x2d) && (index<=0x2F)) {
			return offset+index-0x2d+9;
		} else if (index==0xa4) {
			return 12+offset;
		} else
			return 0;
}

static void SetFormatIdxUsed(do_xls_t *xls, crnt_t *crnt, int format_code) {
	int dateindex;
 	/*do_log(LOG_INFO,"Format idx %x to be set to dirty\n",format_code);
	 */
	dateindex=BuiltInDateFormatIdx(format_code);
	if (dateindex) {
	  crnt->FormatIdxUsed[dateindex]=1;
 	  /*do_log(LOG_INFO,"Date idx %d is set to be dirty\n",dateindex); */
	}
}
static int do_xls_cell_set(do_xls_t *xls, crnt_t *crnt, int row, int col, char *str)
{
    if ( !crnt->sheet ) {
        crnt->sheet = do_malloc0(sizeof(do_sheet_t));
        crnt->sheet->name = do_strdup("");
        crnt->sheet->rows = do_list_new(FALSE);
        do_list_add(xls->sheets, crnt->sheet);
    }
    if ( row >= crnt->sheet->rows->count )
        do_list_insert(crnt->sheet->rows, row, NULL);
    if ( !crnt->sheet->rows->list[row] )
        crnt->sheet->rows->list[row] = do_list_new(TRUE);
    if ( col >= ((do_list_t*)crnt->sheet->rows->list[row])->count )
        do_list_insert(crnt->sheet->rows->list[row], col, str);
    else {
        if ( ((do_list_t*)crnt->sheet->rows->list[row])->list[col] )
            do_free(((do_list_t*)crnt->sheet->rows->list[row])->list[col]);
        ((do_list_t*)crnt->sheet->rows->list[row])->list[col] = str;
    }
    return TRUE;
}
/*
static char *do_xls_read_string (do_xls_t *xls, crnt_t *crnt, void *src, uint16_t *len, int max_len)
{
    char *retval;

	char *ptr;
	char *tail;

	ptr = src;
	if ( max_len != -1 )
        tail = src + max_len;
    else
        tail = NULL;

    int unicode = *(ptr) & 1;

    if ((*((unsigned char*)ptr) & 0x08) && *((unsigned char*)ptr + 2) == 0x00) {
            //formattingRuns = *(ptr+3);
        ptr += 2;
    }
    ptr += 1;
    if ( tail && ptr >= tail )
        return NULL;

    if ( unicode ) {
        if ( tail && ptr + *len*2 >= tail )
            return FALSE;
        retval = iconvn_(crnt->conv, ptr, *len * 2);
        *len = (ptr - (char*)src) + *len*2;
    }
    else {
        if ( tail && ptr + *len >= tail )
            return FALSE;
        retval = strndup(ptr, *len);
        *len = (ptr - (char*)src) + *len;
        // *src = (ptr + len);
        //!!if ( strlen(retval) < len )
    }
	return retval;

}
*/
char *unicodechar(crnt_t *crnt, int uc)
{
    char *retval;
    retval = iconvn_(crnt->uconv, (char*)&uc, 2);
    return retval;
}
char *codepagechar(crnt_t *crnt, char *s, int len)
{
    char *retval;
    retval = iconvn_(crnt->cconv, s, len);
    return retval;
}
static char *do_xls_read_string (do_xls_t *xls, crnt_t *crnt, unsigned char **src, int max_len)
{
	int flags = 0;
	int start_offset=0;
	int to_skip=0;

	int offset = 1;
	int charsize;
	char *dest;
	char *s,*d,*c;
	int count;
	char *tail;
	if ( max_len != -1 )
        tail = (char*)*src + max_len;
    else
        tail = NULL;

	int i,u,l,len;

	flags = *((unsigned char*) (*src + 1 + offset));
	if ( !( flags == 0 || flags == 1 || flags == 8 || flags == 9 ||
					flags == 4 || flags == 5 || flags == 0x0c || flags == 0x0d ) ) {
		count = **src;
		flags = *(*src+offset);
		offset --;
		flags = *(*src+1+offset);
		if (! ( flags == 0 || flags == 1 || flags == 8 || flags == 9 ||
						flags == 4 || flags == 5 || flags == 0x0c || flags == 0x0d ) ) {
			// 			do_log(LOG_INFO, "Strange flags = %d, returning NULL\n", flags);
			return NULL;
		}
	}
	else {
		count=get_uint16(*src,0);
	}
	charsize=(flags &0x01) ? 2 : 1;

	switch (flags & 12 ) {
	case 0x0c: // Far East with RichText formating
		to_skip=4*get_uint16(*src,2+offset)+get_int32(*src, 4+offset);
		start_offset=2+offset+2+4;
		// 		do_log(LOG_INFO, "Far East with RichText formating\n");
		break;

	case 0x08: // With RichText formating
		to_skip=4*get_uint16(*src,2+offset);
		start_offset=2+offset+2;
		// 		do_log(LOG_INFO, "With RichText formating %d\n",getshort(*src,2+offset));
		break;
	case 0x04: // Far East
		to_skip=get_int32(*src, 2+offset);
		start_offset=2+offset+4;
		// 		do_log(LOG_INFO, "Far East\n");
		break;

	default:
		to_skip=0;
		start_offset=2+offset;
		// 		do_log(LOG_INFO, "Default string\n");
	}

	// 	do_log(LOG_INFO, "count=%d skip=%d start_offset=%d\n",
	// 					count, to_skip, start_offset);
	if ( (dest=do_malloc(count+1)) == NULL ) {
		do_log(LOG_ERR, "Dest string alloc error");
		*src+=(to_skip+start_offset+(count*charsize));
		return NULL;
	}
	*src+=start_offset;
    if ( tail && (char*)*src >= tail )
        return NULL;

	len = count;
	*dest=0;l=0;
	for ( s = (char*)*src, d = dest, i = 0; i < count; i++, s+=charsize ) {
		// 		do_log(LOG_INFO, "l=%d len=%d count=%d charsize=%d\n",l,len,count,charsize);
        if ( tail && s >= tail )
            return NULL;

		if ( (charsize == 1 && (*s == 1 || *s == 0)) ||
				 (charsize == 2 && (*s == 1 || *s == 0) && *(s+1) != 4)) {
			// 			do_log(LOG_INFO, "extchar (unicode)=%02x %02x\n",*s, *(s+1));
			charsize=(*s &0x01) ? 2 : 1;
			if (charsize == 2)
				s-=1;
			count++;
			continue;
		}
		if ( charsize == 2 ){
			u = (unsigned short)get_uint16(s, 0);
			c = unicodechar(crnt, u);
			// 			do_log(LOG_INFO, "char=%02x %02x\n", *s, *(s+1));
		} else {
			c = codepagechar(crnt, s, charsize);
		}
		if (c != NULL) {
			int dl = strlen(c);
			while (l+dl>=len) {
				len += 16;
				dest = do_realloc(dest, len+1);
			}
			d = dest + l;
			strcpy(d,c);
			d += dl;
			l += dl;
			do_free(c);
		}
	}
	*src=((unsigned char*)s+to_skip);
    if ( tail && (char*)*src > tail ) {
        do_free(dest);
        return NULL;
    }

	return dest;
}

static char IsFormatIdxUsed(do_xls_t *xls, crnt_t *crnt, int format_code)
{
	int dateindex;
	dateindex = BuiltInDateFormatIdx(format_code);
	if (dateindex) {
		// 	  do_log(LOG_INFO, "Date idx %d is dirty\n",dateindex);
	  return crnt->FormatIdxUsed[dateindex]==1;
	}
	else return 0;
}
static char *GetBuiltInDateFormat(int dateindex) {
	static char *formats[]={
		/* reserved  */ NULL, /* BuiltInDateFormatIdx use dateindex=0 as flag format not found */
		/* 0x0E */ "%m-%d-%y",		/* 01 */
		/* 0x0F */ "%d-%b-%y",		/* 02 */
		/* 0x10 */ "%d-%b",		/* 03 */
		/* 0x11 */ "%b-%d",	 	/* 04 */
		/* 0x12 */ "%l:%M %p",		/* 05 */
		/* 0x13 */ "%l:%M:%S %p",		/* 06 */
		/* 0x14 */ "%H:%M",		/* 07 */
		/* 0x15 */ "%H:%M:%S",		/* 08 */
		/* 0x16 */ "%m-%d-%y %H:%M",	/* 09 */
		/* 0x2d */ "%M:%S",		/* 10 */
		/* 0x2e */ "%H:%M:%S",		/* 11 */
		/* 0x2f */ "%M:%S",		/* 12 */
		/* 0xa4 */ "%m.%d.%Y %l:%M:%S %p"	/* 13 */
	};
	if (dateindex>0 && dateindex <= NUMOFDATEFORMATS) {
	  return formats[dateindex];
	} else
		return NULL;
}

static char *isDateFormat(do_xls_t *xls, crnt_t *crnt, int format_code) {
	int index;
	int dateindex;
	if (format_code>=crnt->formatTableIndex) {
		do_log(LOG_ERR, "Format code %d is used before definition",format_code);
		return NULL;
	}

	index = crnt->formatTable[format_code];
	if (IsFormatIdxUsed(xls, crnt, index)) {
		//do_log(LOG_ALERT, "Format %x is redefined\n",index);
	  /* this format is something user-defined --- not a standard built-in date*/
	  return NULL;
	}
	dateindex=BuiltInDateFormatIdx(index);
	if (dateindex) {
	  if (crnt->forced_date_format) return crnt->forced_date_format;
		return GetBuiltInDateFormat(dateindex);
	} else
		return NULL;
}
static time_t float2date(do_xls_t *xls, crnt_t *crnt, double f)
{
	/* Hacked version. Excell stores date as floating point count of days
	 * since 1.1.1900. or 1.1.1904
	 * We are substracting value of 1.1.1970 and multiplying
	 * by 86400 thus getting seconds from the epoch
	 */
	return rint((f-crnt->date_shift)*86400);
}

static char *number2string(do_xls_t *xls, crnt_t *crnt, double d,short int format_code)
{
	static char buffer [128];
	char *datefmt;
	if ((datefmt=isDateFormat(xls, crnt, format_code))!=NULL) {
		time_t t = float2date(xls, crnt, d);
		strftime(buffer, 127,datefmt, gmtime(&t));
	} else {
		sprintf(buffer, crnt->number_format,d);
	}
	return buffer;
}

static char *format_double(do_xls_t *xls, crnt_t *crnt, char *rec,int offset,int format_code) {
	union { char cc[8];
		double d;} dconv;
	char *d,*s;
	int i;
# ifdef WORDS_BIGENDIAN
	for(s=rec+offset+8,d=dconv.cc,i=0;
			i<8;i++) *(d++)=*(--s);
# else
	for(s=rec+offset,d=dconv.cc,i=0;
			i<8;i++) *(d++)=*(s++);
# endif
	return number2string(xls, crnt, dconv.d,format_code);
}

static char *format_int(do_xls_t *xls, crnt_t *crnt, int value,int format_code) {
	static char buffer[12];
	sprintf(buffer,"%i",value);
	return buffer;
}
/*
 * Formats RK record
 */
double
gsf_le_get_double (void const *p)
{
	double data;
	memcpy ((char *)&data + 4, p, 4);
	memcpy ((char *)&data, (char const *)p + 4, 4);
	return data;
}

static char* format_rk(do_xls_t *xls, crnt_t *crnt, char *rec, short int format_code)
{
	uint32_t number;
	char buf[1024];
	enum eType {
		eIEEE = 0, eIEEEx100 = 1, eInt = 2, eIntx100 = 3
	} type;

	number = get_uint32(rec, 0);
	type = (number & 0x3);
	switch (type) {
	case eIEEE:
	case eIEEEx100:
	{
		//do_log(LOG_ERR, "unsupport rk");
		//return do_strdup("");
		uint8_t tmp[8];

		double answer;
		int lp;

		// Think carefully about big/little endian issues before
		//   changing this code.
		for ( lp = 0; lp < 4; lp++ ) {
			tmp[lp ]= (lp > 0) ?
                (((uint8_t*)rec)[lp]) :
                (((uint8_t*)rec)[lp] & 0xfc);
			tmp[lp + 4] = 0;
		}
        memcpy ((char *)&answer + 4, tmp, 4);
        memcpy ((char *)&answer, (char const *)tmp + 4, 4);
		return
            number2string(xls, crnt, (type == eIEEEx100 ? answer / 100. : answer), format_code);
	}
	case eInt:
        sprintf(buf, "%d", number >> 2);
		return do_strdup(buf);
	case eIntx100:
		number >>= 2;
		if ((number % 100) == 0) {
			sprintf(buf, "%d", number / 100);
            return do_strdup(buf);
		}
		else
			return number2string(xls, crnt, number / 100., format_code);
	}

	while (1) abort ();
}
/*
static char* format_rk(do_xls_t *xls, crnt_t *crnt, char *rec,short int format_code) {
	double value=0.0;
	int i;

	if ( *(rec) & 0x02 )
	{
		value=(double)(get_int32((char*)rec,0)>>2);
	}
	else {

		union { char cc[8];
			double d;} dconv;
		char *d,*s;
		for(i=0;i<8;i++)
			dconv.cc[i]='\0';
# ifdef WORDS_BIGENDIAN
		for(s=rec+4,d=dconv.cc,i=0; i<4;i++)
			*(d++)=*(--s);
		dconv.cc[0]=dconv.cc[0] & 0xfc;
# else
		for(s=rec,d=dconv.cc+4,i=0;
				i<4;i++) *(d++)=*(s++);
		dconv.cc[3]=dconv.cc[3] & 0xfc;
# endif
		value=dconv.d;
	}
	if ( *(rec) & 0x01 )
		value=value*0.01;
	return number2string(xls, crnt, value,format_code);
}
*/
static void SetCodePage(do_xls_t *xls, crnt_t *crnt, int code_page)
{
    const char *name;
    crnt->codepage = code_page;
    name = charset_from_codepage(code_page);
    if ( name[0] != '\0' ) {
        const char *str;
        if (xls->use_utf8)
            str = UTF_CHARSET;
        else
            str = get_locale_lang();

        crnt->cconv = iconv_open(str, name);
        if (crnt->cconv == (iconv_t)-1)
            do_log(LOG_ERR, "Error iconv_open (\"%s\" to \"%s\") ", name, str);
    }
}
