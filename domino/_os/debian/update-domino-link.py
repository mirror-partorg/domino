#!/bin/env python3
#-*- coding: utf-8 -*-

from psycopg2 import connect
from datetime import datetime
from socket import gethostname
from os import system
from sys import exit,argv

conn_str_local = "host=localhost dbname=domino user=domino password=trust"
conn_str_main = "host=db.glekar.com dbname=domino user=domino password=trust"
#conn_str_main = "host=192.168.8.2 dbname=domino user=domino password=trust"
send_error_programm = "/usr/local/domino/bin/send-error-message"

def send_error(msg):
    print("Error: %s"%(msg))
    system("%s \"%s %s: %s\""%(send_error_programm,gethostname(),argv[0],msg))
    exit(1)

def execute(cur,str_,par=None):
    try: cur.execute(str_,par)
    except: send_error("Невозможно выполнить \'%s\'"%(str_))

print("connect to local %s"%(conn_str_local))
try: db_local=connect(conn_str_local) 
except: send_error("Невозможно соединиться с \'%s\'"%(conn_str_local))
print("connected to local")
print("connect to main %s"%(conn_str_main))
try: db_main=connect(conn_str_main)
except: send_error("Невозможно соединиться с \'%s\'"%(conn_str_main))
print("connected to main")
cur_main=db_main.cursor()
cur=db_local.cursor()

execute(cur,"SELECT action_out_index,action_in_index FROM _replic")
l=cur.fetchone()
indx = [l and l[0] or 0,l and l[1] or 0]
insert_repl = not l
old_indx = indx[:]
execute(cur,"""SELECT b.action_type,a.*,b.action_time,b.product_barcode,b.hostname FROM _protocol as b,
(SELECT key_supplier_code,key_product_code,MAX(action_index) as action_index 
 FROM _protocol 
 WHERE action_index > %s  AND action_obj in (1,2,3)
 GROUP BY key_supplier_code,key_product_code
 ORDER BY action_index) as a WHERE b.action_index = a.action_index"""%(indx[0]))
l=cur.fetchall()
print("update from local %s"%(len(l)))
for r in l:
	if r[0] == 1: # update
		execute(cur,"SELECT * FROM _product WHERE supplier_code=%s AND product_code=%s",(r[1],r[2]))
		p = cur.fetchone()
		if not p: continue
		execute(cur,"SELECT * FROM _product_link WHERE supplier_code=%s AND product_code=%s",(r[1],r[2]))
		l_ =cur.fetchone()
		if not l_: continue
		print(l_)
		print("SELECT * FROM make_main_product_link(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)"%(p[0],p[1],p[2],p[3],l_[2],l_[3],l_[4],l_[5],r[4],r[5],r[6] or gethostname()))
		execute(cur_main,"SELECT * FROM make_main_product_link(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)",(p[0],p[1],p[2],p[3],l_[2],l_[3],l_[4],l_[5],r[4],r[5],r[6] or gethostname()))
		sad=cur_main.fetchone()
		execute(cur_main,"COMMIT")
		indx[0] = r[3]
	elif r[0] == 2: # remove
		execute(cur_main,"SELECT * FROM clear_main_product_link(%s,%s,%s)",(r[1],r[2],r[4]))
		sad=cur_main.fetchone()
		execute(cur_main,"COMMIT")
		indx[0] = r[3]
if old_indx[0] != indx[0]:
	if insert_repl:
		execute(cur,"INSERT INTO _replic (action_out_index,action_out_time) VALUES(%s,%s)",(indx[0],datetime.now()))
	else:
		execute(cur,"UPDATE _replic set action_out_index=%s,action_out_time=%s",(indx[0],datetime.now()))
	execute(cur,"COMMIT")
execute(cur_main,"""SELECT b.action_type,a.*,b.product_barcode FROM _protocol as b,
(SELECT key_supplier_code,key_product_code,action_obj,MAX(action_index) as action_index 
 FROM _protocol 
 WHERE action_index > %s  AND action_obj in (1,2,3)
 GROUP BY key_supplier_code,key_product_code,action_obj
 ORDER BY action_index) as a WHERE b.action_index = a.action_index"""%(indx[1]))
l=cur_main.fetchall()
print("update from global %s"%(len(l)))
for r in l:
	if r[0] == 1: # update
		if r[3] == 1: # product
			execute(cur_main,"SELECT * FROM _product WHERE supplier_code=%s AND product_code=%s",(r[1],r[2]))
			p = cur_main.fetchone()
			if not p: continue
			execute(cur,"SELECT product_name,manufactor_name FROM _product WHERE supplier_code=%s AND product_code=%s",(r[1],r[2]))
			pl=cur.fetchone()
			if not pl:
				execute(cur,"INSERT INTO _product VALUES(%s,%s,%s,%s)",p)
				execute(cur,"COMMIT")
			elif pl[0] != p[2] or pl[1] != p[3]:
				execute(cur,"UPDATE _product SET product_name=%s,manufactor_name=%s WHERE supplier_code=%s AND product_code=%s",(p[2],p[3],p[0],p[1]))
				execute(cur,"COMMIT")
			indx[1] = r[4]
		elif r[3] == 2: #link
			execute(cur_main,"SELECT * FROM _product_link WHERE supplier_code=%s AND product_code=%s",(r[1],r[2]))
			l_ =cur_main.fetchone()
			if not l_: continue
			execute(cur,"SELECT domino_code,domino_name,domino_coef,domino_unpack FROM _product_link WHERE supplier_code=%s AND product_code=%s",(r[1],r[2]))
			ll = cur.fetchone()
			if not ll:
				execute(cur,"INSERT INTO _product_link VALUES(%s,%s,%s,%s,%s,%s)",l_)
				execute(cur,"COMMIT")
			elif ll[0] != l_[2] or ll[1] != l_[3] or ll[2] != l_[4] or ll[3] != l_[5]:
				execute(cur,"UPDATE _product_link SET domino_code=%s,domino_name=%s,domino_coef=%s,domino_unpack=%s WHERE supplier_code=%s AND product_code=%s",(l_[2],l_[3],l_[4],l_[5],l_[0],l_[1]))
				execute(cur,"COMMIT")
			indx[1] = r[4]
		elif r[3] == 3: #barcode
			execute(cur_main,"SELECT * FROM _product_barcode WHERE supplier_code=%s AND product_code=%s AND product_barcode=%s",(r[1],r[2],r[5]))
			l_ =cur_main.fetchone()
			if not l_:
				execute(cur,"INSERT INTO _product_barcode VALUES(%s,%s,%s)",(r[1],r[2],r[5]))
				execute(cur,"COMMIT")
			indx[1] = r[4]
	elif r[0] == 2: #remove
		execute(cur,"DELETE FROM _product_link WHERE supplier_code=%s AND product_code=%s",(r[1],r[2]))
		execute(cur,"COMMIT")
		indx[1] = r[4]
		print(r[4])
if old_indx[1] != indx[1]:
	if insert_repl:
		execute(cur,"INSERT INTO _replic (action_in_index,action_out_index,action_in_time) VALUES(%s,%s,%s)",(indx[1],1,datetime.now()))
	else:
		execute(cur,"UPDATE _replic set action_in_index=%s,action_in_time=%s",(indx[1],datetime.now()))
	execute(cur,"COMMIT")
