/*************************************************************************
**
** Definitions db struct for Domino 7.7 for btrive API
**
*************************************************************************/
#ifndef _DOMINO_FDB_H
#define _DOMINO_FDB_H

#include <btrieve.h>
#include <stdint.h>
#include <def.h>
typedef double BTI_REAL;
typedef int64_t BTI_CURRENCY;
#define BTI_CURRENCY_SHIFT 10000

/***************************************************************************
**
**  Definitions db documents
**
****************************************************************************/
/***************************************************************************
  Definition of record from 'document.dat' //
****************************************************************************/
#define DOMINO_FIELD_BCODE_SIZE 20
#define DOMINO_FIELD_CODE_SIZE 16
#define DOMINO_FIELD_PRODUCT_NAME_SIZE 150
#define DOMINO_FIELD_ACCOUNT 6
#define DOMINO_FIELD_SUBACCOUNT 20
#define DOMINO_FIELD_OTHER 20
#define DOMINO_FIELD_DEVISION 3
#define DOMINO_FIELD_PARTNER_GCODE 3
#if !defined(PACK_STRUCT)
#error Not defined PACK_STRUCT
#endif

  typedef struct PACK_STRUCT {
    BTI_CHAR dtype[2];
    BTI_CHAR dtype1[10];
    BTI_CHAR name[40];
    BTI_CHAR sklad[10];
    BTI_CHAR document[10];
    BTI_LONG date;
    BTI_LONG time;
    BTI_CHAR p_g_code[3];
    BTI_LONG p_code;
    BTI_CHAR p_doc[20];
    BTI_LONG p_date;
    BTI_LONG p_time;
    BTI_CHAR party[20];
    BTI_SINT accepted;
    BTI_CHAR params[4096];
  } document_struct_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR dtype[2];
    BTI_CHAR sklad[10];
    BTI_CHAR document[10];
  } document_key0_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR dtype[2];
    BTI_LONG date;
    BTI_LONG time;
  } document_key1_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR p_g_code[3];
    BTI_LONG p_code;
    BTI_CHAR party[20];
    BTI_LONG date;
    BTI_LONG time;
  } document_key2_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR dtype[2];
    BTI_CHAR sklad[10];
    BTI_LONG date;
    BTI_LONG time;
  } document_key3_t;
#ifdef DOMINO78
  typedef struct PACK_STRUCT {
    BTI_CHAR dtype[2];
    BTI_CHAR p_g_code[3];
    BTI_LONG p_code;
    BTI_LONG date;
    BTI_LONG time;
  } document_key4_t;
#endif
  typedef struct {
    BTI_WORD size;
    document_struct_t data;
  } document_rec_t;

/***************************************************************************
  Definition of record from 'd_order.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT{
    BTI_CHAR dtype[2];
    BTI_CHAR dtype1[10];
    BTI_CHAR sklad[10];
    BTI_CHAR document[10];
    BTI_LONG line;
    BTI_LONG sort;
    BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
    BTI_LONG date;
    BTI_LONG time;
    BTI_BYTE quant[4][7];
    BTI_SINT nquant[4];
    BTI_REAL price[8];
    BTI_SINT nprice[8];
    BTI_BYTE accepted;
    BTI_CHAR params[1024];
  } document_order_struct_t;
  typedef struct PACK_STRUCT{
    BTI_CHAR dtype[2];
    BTI_CHAR sklad[10];
    BTI_CHAR document[10];
    BTI_LONG line;
  } document_order_key0_t;
  typedef struct PACK_STRUCT{
    BTI_CHAR dtype[2];
    BTI_CHAR sklad[10];
    BTI_CHAR document[10];
    BTI_LONG sort;
  } document_order_key1_t;
  typedef struct PACK_STRUCT{
    BTI_CHAR dtype[2];
    BTI_CHAR sklad[10];
    BTI_CHAR document[10];
    BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
  } document_order_key2_t;
  typedef struct PACK_STRUCT{
    BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
    BTI_LONG date;
    BTI_LONG time;
  } document_order_key3_t;
  typedef struct PACK_STRUCT{
    BTI_CHAR sklad[10];
    BTI_LONG date;
    BTI_LONG time;
  } document_order_key4_t;
  typedef struct {
    BTI_WORD size;
    document_order_struct_t data;
  } document_order_rec_t;
/***************************************************************************
  Definition of record from 'd_link.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
    BTI_SINT type;
    BTI_CHAR m_dtype[2];
    BTI_CHAR m_sklad[10];
    BTI_CHAR m_document[10];
    BTI_CHAR s_dtype[2];
    BTI_CHAR s_sklad[10];
    BTI_CHAR s_document[10];
    BTI_LONG sort;
  } document_link_struct_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR m_dtype[2];
    BTI_CHAR m_sklad[10];
    BTI_CHAR m_document[10];
    BTI_SINT type;
    BTI_LONG  sort;
  } document_link_key0_t;
  typedef  struct {
	BTI_CHAR s_dtype[2];
	BTI_CHAR s_sklad[10];
	BTI_CHAR s_document[10];
	BTI_SINT type;
	BTI_LONG sort;
  } document_link_key1_t;
  typedef struct {
    BTI_WORD size;
    document_link_struct_t data;
  } document_link_rec_t;
/***************************************************************************
  Definition of record from 'enum.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT{
    BTI_CHAR  name[64];
    BTI_ULONG lastvalue;
    BTI_LONG  step;
    BTI_ULONG LowBound;
    BTI_ULONG hiBound;
    BTI_SINT  cycleFl;
    BTI_CHAR  format[128];
  } enum_struct_t;
  typedef struct PACK_STRUCT{
    BTI_CHAR  name[64];
  } enum_key0_t;
  typedef struct {
    BTI_WORD size;
    enum_struct_t data;
  } enum_rec_t;
/***************************************************************************
  Definition of record from 'd_view.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT{
	BTI_BYTE  type;
    BTI_CHAR  sort[40];
    BTI_CHAR  dtype[2];
    BTI_CHAR  sklad[10];
    BTI_CHAR  document[10];
    BTI_CHAR  dtype1[10];
  } document_view_struct_t;
  typedef struct PACK_STRUCT{
	BTI_BYTE  type;
    BTI_CHAR  sort[40];
    BTI_CHAR  dtype[2];
    BTI_CHAR  sklad[10];
    BTI_CHAR  document[10];
  } document_view_key0_t;
  typedef struct PACK_STRUCT{
    BTI_CHAR  dtype[2];
    BTI_CHAR  sklad[10];
    BTI_CHAR  document[10];
	BTI_BYTE  type;
  } document_view_key1_t;
  typedef struct {
    BTI_WORD size;
    document_view_struct_t data;
  } document_view_rec_t;
/***************************************************************************
  Definition of record from 'doc_data.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
    BTI_CHAR dtype[2];
    BTI_CHAR sklad[10];
    BTI_CHAR document[10];
    BTI_LONG type;
    BTI_LONG number;
#ifdef DOMINO78
    BTI_CHAR analitics[32];
#endif
    BTI_CHAR params[2048];
  } document_data_struct_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR dtype[2];
    BTI_CHAR sklad[10];
    BTI_CHAR document[10];
    BTI_LONG type;
    BTI_LONG number;
  } document_data_key0_t;
#ifdef DOMINO78
  typedef struct PACK_STRUCT {
    BTI_CHAR dtype[2];
    BTI_CHAR sklad[10];
    BTI_CHAR document[10];
    BTI_LONG type;
    BTI_CHAR analitics[32];
  } document_data_key1_t;
  typedef struct PACK_STRUCT {
    BTI_LONG type;
    BTI_CHAR analitics[32];
  } document_data_key2_t;
#endif
  typedef struct {
    BTI_WORD size;
    document_data_struct_t data;
  } document_data_rec_t;
/***************************************************************************
**
**  Definitions db product
**
****************************************************************************/
/***************************************************************************
  Definition of record from 'towar.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
    BTI_CHAR class_id[2];                           //0
    BTI_CHAR group_id[2];                           //2
    BTI_CHAR subgroup_id[2];                        //4
    BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];          //6
    BTI_CHAR name[DOMINO_FIELD_PRODUCT_NAME_SIZE];  //22
    BTI_CHAR article[20];                           //172
    BTI_CHAR variant[15];                           //192
    BTI_CHAR WES[3];                                //207
    BTI_CHAR unit[4];                               //210
    BTI_CHAR tech_unit[4];                          //214
    BTI_CHAR supplier_region[3];                    //218
    BTI_LONG supplier_code;                         //221
    BTI_CHAR mfr_region[3];                         //225
    BTI_LONG mfr_code;                              //228
    BTI_BYTE ptype;                                 //232
    BTI_REAL price[15];                             //233
    BTI_BYTE tech_unit_rate[4];                     //353
    BTI_LONG date;                                  //357
    BTI_LONG time;                                  //361
    BTI_LONG amor_date_mk;                          //365
    BTI_LONG amor_date_vd;                          //369
    BTI_CHAR amor_code[6];                          //373
    BTI_CHAR party[20];                             //379
    BTI_SINT base_parcel;                           //399
    BTI_BYTE price_filled;                          //401
    BTI_BYTE total_price;                           //402
    BTI_LONG life_time;                             //403
    BTI_BYTE nprice[15];                            //407
    BTI_CHAR params[2048];                          //422
  }product_struct_t;
  typedef struct PACK_STRUCT {
    BTI_SINT base_parcel;
    BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
  }product_key0_t;
  typedef struct PACK_STRUCT {
    BTI_SINT base_parcel;
    BTI_CHAR class_id[2];
    BTI_CHAR group_id[2];
    BTI_CHAR subgroup_id[2];
    BTI_CHAR name[20];
  }product_key1_t;
  typedef struct PACK_STRUCT {
    BTI_SINT base_parcel;
    BTI_CHAR article[20];
  }product_key2_t;
  typedef struct PACK_STRUCT {
    BTI_SINT base_parcel;
    BTI_CHAR supplier_region[3];
    BTI_LONG supplier_code;
    BTI_CHAR party[20];
  }product_key3_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
  }product_key4_t;
  typedef struct {
    BTI_WORD size;
    product_struct_t data;
  } product_rec_t;
/***************************************************************************
  Definition of record from 't_link.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
    BTI_SINT type;
    BTI_CHAR m_code[DOMINO_FIELD_CODE_SIZE];
    BTI_CHAR s_code[DOMINO_FIELD_CODE_SIZE];
    BTI_LONG number;
    BTI_REAL qty[4];
    BTI_CHAR params[256];
  } product_link_struct_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR m_code[DOMINO_FIELD_CODE_SIZE];
    BTI_SINT type;
    BTI_LONG number;
  } product_link_key0_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR s_code[DOMINO_FIELD_CODE_SIZE];
    BTI_SINT type;
  } product_link_key1_t;
  typedef struct {
    BTI_WORD size;
    product_link_struct_t data;
  } product_link_rec_t;
/***************************************************************************
  Definition of record from 't_view.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
    BTI_SINT type;
    BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
    BTI_CHAR barcode[DOMINO_FIELD_BCODE_SIZE];
    BTI_CHAR sklad[10];
    BTI_CHAR sort[40];
  } product_view_struct_t;
  typedef struct PACK_STRUCT {
    BTI_SINT type;
    BTI_CHAR sklad[10];
    BTI_CHAR sort[40];
    BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
  } product_view_key0_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
    BTI_CHAR sklad[10];
  } product_view_key1_t;
  typedef struct {
    BTI_WORD size;
    product_view_struct_t data;
  } product_view_rec_t;
/***************************************************************************
  Definition of record from 'tow_data.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
	BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
	BTI_LONG type;
	BTI_LONG number;
#ifdef DOMINO78
        BTI_CHAR analitics[32];
#endif
	BTI_CHAR params[2048];
  } product_data_struct_t;
  typedef struct PACK_STRUCT {
	BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
	BTI_LONG type;
	BTI_LONG number;
  } product_data_key0_t;
#ifdef DOMINO78
  typedef struct PACK_STRUCT {
	BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
	BTI_LONG type;
        BTI_CHAR analitics[32];
  } product_data_key1_t;
  typedef struct PACK_STRUCT {
	BTI_LONG type;
        BTI_CHAR analitics[32];
  } product_data_key2_t;
#endif
  typedef struct {
    BTI_WORD size;
    product_data_struct_t data;
  } product_data_rec_t;
/***************************************************************************
  Definition of record from 'barcodes.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
	BTI_CHAR barcode[DOMINO_FIELD_BCODE_SIZE];
	BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
	BTI_BYTE total[7];
	BTI_BYTE sk[3];
  } barcode_struct_t;
  typedef struct PACK_STRUCT {
	BTI_CHAR barcode[DOMINO_FIELD_BCODE_SIZE];
  } barcode_key0_t;
  typedef struct PACK_STRUCT {
	BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
  } barcode_key1_t;
  typedef struct {
    BTI_WORD size;
    barcode_struct_t data;
  } barcode_rec_t;
/***************************************************************************
**
**  Definitions handbooks
**
****************************************************************************/
/***************************************************************************
  Definition of record from 'account.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
	BTI_CHAR account[6];
	BTI_CHAR ticacc[3];
#ifdef DOMINO78
	BTI_CHAR name[128];
#else
	BTI_CHAR name[30];
#endif
	BTI_BYTE debit;
	BTI_BYTE credit;
	BTI_BYTE positivesaldo;
	BTI_BYTE itog;
	BTI_BYTE analitsub;
	BTI_BYTE analitpartner;
	BTI_BYTE analitother;
	BTI_CHAR othername[15];
	BTI_BYTE sumincl[3];
  } account_struct_t;
  typedef struct PACK_STRUCT {
	BTI_CHAR account[6];
	BTI_BYTE itog;
  } account_key0_t;
  typedef struct {
    BTI_WORD size;
    account_struct_t data;
  } account_rec_t;
/***************************************************************************
  Definition of record from 'subacc.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
	BTI_CHAR account[6];
	BTI_CHAR subaccount[20];
	BTI_CHAR ticacc[3];
#ifdef DOMINO78
	BTI_CHAR name[128];
#else
	BTI_CHAR name[30];
#endif
	BTI_BYTE debit;
	BTI_BYTE credit;
	BTI_BYTE positivesaldo;
	BTI_BYTE itog;
	BTI_BYTE sumincl[3];
  } subaccount_struct_t;
  typedef struct PACK_STRUCT {
	BTI_CHAR account[6];
	BTI_CHAR subaccount[20];
	BTI_BYTE itog;
  } subaccount_key0_t;
  typedef struct {
    BTI_WORD size;
    subaccount_struct_t data;
  } subaccount_rec_t;
/***************************************************************************
  Definition of record from 'klass.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
	BTI_CHAR class_id[2];
#ifdef DOMINO78
	BTI_CHAR name[128];
#else
	BTI_CHAR name[30];
#endif
  }class_struct_t;
  typedef struct PACK_STRUCT {
	BTI_CHAR class_id[2];
  }class_key0_t;
  typedef struct {
    BTI_WORD size;
    class_struct_t data;
  } class_rec_t;
/***************************************************************************
  Definition of record from 'grouppa.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
    BTI_CHAR group_id[2];
#ifdef DOMINO78
	BTI_CHAR name[128];
#else
    BTI_CHAR name[30];
#endif
    BTI_CHAR class_id[2];
  } group_struct_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR class_id[2];
    BTI_CHAR group_id[2];
  } group_key0_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR class_id[2];
    BTI_CHAR name[30];
  } group_key1_t;
  typedef struct {
    BTI_WORD size;
    group_struct_t data;
  } group_rec_t;
/***************************************************************************
  Definition of record from 'podgru.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
	BTI_CHAR group_id[2];
	BTI_CHAR subgroup_id[2];
#ifdef DOMINO78
	BTI_CHAR name[128];
#else
	BTI_CHAR name[30];
#endif
	BTI_CHAR class_id[2];
	BTI_LONG stat;
	BTI_CHAR params[1024];
  } subgroup_struct_t;
  typedef struct PACK_STRUCT {
	BTI_CHAR class_id[2];
	BTI_CHAR group_id[2];
	BTI_CHAR subgroup_id[2];
  } subgroup_key0_t;
  typedef struct PACK_STRUCT {
	BTI_CHAR class_id[2];
	BTI_CHAR group_id[2];
	BTI_CHAR name[30];
  } subgroup_key1_t;
  typedef struct {
    BTI_WORD size;
    subgroup_struct_t data;
  } subgroup_rec_t;
/***************************************************************************
  Definition of record from 'pgruppa.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
    BTI_CHAR code[3];
    BTI_CHAR name[30];
    BTI_LONG stat;
    BTI_CHAR params[4096];
  } region_struct_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR code[3];
  } region_key0_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR name[30];
  } region_key1_t;
  typedef struct {
    BTI_WORD size;
    region_struct_t data;
  } region_rec_t;
/***************************************************************************
**
**  Partner
**
****************************************************************************/
/***************************************************************************
  Definition of record from 'otdel.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
	BTI_CHAR code[3];
	BTI_CHAR name[30];
	BTI_CHAR params[2048];
  } otdel_struct_t;
  typedef struct PACK_STRUCT {
	BTI_CHAR code[3];
  } otdel_key0_t;
  typedef struct {
    BTI_WORD size;
    otdel_struct_t data;
  } otdel_rec_t;
/***************************************************************************
  Definition of record from 'partner.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
    BTI_CHAR g_code[3];
    BTI_LONG code;
    BTI_CHAR otdel[3];
    BTI_CHAR sklad[10];
    BTI_CHAR name[50];
    BTI_CHAR bank[100];
    BTI_CHAR MFO[8];
    BTI_CHAR SHET[40];
    BTI_CHAR ADRES[150];
    BTI_CHAR PLAT_PGR[2];
    BTI_LONG PLAT_PAR;
    BTI_CHAR KSHET[40];
    BTI_CHAR fio[150];
    BTI_BYTE SUM[5][8];
    BTI_CHAR codestr[10];
    BTI_SINT state;
    BTI_SINT type;
    BTI_CHAR Klass[3];
    BTI_CHAR Param[20];
    BTI_SINT Sum_N[5];
    BTI_CHAR params[2048];
  } partner_struct_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR g_code[3];
    BTI_LONG code;
  } partner_key0_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR g_code[3];
    BTI_CHAR name[50];
  } partner_key1_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR name[50];
  } partner_key2_t;
  typedef struct PACK_STRUCT {
    BTI_LONG code;
  } partner_key3_t;
  typedef struct {
    BTI_WORD size;
    partner_struct_t data;
  } partner_rec_t;
/***************************************************************************
  Definition of record from 'par_data.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
    BTI_CHAR g_code[3];
    BTI_LONG code;
    BTI_SINT type;
    BTI_LONG number;
#ifndef DOMINO78
    BTI_LONG date;
    BTI_LONG time;
#else
    BTI_CHAR analitics[32];
#endif
    BTI_CHAR params[8192];
  } partner_data_struct_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR g_code[3];
    BTI_LONG code;
    BTI_SINT type;
    BTI_LONG number;
  } partner_data_key0_t;
#ifdef DOMINO78
  typedef struct PACK_STRUCT {
    BTI_CHAR g_code[3];
    BTI_LONG code;
    BTI_SINT type;
    BTI_CHAR analitics[32];
  } partner_data_key1_t;
  typedef struct PACK_STRUCT {
    BTI_SINT type;
    BTI_CHAR analitics[32];
  } partner_data_key2_t;
#else
  typedef struct PACK_STRUCT {
    BTI_SINT type;
    BTI_CHAR g_code[3];
    BTI_LONG code;
  } partner_data_key1_t;
#endif
  typedef struct {
    BTI_WORD size;
    partner_data_struct_t data;
  } partner_data_rec_t;
/***************************************************************************
  Definition of record from 'p_view.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
    BTI_WORD type;
    BTI_CHAR sort[40];
    BTI_CHAR g_code[3];
    BTI_LONG code;
    BTI_WORD type_par;
  } partner_view_struct_t;
  typedef struct PACK_STRUCT {
    BTI_WORD type;
    BTI_CHAR sort[40];
    BTI_CHAR g_code[3];
    BTI_LONG code;
  } partner_view_key0_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR g_code[3];
    BTI_LONG code;
    BTI_WORD type;
  } partner_view_key1_t;
  typedef struct {
    BTI_WORD size;
    partner_view_struct_t data;
  } partner_view_rec_t;
/***************************************************************************
  Definition of record from 'sklad.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
	BTI_CHAR code[10];
	BTI_CHAR name[40];
	BTI_CHAR type_[2];
	BTI_CHAR p_g_code[3];
	BTI_LONG p_code;
	BTI_BYTE OTDEL[2];
	BTI_CHAR mol[40];
	BTI_CHAR mol1[40];
	BTI_CHAR PASSWD[10];
	BTI_LONG TOWAR;
	BTI_CHAR TOW_PREFIX[10];
	BTI_LONG P_ORDER;
	BTI_CHAR P_OPER[2];
	BTI_LONG reporting_period_date;
	BTI_CHAR P_REGION[3];
	BTI_LONG P_PARTNER;
	BTI_LONG R_ORDER;
	BTI_CHAR R_OPER[2];
	BTI_LONG R_DATA;
	BTI_CHAR R_REGION[3];
	BTI_LONG R_PARTNER;
	BTI_BYTE SUM[60][8];
	BTI_BYTE KOL[5][7];
	BTI_CHAR CEK[5];
	BTI_CHAR CONFIG_PR[10];
	BTI_LONG CONFIG_NUM[5];
	BTI_LONG stat;
	BTI_CHAR params[1024];
  } sklad_struct_t;
  typedef struct PACK_STRUCT {
	BTI_CHAR code[10];
  } sklad_key0_t;
  typedef struct {
    BTI_WORD size;
    sklad_struct_t data;
  } sklad_rec_t;
/***************************************************************************
**
**  project
**
****************************************************************************/
/***************************************************************************
  Definition of record from 'dprow.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
    BTI_CHAR dtype[2];
	BTI_CHAR dtype1[10];
	BTI_LONG number;
	BTI_CHAR params[4069];
  } document_prow_struct_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR dtype[2];
	BTI_CHAR dtype1[10];
	BTI_LONG number;
  } document_prow_key0_t;
  typedef struct {
    BTI_WORD size;
    document_prow_struct_t data;
  } document_prow_rec_t;
/***************************************************************************
  Definition of record from 'dtype.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
    BTI_CHAR dtype[2];
    BTI_CHAR dtype1[10];
    BTI_CHAR name[40];
    BTI_CHAR sname[8];
    BTI_CHAR prices[40];
    BTI_CHAR print[80];
    BTI_CHAR form[6];
    BTI_CHAR accept_cmd[40];
    BTI_CHAR deaccept_cmd[40];
    BTI_CHAR c_dtypes[40];
    BTI_CHAR d_param[40];
    BTI_CHAR params[4096];
  } document_type_struct_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR dtype[2];
    BTI_CHAR dtype1[10];
  } document_type_key0_t;
  typedef struct {
    BTI_WORD size;
    document_type_struct_t data;
  } document_type_rec_t;
/***************************************************************************
**
**  Definitions db stocks
**
****************************************************************************/
/***************************************************************************
  Definition of record from 'stock.dat' //
****************************************************************************/
#ifdef DOMINO78
 typedef struct PACK_STRUCT {
    BTI_CHAR sklad[10];
    BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
    BTI_CHAR code0[DOMINO_FIELD_CODE_SIZE];
    BTI_CHAR sklad0[10];
    BTI_BYTE quant[6][8];
    BTI_SINT nquant[6];
  } stock_struct_t;
#else
 typedef struct PACK_STRUCT {
    BTI_CHAR sklad[10];
    BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
    BTI_BYTE quant[6][8];
    BTI_SINT nquant[6];
  } stock_struct_t;
#endif
  typedef struct PACK_STRUCT {
    BTI_CHAR sklad[10];
    BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
  } stock_key0_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
  } stock_key1_t;
#ifdef DOMINO78
  typedef struct PACK_STRUCT {
    BTI_CHAR sklad0[10];
    BTI_CHAR code0[DOMINO_FIELD_CODE_SIZE];
  } stock_key2_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR code0[DOMINO_FIELD_CODE_SIZE];
  } stock_key3_t;
#endif 
  typedef struct {
    BTI_WORD size;
    stock_struct_t data;
  } stock_rec_t;
/***************************************************************************
**
**  Definitions saldo
**
****************************************************************************/
/***************************************************************************
  Definition of record from 'prowod.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
    BTI_CHAR division[3];
    BTI_LONG date;
    BTI_CHAR debit_account[6];
    BTI_CHAR debit_subaccount[20];
    BTI_CHAR debit_other[20];
    BTI_CHAR debit_p_g_code[3];
    BTI_LONG debit_p_code;
    BTI_CHAR credit_account[6];
    BTI_CHAR credit_subaccount[20];
    BTI_CHAR credit_other[20];
    BTI_CHAR credit_p_g_code[3];
    BTI_LONG credit_p_code;
    BTI_CHAR valuta[4];
    BTI_BYTE SUM[8];
    BTI_BYTE sums[3][8];
    BTI_CHAR type_doc[3];
    BTI_BYTE type_oper;
    BTI_CHAR oper[3];
    BTI_CHAR document[10];
    BTI_CHAR pach[10];
    BTI_CHAR params[1024];
  } prowod_struct_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR division[3];
    BTI_LONG date;
    BTI_CHAR type_doc[3];
    BTI_CHAR document[10];
  } prowod_key0_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR division[3];
    BTI_CHAR type_doc[3];
    BTI_LONG date;
    BTI_CHAR document[10];
  } prowod_key1_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR division[3];
    BTI_CHAR debit_account[6];
    BTI_CHAR debit_subaccount[20];
    BTI_CHAR debit_p_g_code[3];
    BTI_LONG debit_p_code;
    BTI_CHAR debit_other[20];
    BTI_LONG date;
  } prowod_key2_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR division[3];
    BTI_CHAR credit_account[6];
    BTI_CHAR credit_subaccount[20];
    BTI_CHAR credit_p_g_code[3];
    BTI_LONG credit_p_code;
    BTI_CHAR credit_other[20];
    BTI_LONG date;
  } prowod_key3_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR division[3];
    BTI_CHAR document[10];
    BTI_LONG date;
  } prowod_key4_t;
  typedef struct {
    BTI_WORD size;
    prowod_struct_t data;
  } prowod_rec_t;
/***************************************************************************
  Definition of record from 'prowod2.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
	BTI_CHAR  dtype[2];
	BTI_CHAR  sklad[10];
	BTI_CHAR  document[10];
	BTI_ULONG number;
	BTI_CHAR  otdel[10];
	BTI_LONG  date;
	BTI_LONG  time;
	BTI_ULONG saldo_d;
	BTI_ULONG saldo_k;
	BTI_SINT  nsum[5];
	BTI_BYTE  sums[5][8];
	BTI_CHAR  params[1024];
  } prowod2_struct_t;
  typedef struct PACK_STRUCT {
	BTI_CHAR  dtype[2];
	BTI_CHAR  sklad[10];
	BTI_CHAR  document[10];
	BTI_ULONG number;
	BTI_CHAR  otdel[10];
  } prowod2_key0_t;
  typedef struct PACK_STRUCT {
	BTI_CHAR  otdel[10];
	BTI_LONG  date;
	BTI_LONG  time;
  } prowod2_key1_t;
  typedef struct PACK_STRUCT {
	BTI_CHAR  otdel[10];
	BTI_ULONG saldo_d;
	BTI_LONG  date;
	BTI_LONG  time;
  } prowod2_key2_t;
  typedef struct PACK_STRUCT {
	BTI_CHAR  otdel[10];
	BTI_ULONG saldo_k;
	BTI_LONG  date;
	BTI_LONG  time;
  } prowod2_key3_t;
  typedef struct {
    BTI_WORD size;
    prowod2_struct_t data;
  } prowod2_rec_t;
/***************************************************************************
  Definition of record from 'prowod2.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
	BTI_CHAR division[3];
	BTI_CHAR layer[3];
	BTI_CHAR account[6];
	BTI_BYTE account_total;
	BTI_CHAR subaccount[20];
	BTI_BYTE subaccount_total;
	BTI_CHAR p_g_code[3];
	BTI_LONG p_code;
	BTI_CHAR other[20];
	BTI_LONG date_n;
	BTI_LONG date;
	BTI_LONG date_modify;
	BTI_LONG time_modify;
	BTI_BYTE debit_sum[3][8];
	BTI_BYTE credit_sum[3][8];
  } saldo_struct_t;
  typedef struct PACK_STRUCT {
	BTI_CHAR division[3];
	BTI_CHAR layer[3];
	BTI_CHAR account[6];
	BTI_BYTE account_total;
	BTI_CHAR subaccount[20];
	BTI_BYTE subaccount_total;
	BTI_CHAR p_g_code[3];
	BTI_LONG p_code;
	BTI_CHAR other[20];
  } saldo_key0_t;
  typedef struct {
    BTI_WORD size;
    saldo_struct_t data;
  } saldo_rec_t;
/***************************************************************************
**
**  Definitions protocol
**
****************************************************************************/
/***************************************************************************
  Definition of record from 'protocol.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
#ifdef DOMINO78
    BTI_CHAR objectName[64];
#else
    BTI_CHAR objectName[24];
#endif

    BTI_CHAR userName[20];
    BTI_LONG date;
    BTI_LONG time;
    BTI_CHAR action[32];
  } protocol_struct_t;
  typedef struct PACK_STRUCT {
#ifdef DOMINO78
    BTI_CHAR objectName[64];
#else
    BTI_CHAR objectName[24];
#endif
    BTI_LONG date;
    BTI_LONG time;
  } protocol_key0_t;
  typedef struct PACK_STRUCT {
    BTI_LONG date;
    BTI_LONG time;
  } protocol_key1_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR userName[20];
    BTI_LONG date;
    BTI_LONG time;
  } protocol_key2_t;
  typedef struct {
    BTI_WORD size;
    protocol_struct_t data;
  } protocol_rec_t;
/***************************************************************************
**
**  misc
**
****************************************************************************/
/***************************************************************************
  Definition of record from 'user.dat' //
****************************************************************************/
#define MAXGROUP 255
  typedef struct PACK_STRUCT {
    BTI_CHAR type;
    BTI_CHAR name[64];
    BTI_CHAR pwd[32];
    BTI_SINT supervisore;
    BTI_CHAR userini[80];
    BTI_CHAR groups[MAXGROUP];
    BTI_CHAR params[32768];
  } user_struct_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR type;
    BTI_CHAR name[64];
  } user_key0_t;
  typedef struct {
    BTI_WORD size;
    user_struct_t data;
  } user_rec_t;

#ifndef DOMINO78
/***************************************************************************
  Definition of record from 'ushift.dat' //
****************************************************************************/
#ifndef USE_OLD_CHECK
  typedef struct PACK_STRUCT {
    BTI_SINT cash;   // 2 // 1
    BTI_SINT shift;  // 4 // 3
    BTI_BYTE null1;  // 5 // 5
    BTI_LONG date;   // 9 // 6
    BTI_BYTE null2;  //10 // 10
    BTI_LONG time;   // 14 // 11
    BTI_BYTE null3;  //15  // 15
    BTI_LONG totalCheck; //19 // 16
    BTI_BYTE null4;  //20  // 20
    BTI_REAL sale_sum;    // 28 //21
    BTI_BYTE null5;  //29  // 29
    BTI_LONG date_b; //33  // 30
    BTI_BYTE null6;   //34 // 34
    BTI_LONG time_b;  // 38 // 31
    BTI_BYTE null7;   // 39 // 35
    BTI_CHAR sklad[10]; // 49 // 36
  } shift_struct_t;
  typedef struct PACK_STRUCT {
    BTI_SINT cash;
    BTI_SINT shift;
  } shift_key0_t;
  typedef struct PACK_STRUCT {
    BTI_BYTE null1;
    BTI_LONG date;
    BTI_BYTE null2;
    BTI_LONG time;
  } shift_key1_t;
  typedef struct PACK_STRUCT {
    BTI_BYTE null1;
    BTI_LONG date_b;
    BTI_BYTE null2;
    BTI_LONG time_b;
  } shift_key2_t;
  typedef struct PACK_STRUCT {
    BTI_BYTE null1;
    BTI_CHAR sklad[10];
    BTI_BYTE null2;
    BTI_LONG date;
    BTI_BYTE null3;
    BTI_LONG time;
  } shift_key3_t;
  typedef struct {
    BTI_WORD size;
    shift_struct_t data;
  } shift_rec_t;
/***************************************************************************
  Definition of record from 'shift.dat' //
****************************************************************************/
#else
  typedef struct PACK_STRUCT {
	BTI_SINT cash;
	BTI_SINT shift;
	BTI_BYTE null1;
	BTI_LONG date;
	BTI_BYTE null2;
	BTI_LONG time;
	BTI_BYTE null3;
	BTI_LONG totalCheck;
	BTI_BYTE null4;
	BTI_REAL sale_sum;
  } shift_struct_t;
  typedef struct PACK_STRUCT {
	BTI_SINT cash;
	BTI_SINT shift;
    BTI_BYTE null1;
	BTI_LONG date;
	BTI_BYTE null2;
	BTI_LONG time;
  } shift_key0_t;
  typedef struct PACK_STRUCT {
	BTI_BYTE null1;
	BTI_LONG date;
	BTI_BYTE null2;
	BTI_LONG time;
  } shift_key1_t;
  typedef struct {
    BTI_WORD size;
    shift_struct_t data;
  } shift_rec_t;
#endif
/***************************************************************************
  Definition of record from 'checksums.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
    BTI_BYTE null1;
    BTI_SINT cash;
    BTI_BYTE null2;
    BTI_SINT shift;
    BTI_BYTE null3;
    BTI_SINT check;
    BTI_BYTE null4;
    BTI_LONG date;
    BTI_BYTE null5;
    BTI_LONG time;
    BTI_BYTE null6;
    BTI_SINT cashier;
    BTI_BYTE null7;
    BTI_SINT valuta;
    BTI_BYTE null8;
    BTI_CHAR operation[5];
    BTI_BYTE null9;
    BTI_REAL sale_sum;
    BTI_BYTE null10;
    BTI_SINT total;
  } checksum_struct_t;
  typedef struct PACK_STRUCT {
    BTI_BYTE null1;
    BTI_SINT cash;
    BTI_BYTE null2;
    BTI_SINT shift;
    BTI_BYTE null4;
    BTI_LONG date;
    BTI_BYTE null5;
    BTI_LONG time;
    BTI_BYTE null3;
    BTI_SINT check;
  } checksum_key0_t;
  typedef struct PACK_STRUCT {
    BTI_BYTE null4;
    BTI_LONG date;
    BTI_BYTE null5;
    BTI_LONG time;
    BTI_BYTE null1;
    BTI_SINT cash;
  } checksum_key1_t;
  typedef struct {
    BTI_WORD size;
    checksum_struct_t data;
  } checksum_rec_t;
/***************************************************************************
  Definition of record from 'check.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
	BTI_SINT cash;
	BTI_BYTE null1;
    BTI_SINT shift;
    BTI_BYTE null2;
    BTI_SINT check;
    BTI_BYTE null3;
    BTI_SINT position;
    BTI_BYTE null4;
    BTI_LONG date;
    BTI_BYTE null5;
    BTI_LONG time;
    BTI_BYTE null6;
    BTI_REAL sale_price;
    BTI_BYTE null7;
    BTI_REAL quantity;
    BTI_BYTE null8;
    BTI_CHAR product_code[DOMINO_FIELD_CODE_SIZE];
    BTI_BYTE null9;
    BTI_CHAR bar_code[DOMINO_FIELD_BCODE_SIZE];
    BTI_BYTE null10;
    BTI_SINT cashier;
    BTI_BYTE null11;
    BTI_SINT valuta;
    BTI_BYTE null12;
    BTI_CHAR operation[5];
  } check_struct_t;
  typedef struct PACK_STRUCT {
	BTI_SINT cash;
	BTI_BYTE null1;
    BTI_SINT shift;
    BTI_BYTE null2;
    BTI_SINT check;
    BTI_BYTE null3;
    BTI_SINT position;
    BTI_BYTE null4;
    BTI_LONG date;
    BTI_BYTE null5;
    BTI_LONG time;
  } check_key0_t;
  typedef struct PACK_STRUCT {
    BTI_BYTE null1;
    BTI_LONG date;
    BTI_BYTE null2;
    BTI_LONG time;
  } check_key1_t;
  typedef struct PACK_STRUCT {
    BTI_BYTE null1;
    BTI_CHAR product_code[DOMINO_FIELD_CODE_SIZE];
    BTI_BYTE null2;
    BTI_LONG date;
    BTI_BYTE null3;
    BTI_LONG time;
	BTI_SINT cash;
	BTI_BYTE null4;
    BTI_SINT shift;
    BTI_BYTE null5;
    BTI_SINT check;
  } check_key2_t;
  typedef struct {
    BTI_WORD size;
    check_struct_t data;
  } check_rec_t;
/***************************************************************************
  Definition of record from 'realiz.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
    BTI_CHAR sklad[10];
    BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
    BTI_LONG month;
    BTI_REAL total;
    BTI_CURRENCY sale_sum;
    BTI_CURRENCY profit;
  } realization_struct_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
    BTI_LONG month;
    BTI_CHAR sklad[10];
  } realization_key0_t;
  typedef struct PACK_STRUCT {
    BTI_CHAR sklad[10];
    BTI_LONG month;
    BTI_CHAR code[16];
  } realization_key1_t;
  typedef struct PACK_STRUCT {
    BTI_LONG month;
    BTI_CHAR code[16];
    BTI_CHAR sklad[10];
  } realization_key2_t;
  typedef struct PACK_STRUCT {
    BTI_LONG month;
    BTI_CHAR sklad[10];
    BTI_CHAR code[16];
  } realization_key3_t;
  typedef struct PACK_STRUCT {
	BTI_CHAR code[16];
    BTI_CHAR sklad[10];
    BTI_LONG month;
  } realization_key4_t;
  typedef struct {
    BTI_WORD size;
    realization_struct_t data;
  } realization_rec_t;
/***************************************************************************
  Definition of record from 'discount.dat' //
****************************************************************************/
  typedef struct PACK_STRUCT {
    BTI_LONG card;
    BTI_SINT cash;
    BTI_SINT shift;
    BTI_SINT check;
    BTI_LONG date;
    BTI_LONG time;
    BTI_REAL sale_sum;
    BTI_REAL discount;
  } discount_struct_t;
  typedef struct PACK_STRUCT {
    BTI_SINT cash;
    BTI_SINT shift;
    BTI_LONG date;
    BTI_SINT check;
    BTI_LONG time;
  } discount_key0_t;
  typedef struct PACK_STRUCT {
    BTI_LONG card;
    BTI_LONG date;
    BTI_LONG time;
  } discount_key1_t;
  typedef struct {
    BTI_WORD size;
    discount_struct_t data;
  } discount_rec_t;
#endif
typedef struct PACK_STRUCT {
    BTI_WORD id;
    BTI_CHAR name[20];
    BTI_CHAR location[64];
    BTI_CHAR other[11];
} struct_file_struct_t;

typedef struct PACK_STRUCT {
    BTI_WORD id;
} struct_file_key0_t;

typedef struct PACK_STRUCT {
    BTI_CHAR name[20];
} struct_file_key1_t;

typedef struct {
    BTI_WORD size;
    struct_file_struct_t data;
} struct_file_rec_t;


typedef enum {
    DO_DB_DOCUMENT,
    DO_DB_DOCUMENT_ORDER,
    DO_DB_DOCUMENT_LINK,
    DO_DB_DOCUMENT_VIEW,
    DO_DB_DOCUMENT_DATA,
    DO_DB_DOCUMENT_PROW,
    DO_DB_DOCUMENT_TYPE,
    DO_DB_ENUM,
    DO_DB_PRODUCT,
    DO_DB_PRODUCT_LINK,
    DO_DB_PRODUCT_VIEW,
    DO_DB_PRODUCT_DATA,
    DO_DB_BARCODE,
    DO_DB_ACCOUNT,
    DO_DB_SUBACCOUNT,
    DO_DB_CLASS,
    DO_DB_GROUP,
    DO_DB_SUBGROUP,
    DO_DB_REGION,
    DO_DB_OTDEL,
    DO_DB_PARTNER,
#ifdef USE_DB_PARNER_VIEW
    DO_DB_PARTNER_VIEW,
#endif
    DO_DB_PARTNER_DATA,
    DO_DB_SKLAD,
    DO_DB_STOCK,
    DO_DB_PROWOD,
    DO_DB_PROWOD2,
    DO_DB_SALDO,
    DO_DB_PROTOCOL,
    DO_DB_USER,
#ifndef DOMINO78
    DO_DB_SHIFT,
    DO_DB_CHECKSUM,
    DO_DB_CHECK,
    DO_DB_REALIZATION,
    DO_DB_DISCOUNT,
#endif
    DO_DB_STRUCT_FILE,
    DO_DB_END // ALL_DB must be last item
} file_db_t;

#endif

