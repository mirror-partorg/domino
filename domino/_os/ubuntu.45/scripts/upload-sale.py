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
DOMINO_CODE="cp866"

def send_error(msg):
	print("Error: %s"%(msg))
	system("%s \"%s %s: %s\""%(send_error_programm,gethostname(),argv[0],msg))
	exit(1)

def get_shift_info(cur,shift=None):
	if shift:
		if shift==-1:
			where="WHERE not isnull(time_end)"
		else:
			where="WHERE shiftnum=%s"%(shift)
	else: where=""
	sql="""SELECT cashcode,workshiftid,shiftnum,time_beg,time_end,scode,
	                     IF(workshift.scode,(SELECT username FROM users WHERE users.usercode=workshift.scode AND workshift.workshiftid=users.workshiftid),'') 
	               FROM workshift %s ORDER BY workshiftid DESC LIMIT 1"""%(where)
	cur.execute(sql)
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
def get_sale(cur,info,pos=0):
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
	              IF(a.opcode=58,-a.cquant,a.cquant),
	              a.scode,
	              (SELECT MAX(valcode) FROM moneyitem AS c 
	                  WHERE c.documentid=a.documentid AND c.opcode IN (70,74)) as valuta,
	              0,
	              IFNULL((SELECT number FROM carditem AS c WHERE c.documentid=a.documentid),'') as cardnumber,
	              a.posnum,
	              a.bquant,
	              a.documentid,
	              a.sumb,
	              a.sumn
	              FROM goodsitem AS a,document AS b
	              WHERE goodsitemid > %d AND workshiftid=%d AND doctype IN (1,2,25) AND opcode in (50,58) AND
	                    closed=1 AND a.documentid=b.documentid ORDER BY goodsitemid"""%(pos,info[1])
	#print(sql)
	cur.execute(sql)
	return cur.fetchall()
def get_money(cur,info):
	sql="""SELECT b.documentid,
	              b.checknum,
	              b.scode,
	              b.scode,
	              b.cashcode,
	              %d,
	              a.mtime,
	              a.mtime,
	              a.opcode,
	              a.valcode,
	              1.000,
	              0.000,
	              0,
	              a.vsum,
	              a.sumb,
	              a.sumn,
	              a.sume,
	              a.docnum,
	              a.c_link
	              FROM moneyitem AS a,document AS b
	              WHERE workshiftid=%d AND doctype IN (1,2,25) AND
	                    closed=1 AND a.documentid=b.documentid ORDER BY moneyitemid"""%(info[2],info[1])
	#print(sql)
	cur.execute(sql)
	return cur.fetchall()
def print_file(filename,info,sale,money):
	h=open(filename,"w+")
	txt=u"""КОНТРОЛЬНАЯ ЛЕНТА
ДАТА=%s
ДАТА НАЧАЛА СМЕНЫ=%s
ВРЕМЯ=%s
КАССА=%s
СЕАНС=%s/%6.6d
КАССИР=%s\n\n"""%(info[4].strftime("%d/%m/%Y"),info[3].strftime("%d/%m/%Y"),info[4].strftime("%H:%M"),info[0],info[0],info[2],info[6])
	h.write(txt.encode(DOMINO_CODE))
	for i in sale:
		bcode=i[7]
		if bcode[:len(ARTIX_PRODUCT_CODE_PREFIX)]==ARTIX_PRODUCT_CODE_PREFIX:
			bcode=bcode[len(ARTIX_PRODUCT_CODE_PREFIX):]
		arg=tuple([i[1]]+[i[2].strftime("%d/%m/%Y"),i[3].strftime("%H:%M")]+list(i[4:7])+[bcode]+list(i[8:21])+list(i[23:25]))
		txt="T=%d,%s,%s,\"%s\",\"%s\",%d,\"%s\",%d,%d,%d,%f,%d,%d,%d,%f,%s,%d,%d,%s,%d,%f,%f\n"%arg
		h.write(txt.encode(DOMINO_CODE))
	h.write("\n\n")
	for i in money:
		arg=tuple(list(i[1:6])+[i[6].strftime("%d/%m/%Y"),i[7].strftime("%H:%M")]+list(i[8:19]))
		txt="M=%d,%s,%s,%s,%d,%s,%s,%d,%d,%f,%f,%d,%f,%f,%f,%f,\"%s\",%d\n"%arg
		h.write(txt.encode(DOMINO_CODE))
	h.close()
if __name__ == "__main__":
	l=connect(host="localhost", user="root", passwd="", db="documents", charset='utf8')
	cur=l.cursor()
	info=get_shift_info(cur,shift=len(argv)==2 and argv[1] or -1)
	sale=get_sale(cur,info)
	money=get_money(cur,info)
	cur.close()
	filename="/usr/local/domino/mail/cl/%2.2d%6.6d.cl"%(int(info[0]),info[2])
	print_file(filename,info,sale,money)
