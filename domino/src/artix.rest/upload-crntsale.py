#!/usr/bin/python
# -*- coding: utf-8 -*-

from pydomino import Alias,local_store
from socket import gethostname
from sys import argv,exit
from MySQLdb import connect
from os import system
from datetime import datetime

send_error_programm = "/usr/local/domino/bin/send-error-message"
ARTIX_PRODUCT_CODE_PREFIX="DC"

def send_error(msg):
	print("Error: %s"%(msg))
	system("%s \"%s %s: %s\""%(send_error_programm,gethostname(),argv[0],msg))
	exit(1)

def get_shift_info(cur,shift=None):
	if shift:
		where="WHERE shiftnum=%s"%(shift)
	else: where=""
	cur.execute("""SELECT cashcode,workshiftid,shiftnum,time_beg,time_end,scode,
	                     IF(workshift.scode,(SELECT username FROM users WHERE users.usercode=workshift.scode AND workshift.workshiftid=users.workshiftid),'') 
	               FROM workshift %s ORDER BY workshiftid DESC LIMIT 1"""%(where))
	p = cur.fetchone()
	return p
def get_last_pos(alias,info,store):
	k=alias.DocumentKey0()
	k.set(["ЧК",store,"%2.2d%4.4d"%(int(info[0]),info[2])])
	if not k.get_equal():
		now=datetime.now()
		d=alias.Document()
		d.set_document_type(k.document_type)
		d.set_document_operation("1")
		d.set_store(k.store)
		d.set_document(k.document)
		d.set_date(now)
		d.set_time(now)
		d.set_partner_region_code("КА")
		d.set_partner_code(int(info[0]))
		d.set_partner_document("%2.2d/%6.6d"%(int(info[0]),info[2]))
		d.set_partner_document_date(now)
		d.set_partner_document_time(now)
		try: err = not d.insert()
		except: err = True
		if err:
			send_error("Невозможно создать документ:\n%s"%(d.__str__()))
	k1=alias.DocumentOrderKey0()
	k1.set(k)
	k2=k1.clone()
	k1.set_line(999999999)
	if not k1.get_le() or k1.cmp(k2,3):
		return 0
	return k1.line
def get_sale(cur,info,pos):
	sql="""SELECT a.goodsitemid,
	              b.checknum,
	              a.ttime,
	              a.ttime,
	              IF((SELECT MAX(valcode) FROM moneyitem AS c 
	                  WHERE c.documentid=a.documentid AND c.opcode IN (70,74)) = 1,
	                 IF(a.opcode = 50,'01','02'),
	                 IF(a.opcode = 50,'03','04')) as opcode,
	              '2' as division,
	              1,
	              a.bcode,
	              1,1,1,
	              a.sumb/a.cquant,
	              1,1,1,
	              a.cquant,
	              a.scode,
	              (SELECT MAX(valcode) FROM moneyitem AS c 
	                  WHERE c.documentid=a.documentid AND c.opcode IN (70,74)) as valuta,
	              0,
	              IFNULL((SELECT number FROM carditem AS c WHERE c.documentid=a.documentid),'') as cardnumber,
	              a.posnum,
	              a.bquant,
	              a.documentid
	              FROM goodsitem AS a,document AS b
	              WHERE goodsitemid > %d AND workshiftid=%d AND doctype IN (1,2,25) AND opcode in (50,58) AND
	                    closed=1 AND a.documentid=b.documentid ORDER BY goodsitemid"""%(pos,info[1])
	#print(sql)
	cur.execute(sql)
	return cur.fetchall()
def add_sale(alias,store,info,sale):
	o=alias.DocumentOrder()
	k,b=alias.BarcodeKey0(),alias.Barcode()
	ks,s=alias.StockKey0(),alias.Stock()
	ks.set_store(store)
	o.set_document_type("ЧК")
	o.set_document_operation("1")
	o.set_store(store)
	o.set_document("%2.2d%4.4d"%(int(info[0]),info[2]))
	alias.transaction_start()
	for sale in sales:
		bcode=sale[7]
		quant=float(sale[15])
		price=float(sale[11])
		if bcode[:len(ARTIX_PRODUCT_CODE_PREFIX)]==ARTIX_PRODUCT_CODE_PREFIX:
			bcode=bcode[len(ARTIX_PRODUCT_CODE_PREFIX):]
		k.set([bcode])
		try: err = not b.get_equal(k)
		except: err = True
		if err:
			send_error("Невозможно найти товар с продажным кодом \"%s\""%(bcode))
		o.set_line(sale[0])
		o.set_code(b.code)
		o.set_date(sale[2])
		o.set_time(sale[3])
		o.set_quant(2,quant)
		o.set_price(9,price)
		o.set_param("ACPT","1")
		o.set_param("НОМЕР_ЧЕКА",str(sale[1]))
		o.set_param("КАССИР",sale[16])
		o.set_param("КОЛ",str(sale[21]))
		o.set_param("ШТРИХКОД",sale[7])
		o.set_param("ПОЗИЦИЯ",str(sale[20]))
		o.set_param("ОПЕРАЦИЯ",sale[4])
		o.set_param("ВАЛЮТА",str(sale[17]))
		if sale[19]:
			o.set_param("КАРТА",sale[19])
		o.set_accepted(1)
		try: err = not o.insert()
		except: err = True
		if err:
			send_error("Невозможно добавить строку:\n%s"%(o.__str__()))
		ks.set_code(b.code)
		if not s.get_equal(ks):
			s.set_store(store)
			s.set_code(b.code)
			s.clear_quants()
			s.set_quant(2,-quant)
			s.insert()
		else:
			s.set_quant(2,s.quant(2)-quant)
			s.update()
	alias.transaction_stop()
	print("%s: Добавленно %d стр."%(datetime.now().strftime("%d/%m/%y %H:%M:%S"),len(sales)))
if __name__ == "__main__":
	a=Alias()
	store=local_store()
	l=connect(host="localhost", user="root", passwd="", db="documents", charset='utf8')
	cur=l.cursor()
	info=get_shift_info(cur,shift=len(argv)==2 and argv[1] or None)
	pos=get_last_pos(a,info,store)
	sales=get_sale(cur,info,pos)
	if sales:
		add_sale(a,store,info,sales)
	cur.close()
