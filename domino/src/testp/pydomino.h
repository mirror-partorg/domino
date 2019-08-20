#ifndef PYDOMINO_H_INCLUDED
#define PYDOMINO_H_INCLUDED

#include <Python.h>
#include <domino.h>

typedef struct {
    PyObject_HEAD
    do_alias_t *alias;
} Alias;
typedef struct {
	PyObject_HEAD
	document_key0_t *priv;
	Alias *alias;
} DocumentKey0;
typedef struct {
	PyObject_HEAD
	document_key1_t *priv;
	Alias *alias;
} DocumentKey1;
typedef struct {
	PyObject_HEAD
	document_key2_t *priv;
	Alias *alias;
} DocumentKey2;
typedef struct {
	PyObject_HEAD
	document_key3_t *priv;
	Alias *alias;
} DocumentKey3;
typedef struct {
	PyObject_HEAD
	document_rec_t *priv;
	Alias *alias;
} Document;
typedef struct {
	PyObject_HEAD
	document_order_key0_t *priv;
	Alias *alias;
} DocumentOrderKey0;
typedef struct {
	PyObject_HEAD
	document_order_key1_t *priv;
	Alias *alias;
} DocumentOrderKey1;
typedef struct {
	PyObject_HEAD
	document_order_key2_t *priv;
	Alias *alias;
} DocumentOrderKey2;
typedef struct {
	PyObject_HEAD
	document_order_key3_t *priv;
	Alias *alias;
} DocumentOrderKey3;
typedef struct {
	PyObject_HEAD
	document_order_key4_t *priv;
	Alias *alias;
} DocumentOrderKey4;
typedef struct {
	PyObject_HEAD
	document_order_rec_t *priv;
	Alias *alias;
} DocumentOrder;
typedef struct {
	PyObject_HEAD
	document_link_key0_t *priv;
	Alias *alias;
} DocumentLinkKey0;
typedef struct {
	PyObject_HEAD
	document_link_key1_t *priv;
	Alias *alias;
} DocumentLinkKey1;
typedef struct {
	PyObject_HEAD
	document_link_rec_t *priv;
	Alias *alias;
} DocumentLink;
typedef struct {
	PyObject_HEAD
	enum_key0_t *priv;
	Alias *alias;
} EnumKey0;
typedef struct {
	PyObject_HEAD
	enum_rec_t *priv;
	Alias *alias;
} Enum;
typedef struct {
	PyObject_HEAD
	document_view_key0_t *priv;
	Alias *alias;
} DocumentViewKey0;
typedef struct {
	PyObject_HEAD
	document_view_key1_t *priv;
	Alias *alias;
} DocumentViewKey1;
typedef struct {
	PyObject_HEAD
	document_view_rec_t *priv;
	Alias *alias;
} DocumentView;
typedef struct {
	PyObject_HEAD
	document_data_key0_t *priv;
	Alias *alias;
} DocumentDataKey0;
typedef struct {
	PyObject_HEAD
	document_data_rec_t *priv;
	Alias *alias;
} DocumentData;
typedef struct {
	PyObject_HEAD
	product_rec_t *priv;
	Alias *alias;
} Product;
typedef struct {
	PyObject_HEAD
	product_link_key0_t *priv;
	Alias *alias;
} ProductLinkKey0;
typedef struct {
	PyObject_HEAD
	product_link_key1_t *priv;
	Alias *alias;
} ProductLinkKey1;
typedef struct {
	PyObject_HEAD
	product_link_rec_t *priv;
	Alias *alias;
} ProductLink;
typedef struct {
	PyObject_HEAD
	product_view_key0_t *priv;
	Alias *alias;
} ProductViewKey0;
typedef struct {
	PyObject_HEAD
	product_view_key1_t *priv;
	Alias *alias;
} ProductViewKey1;
typedef struct {
	PyObject_HEAD
	product_view_rec_t *priv;
	Alias *alias;
} ProductView;
typedef struct {
	PyObject_HEAD
	product_data_key0_t *priv;
	Alias *alias;
} ProductDataKey0;
typedef struct {
	PyObject_HEAD
	product_data_rec_t *priv;
	Alias *alias;
} ProductData;
typedef struct {
	PyObject_HEAD
	barcode_key0_t *priv;
	Alias *alias;
} BarcodeKey0;
typedef struct {
	PyObject_HEAD
	barcode_key1_t *priv;
	Alias *alias;
} BarcodeKey1;
typedef struct {
	PyObject_HEAD
	barcode_rec_t *priv;
	Alias *alias;
} Barcode;
typedef struct {
	PyObject_HEAD
	account_key0_t *priv;
	Alias *alias;
} AccountKey0;
typedef struct {
	PyObject_HEAD
	account_rec_t *priv;
	Alias *alias;
} Account;
typedef struct {
	PyObject_HEAD
	subaccount_key0_t *priv;
	Alias *alias;
} SubaccountKey0;
typedef struct {
	PyObject_HEAD
	subaccount_rec_t *priv;
	Alias *alias;
} Subaccount;
typedef struct {
	PyObject_HEAD
	class_rec_t *priv;
	Alias *alias;
} Class;
typedef struct {
	PyObject_HEAD
	group_key0_t *priv;
	Alias *alias;
} GroupKey0;
typedef struct {
	PyObject_HEAD
	group_key1_t *priv;
	Alias *alias;
} GroupKey1;
typedef struct {
	PyObject_HEAD
	group_rec_t *priv;
	Alias *alias;
} Group;
typedef struct {
	PyObject_HEAD
	subgroup_key0_t *priv;
	Alias *alias;
} SubgroupKey0;
typedef struct {
	PyObject_HEAD
	subgroup_key1_t *priv;
	Alias *alias;
} SubgroupKey1;
typedef struct {
	PyObject_HEAD
	subgroup_rec_t *priv;
	Alias *alias;
} Subgroup;
typedef struct {
	PyObject_HEAD
	region_key0_t *priv;
	Alias *alias;
} RegionKey0;
typedef struct {
	PyObject_HEAD
	region_key1_t *priv;
	Alias *alias;
} RegionKey1;
typedef struct {
	PyObject_HEAD
	region_rec_t *priv;
	Alias *alias;
} Region;
typedef struct {
	PyObject_HEAD
	otdel_key0_t *priv;
	Alias *alias;
} OtdelKey0;
typedef struct {
	PyObject_HEAD
	otdel_rec_t *priv;
	Alias *alias;
} Otdel;
typedef struct {
	PyObject_HEAD
	partner_key0_t *priv;
	Alias *alias;
} PartnerKey0;
typedef struct {
	PyObject_HEAD
	partner_key1_t *priv;
	Alias *alias;
} PartnerKey1;
typedef struct {
	PyObject_HEAD
	partner_key2_t *priv;
	Alias *alias;
} PartnerKey2;
typedef struct {
	PyObject_HEAD
	partner_key3_t *priv;
	Alias *alias;
} PartnerKey3;
typedef struct {
	PyObject_HEAD
	partner_rec_t *priv;
	Alias *alias;
} Partner;
typedef struct {
	PyObject_HEAD
	partner_data_key0_t *priv;
	Alias *alias;
} PartnerDataKey0;
typedef struct {
	PyObject_HEAD
	partner_data_key1_t *priv;
	Alias *alias;
} PartnerDataKey1;
typedef struct {
	PyObject_HEAD
	partner_data_rec_t *priv;
	Alias *alias;
} PartnerData;
typedef struct {
	PyObject_HEAD
	partner_view_key0_t *priv;
	Alias *alias;
} PartnerViewKey0;
typedef struct {
	PyObject_HEAD
	partner_view_key1_t *priv;
	Alias *alias;
} PartnerViewKey1;
typedef struct {
	PyObject_HEAD
	partner_view_rec_t *priv;
	Alias *alias;
} PartnerView;
typedef struct {
	PyObject_HEAD
	sklad_key0_t *priv;
	Alias *alias;
} SkladKey0;
typedef struct {
	PyObject_HEAD
	sklad_rec_t *priv;
	Alias *alias;
} Sklad;
typedef struct {
	PyObject_HEAD
	document_prow_key0_t *priv;
	Alias *alias;
} DocumentProwKey0;
typedef struct {
	PyObject_HEAD
	document_prow_rec_t *priv;
	Alias *alias;
} DocumentProw;
typedef struct {
	PyObject_HEAD
	document_type_key0_t *priv;
	Alias *alias;
} DocumentTypeKey0;
typedef struct {
	PyObject_HEAD
	document_type_rec_t *priv;
	Alias *alias;
} DocumentType;
typedef struct {
	PyObject_HEAD
	stock_key0_t *priv;
	Alias *alias;
} StockKey0;
typedef struct {
	PyObject_HEAD
	stock_key1_t *priv;
	Alias *alias;
} StockKey1;
typedef struct {
	PyObject_HEAD
	stock_rec_t *priv;
	Alias *alias;
} Stock;
typedef struct {
	PyObject_HEAD
	prowod_key0_t *priv;
	Alias *alias;
} ProwodKey0;
typedef struct {
	PyObject_HEAD
	prowod_key1_t *priv;
	Alias *alias;
} ProwodKey1;
typedef struct {
	PyObject_HEAD
	prowod_key2_t *priv;
	Alias *alias;
} ProwodKey2;
typedef struct {
	PyObject_HEAD
	prowod_key3_t *priv;
	Alias *alias;
} ProwodKey3;
typedef struct {
	PyObject_HEAD
	prowod_key4_t *priv;
	Alias *alias;
} ProwodKey4;
typedef struct {
	PyObject_HEAD
	prowod_rec_t *priv;
	Alias *alias;
} Prowod;
typedef struct {
	PyObject_HEAD
	prowod2_key0_t *priv;
	Alias *alias;
} Prowod2Key0;
typedef struct {
	PyObject_HEAD
	prowod2_key1_t *priv;
	Alias *alias;
} Prowod2Key1;
typedef struct {
	PyObject_HEAD
	prowod2_key2_t *priv;
	Alias *alias;
} Prowod2Key2;
typedef struct {
	PyObject_HEAD
	prowod2_key3_t *priv;
	Alias *alias;
} Prowod2Key3;
typedef struct {
	PyObject_HEAD
	prowod2_rec_t *priv;
	Alias *alias;
} Prowod2;
typedef struct {
	PyObject_HEAD
	saldo_key0_t *priv;
	Alias *alias;
} SaldoKey0;
typedef struct {
	PyObject_HEAD
	saldo_rec_t *priv;
	Alias *alias;
} Saldo;
typedef struct {
	PyObject_HEAD
	protocol_key0_t *priv;
	Alias *alias;
} ProtocolKey0;
typedef struct {
	PyObject_HEAD
	protocol_key1_t *priv;
	Alias *alias;
} ProtocolKey1;
typedef struct {
	PyObject_HEAD
	protocol_key2_t *priv;
	Alias *alias;
} ProtocolKey2;
typedef struct {
	PyObject_HEAD
	protocol_rec_t *priv;
	Alias *alias;
} Protocol;
typedef struct {
	PyObject_HEAD
	user_key0_t *priv;
	Alias *alias;
} UserKey0;
typedef struct {
	PyObject_HEAD
	user_rec_t *priv;
	Alias *alias;
} User;
typedef struct {
	PyObject_HEAD
	shift_key0_t *priv;
	Alias *alias;
} ShiftKey0;
typedef struct {
	PyObject_HEAD
	shift_key1_t *priv;
	Alias *alias;
} ShiftKey1;
typedef struct {
	PyObject_HEAD
	shift_key2_t *priv;
	Alias *alias;
} ShiftKey2;
typedef struct {
	PyObject_HEAD
	shift_key3_t *priv;
	Alias *alias;
} ShiftKey3;
typedef struct {
	PyObject_HEAD
	shift_rec_t *priv;
	Alias *alias;
} Shift;
typedef struct {
	PyObject_HEAD
	checksum_key0_t *priv;
	Alias *alias;
} ChecksumKey0;
typedef struct {
	PyObject_HEAD
	checksum_key1_t *priv;
	Alias *alias;
} ChecksumKey1;
typedef struct {
	PyObject_HEAD
	checksum_rec_t *priv;
	Alias *alias;
} Checksum;
typedef struct {
	PyObject_HEAD
	check_key0_t *priv;
	Alias *alias;
} CheckKey0;
typedef struct {
	PyObject_HEAD
	check_key1_t *priv;
	Alias *alias;
} CheckKey1;
typedef struct {
	PyObject_HEAD
	check_key2_t *priv;
	Alias *alias;
} CheckKey2;
typedef struct {
	PyObject_HEAD
	check_rec_t *priv;
	Alias *alias;
} Check;
typedef struct {
	PyObject_HEAD
	realization_key0_t *priv;
	Alias *alias;
} RealizationKey0;
typedef struct {
	PyObject_HEAD
	realization_key1_t *priv;
	Alias *alias;
} RealizationKey1;
typedef struct {
	PyObject_HEAD
	realization_key2_t *priv;
	Alias *alias;
} RealizationKey2;
typedef struct {
	PyObject_HEAD
	realization_key3_t *priv;
	Alias *alias;
} RealizationKey3;
typedef struct {
	PyObject_HEAD
	realization_key4_t *priv;
	Alias *alias;
} RealizationKey4;
typedef struct {
	PyObject_HEAD
	realization_rec_t *priv;
	Alias *alias;
} Realization;
typedef struct {
	PyObject_HEAD
	discount_key0_t *priv;
	Alias *alias;
} DiscountKey0;
typedef struct {
	PyObject_HEAD
	discount_key1_t *priv;
	Alias *alias;
} DiscountKey1;
typedef struct {
	PyObject_HEAD
	discount_rec_t *priv;
	Alias *alias;
} Discount;
typedef struct {
	PyObject_HEAD
	struct_file_key0_t *priv;
	Alias *alias;
} StructFileKey0;
typedef struct {
	PyObject_HEAD
	struct_file_key1_t *priv;
	Alias *alias;
} StructFileKey1;
typedef struct {
	PyObject_HEAD
	struct_file_rec_t *priv;
	Alias *alias;
} StructFile;

#endif // PYDOMINO_H_INCLUDED
