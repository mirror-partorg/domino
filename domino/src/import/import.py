#!/bin/env python
#-*- coding: UTF-8 -*-

from xml.dom.minidom import parse, parseString
from datetime import datetime,date,time

datetime_format="%d.%m.%Y %H.%M.%S"
date_format="%d.%m.%Y"
time_format="%H.%M.%S"
TAG_DOC="Документ"
ATTR_DOC_DATE="ДатаДок"
ATTR_DOC_TIME="ВремДок"
TAG_INVOICE="ТН"
ATTR_INVOICE_DATE="ДатаТН"
ATTR_INVOICE_NUMBER="НомТН"
TAG_PRODUCT="СвТов"
ATTR_PRODUCT_CODE="КодТов"
ATTR_PRODUCT_NAME="НаимТов"
ATTR_PRODUCT_UNIT="НаимЕдИзм"
ATTR_PRODUCT_QUANT="КолМест"
ATTR_PRODUCT_UNIT_CODE="ОКЕЙ_Тов"
ATTR_PRODUCT_VAT="СтавкаНДС"
ATTR_PRODUCT_VAT_SUM="СумНДС"
ATTR_PRODUCT_SUM="СумУчНДС"

def load_file(filename):
	dom = parse(filename)
	doc=dom.getElementsByTagName(TAG_DOC)
	res=[datetime.strptime("%s %s"%(doc[0].getAttribute(ATTR_DOC_DATE),doc[0].getAttribute(ATTR_DOC_TIME)),datetime_format)]
	invoice=dom.getElementsByTagName(TAG_INVOICE)
	res.append([datetime.strptime(invoice[0].getAttribute(ATTR_INVOICE_DATE),date_format),invoice[0].getAttribute(ATTR_INVOICE_NUMBER)])
	res.append([])
	for line in dom.getElementsByTagName(TAG_PRODUCT):
		res[3].append([
					line.getAttribute(ATTR_PRODUCT_CODE),
					line.getAttribute(ATTR_PRODUCT_NAME),
					line.getAttribute(ATTR_PRODUCT_UNIT),
					line.getAttribute(ATTR_PRODUCT_UNIT_CODE),
					line.getAttribute(ATTR_PRODUCT_QUANT),
					line.getAttribute(ATTR_PRODUCT_),
					line.getAttribute(ATTR_PRODUCT_NAME),
					line.getAttribute(ATTR_PRODUCT_NAME),
					line.getAttribute(ATTR_PRODUCT_NAME),
					
					
	print(doc_datetime,invoice_date,invoice_number)
	#doc_date=doc.get
	#print(doc_date))#m.getElementsByTagName("ТН"))
	
if __name__ == "__main__":
	load_file("./DP_OTORG12.xml")