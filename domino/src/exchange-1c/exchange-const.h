#ifndef EXCHANGE_CONST_H_INCLUDED
#define EXCHANGE_CONST_H_INCLUDED

#define CONST_1C_DATE_CREATE "2012-05-28T16:50:10"

#define DOMINO_DATE_20190101 79627


//!!
#define firm_guid_number 1


#define ELEM_1C_ROOT "ФайлОбмена"

#define ELEM_1C_OBJECT "Объект"
#define ELEM_1C_TABLE "ТабличнаяЧасть"
#define ELEM_1C_SET "НаборЗаписей"
#define ELEM_1C_RECORD "Запись"
#define ELEM_1C_VALUE "Значение"
#define ELEM_1C_PROPERTY "Свойство"
#define ELEM_1C_LINK "Ссылка"
#define ELEM_1C_RULE "Правило"
#define ELEM_1C_RULES "ПравилаОбмена"
#define ELEM_1C_RULE_NAME "ИмяПравила"
#define ELEM_1C_EMPTY "Пусто"
#define ELEM_1C_ID "Ид"
#define ELEM_1C_NAME "Наименование"
#define ELEM_1C_DATE_CREATED "ДатаВремяСоздания"
#define ELEM_1C_SOURCE "Источник"
#define ELEM_1C_DEST "Приемник"
#define ELEM_1C_PARAMS "Параметры"
#define ELEM_1C_RUNTIME "Обработки"
#define ELEM_1C_RULES_CONVERT "ПравилаКонвертацииОбъектов"
#define ELEM_1C_FIND_FIELD_ORDER "ПоследовательностьПолейПоиска"
#define ELEM_1C_SYNC_FOR_ID "СинхронизироватьПоИдентификатору"

#define ELEM_1C_TYPE "Тип"
#define ELEM_1C_NAME_S "Имя"
#define ELEM_1C_NPP "Нпп"


#define ELEM_1C_GUID "{УникальныйИдентификатор}"
#define ELEM_1C_CONST_NAME "{ИмяПредопределенногоЭлемента}"
#define ELEM_1C_CODE "Код"
#define ELEM_1C_OWNER "Владелец"
#define ELEM_1C_GROUP "ЭтоГруппа"
#define ELEM_1C_NAME  "Наименование"
#define ELEM_1C_INN "ИНН"
#define ELEM_1C_FIRM "Организация"
#define ELEM_1C_KPP "КПП"
#define ELEM_1C_UNIT "Склад"
#define ELEM_1C_NAME_FULL "НаименованиеПолное"
#define ELEM_1C_DEFAULT_ACCOUNT "ОсновнойБанковскийСчет"
#define ELEM_1C_DEFAULT_CONTRACT "ОсновнойДоговорКонтрагента"
#define ELEM_1C_ACCOUNT_NUMBER "НомерСчета"
#define ELEM_1C_NUMBER "Номер"
#define ELEM_1C_UNIT_KIND "ВидСклада"
#define ELEM_1C_DOC_VALUTA "ВалютаДокумента"
#define ELEM_1C_DATE "Дата"
#define ELEM_1C_DOC_SOURCE_DATE "ДатаВходящегоДокумента"
#define ELEM_1C_DOC_SUM "СуммаДокумента"
#define ELEM_1C_DOC_SOURCE_NUMBER "НомерВходящегоДокумента"
#define ELEM_1C_CONSIGNER "Грузоотправитель"
#define ELEM_1C_CONSIGNEE "Грузополучатель"
#define ELEM_1C_INPUT_INVOICE_DATE "ДатаВходящегоСчетаФактуры"
#define ELEM_1C_INPUT_INVOICE_NUMBER "НомерВходящегоСчетаФактуры"
#define ELEM_1C_PARTNER_CONTRACT "ДоговорКонтрагента"
#define ELEM_1C_PARTNER "Контрагент"
#define ELEM_1C_KIND_CONTRACT "ВидДоговора"


#define ELEM_1C_FORMAT_VERSION "ВерсияФормата"
#define CONST_1C_FORMAT_VERSION "2.0"
#define ELEM_1C_DATE_UPLOAD "ДатаВыгрузки"
#define ELEM_1C_DATE_START "НачалоПериодаВыгрузки"
#define ELEM_1C_DATE_END "ОкончаниеПериодаВыгрузки"
#define ELEM_1C_NAME_SOURCE "ИмяКонфигурацииИсточника"
#define ELEM_1C_NAME_DEST "ИмяКонфигурацииПриемника"
#define CONST_1C_NAME_BOOK "БухгалтерияПредприятия"
#define ELEM_1C_GUID_RULES "ИдПравилКонвертации"
#define CONST_1C_GUID_RULES "1a1d060d-7a13-4820-b1b4-737f97d95857"
#define ELEM_1C_COMMENT "Комментарий"
#define ELEM_1C_KIND_OPER "ВидОперации"
#define ELEM_1C_KIND_CODE_OPER "КодВидаОперации"
#define ELEM_1C_KIND_CODE_IN "КодСпособаПолучения"

//#define CONST_1C_KIND_OPER_PRODUCT_IN "ПокупкаКомиссия"
//#define CONST_1C_KIND_OPER_PRODUCT_MOVE "ТоварыПродукция"
#define CONST_1C_KIND_CODE_OPER_PRODUCT_IN "01"
#define CONST_1C_KIND_CODE_IN 1
#define CONST_1C_DOCUMENT_NUMBER_LEN 11


#define CONST_1C_TRUE "true"
#define CONST_1C_FALSE "false"
#define CONST_1C_DOCUMENT_LINK "ДокументСсылка"
#define CONST_EMPTY_GUID "{empty}"
#define CONST_1C_CONST_STRING "Строка"
#define CONST_1C_CONST_DATE "Дата"
#define CONST_1C_CONST_BOOLEAN "Булево"
#define CONST_1C_CONST_NUMBER "Число"
#define CONST_1C_AUTOMATICALLY "Автоматически"
#define CONST_1C_METHOD_VAT_IN_PRICE "НеИзменять"

#define CONST_1C_DEFAULT_VALUTA "643"
//#define CONST_1C_OTHER_PROFIT_OUTLAY_SURPLUS "00054"
#define  CONST_1C_PRICE_PURCHASE "000000002"

//#define PARTNER_DOMINO_REGION "XXX"
//#define PARTNER_DOMINO_CODE 4545569
#define PARTNER_DATA_DOMINO_TYPE 15000

#define DATE_FORMAT_1C "%Y-%m-%dT%H:%M:%S"

#define CONST_1C_FIND_FIELD_ORDER_PRODUCT "СтрокаИменСвойствПоиска = \"НомерВходящегоДокумента\";"
#define CONST_1C_FIND_FIELD_ORDER_NUMBER "СтрокаИменСвойствПоиска = \"Номер,НомерВходящегоДокумента\";"



#endif // EXCHANGE_CONST_H_INCLUDED
