#!/usr/bin/python
# -*- coding: utf-8 -*-

import httplib2
import urllib
import base64
import json
import time
from pydomino import Alias,local_store
from xml.dom.minidom import parse
from sys import argv,exit,version_info
from os import system,getpid,path
from socket import gethostname
from datetime import datetime,timedelta
from fcntl import flock,LOCK_EX,LOCK_NB,LOCK_UN,LOCK_SH

lock_file="/tmp/load_handbook.lock"
send_error_programm = "/usr/local/domino/bin/send-error-message"
ARTIX_UNPACK_PRICE_INDEX=50
DOMINO_PROTOCOL_PRODUCT_PREFIX="ТОВАР."
DOMINO_RETAIL_PRICE_INDEX=4
DOMINO_BASE_CODE_LEN=5
DOMINO_CASHIER_REGION="CAS"
DOMINO_KEYBOARD_TYPE="GR"
ARTIX_PRODUCT_CODE_PREFIX="DC"
ARTIX_CASHIER_PASSWORD_MASK=";%s=1?"
ARTIX_ROLES={"1":"5","2":"6","4":"2","8":"4"}
domino_units = {"АМПЛ":1,
"БЛСТ":2,
"ДРЖ.":3,
"КАПС":4,
"КАРП":5,
"ПАК.":6,
"ПАРА":7,
"ПОР.":8,
"СВЕЧ":9,
"ТАБ.":10,
"ТУБА":11,
"ТЮБ.":12,
"УПАК":13,
"ФЛ.":14,
"ШТ":15}


class ArtixExchange:
	def __init__(self,shopcode,hostname,port=28080,user="admin",password="admin"):
		self.host=hostname
		self.shopcode=shopcode
		self.user=user
		self.port=port
		self.password=password
	def send(self,commands):
		if version_info < (3,):
			login = b"%s:%s"%(self.user,self.password)
		else:
			login = bytes("%s:%s"%(self.user,self.password), 'utf-8')
		http = httplib2.Http(".cache")
		base64string = base64.encodestring(login)
		str_=base64string.decode("utf-8")
		authheader =  "Basic %s" % str_
		jsonHeader = {'Content-Type':'application/json', "Accept": "application/json", "Authorization": authheader}
		method = 'POST'
		dictionaryId = str(time.time())
		url = 'http://%s:%s/aifexchange/loaddicts?shopcode=%s&dictionaryid=%s' % (self.host, self.port, self.shopcode, dictionaryId)
		body = json.dumps(commands)
		answer = None
		try:
			response, content = http.request(url, method, headers=jsonHeader, body=body)
			if response.status == 200:
				answer = json.loads(content.decode("cp1251"))
			else: print("status",response.status)
		except: pass
		return answer and answer['success']

def send_error(msg):
	print("Error: %s"%(msg))
	system("%s \"%s %s: %s\""%(send_error_programm,gethostname(),argv[0],msg))
	exit(1)

def add_units():
	commands=[]
	for i in domino_units:
		commands.append({
		"command":"addUnit",
        "unit":{
            "unitCode":domino_units[i],
            "name":i,
            "fractional":False
        }})
	return commands
def get_barcode(product,barcode):
	bar={"barcode": barcode.barcode,"cquant": product.coef}
	if product.coef > 1:
		bar["measureCode"]=domino_units[product.tech_unit]
		bar["packingmeasure"]=domino_units[product.unit]
		bar["packingprice"]=product.price(DOMINO_RETAIL_PRICE_INDEX)
		bar["price"]=product.price(DOMINO_RETAIL_PRICE_INDEX)*product.coef
		bar["cquant"]=product.coef
	return bar
def get_product(product,barcode,barcodekey,store_pref):
	if not product.price(DOMINO_RETAIL_PRICE_INDEX): return None
	if product.is_parcel and product.code[6:8] != store_pref: return None
	barcodekey.set_code(product.code)
	if not barcode.get_equal(barcodekey): return None
	data = {"inventcode":(not product.is_parcel and ARTIX_PRODUCT_CODE_PREFIX or "") + product.code,
			"measureCode":domino_units[product.unit],
			"isInventItem":True,
			"price":product.price(DOMINO_RETAIL_PRICE_INDEX),
			"barcodes":[get_barcode(product,barcode)],
			"cquant": 1,
			"name": product.name}
	k=barcodekey.clone()
	while barcode.get_next(barcodekey):
		if k.cmp(barcodekey,1): break
		data["barcodes"].append(get_barcode(product,barcode))
	return data
def add_product(product,barcode,barcodekey,mstore):
	store_pref="%2.2d"%(int(mstore))
	data = get_product(product,barcode,barcodekey,store_pref)
	if not data: return None
	return {"command":"addInventItem", 
			 "invent":data}
def add_invent(alias,mstore,code):
	productkey,product=alias.ProductKey4(),alias.Product()
	barcodekey,barcode=alias.BarcodeKey1(),alias.Barcode()
	productkey.set_code(code)
	commands = []
	if not product.get_equal(productkey): 
		return commands
	command = add_product(product,barcode,barcodekey,mstore)
	if command: commands.append(command)
	if not product.is_parcel:
		while product.get_next(productkey):
			if product.code[:DOMINO_BASE_CODE_LEN] != code: break
			command = add_product(product,barcode,barcodekey,mstore)
			if command: commands.append(command)
	return commands
def add_invents(alias,mstore):
	productkey,product=alias.ProductKey4(),alias.Product()
	barcodekey,barcode=alias.BarcodeKey1(),alias.Barcode()
	res = []
	productkey.set([""])
	b = product.get_ge(productkey)
	i = 0
	while b:
		com = add_product(product,barcode,barcodekey,mstore)
		if com:
			res.append(com)
		i+=1
		if not i % 100: print(i)
		#if i > 100: break
		b = product.get_next(productkey)
	return res
def add_users(alias,mstore):
	res=[]
	partner,k=alias.Partner(),alias.PartnerKey0()
	k.set([DOMINO_CASHIER_REGION,0])
	k1=k.clone()
	b=partner.get_ge(k)
	while b:
		if k1.cmp(k,1): break
		store=partner.store
		if store == mstore:
			data=add_user(partner=partner)
			if data:
				res += data
		b=partner.get_next(k)
	return res
def add_user(alias=None,partner=None,code=None):
	if not partner:
		partner,k=alias.Partner(),alias.PartnerKey0()
		k.set([DOMINO_CASHIER_REGION,code])
		if not partner.get_equal(k):
			return None
	role = partner.param("ПРО_ROLE")
	if role not in ARTIX_ROLES: return None
	return [{
        "command":"addMCashUser",      
        "mcashuser":
        {"code":str(partner.code),
         "name":partner.name,
         "login":str(partner.code),
         "password":ARTIX_CASHIER_PASSWORD_MASK%(partner.param("ПРО_LOGIN")),
         "roleusers":
          [{"rolecode":ARTIX_ROLES[role],
            "rule":"1"}]}}]
def getText(nodelist):
	rc = []
	for node in nodelist:
		if node.nodeType == node.TEXT_NODE:
			rc.append(node.data)
	return ''.join(rc)
def getValue(elemlist):
	try:
		return getText(elemlist[0].childNodes)
	except:
		return None
def nodeElementText(elem,name):
	return getValue(elem.getElementsByTagName(name))
class KeyboardValue:
	keycode=0
	cmactioncode=0
	keyvalue=""
	context="allcontext"
	def __init__(self,elem):
		self.keycode = int(nodeElementText(elem.getElementsByTagName("pk")[0],"keycode"))
		self.cmactioncode=int(nodeElementText(elem,"cmactioncode"))
		self.keyvalue=nodeElementText(elem,"keyvalue")
		self.context=nodeElementText(elem,"context")
	def __str__(self):
		return """
"kbkey":{
         "keycode":%d,
         "cmactioncode":%d,
         "keyvalue":"%s",
         "context":"%s"
        }"""%(self.keycode,self.cmactioncode,self.keyvalue,self.context)
def load_func_keys_from_file(filename):
	dom = parse(filename)
	res = []
	for elem in dom.getElementsByTagName("domainCollection"):
		if elem.getAttribute("xsi:type") == "kbkey":
			k = KeyboardValue(elem)
			res.append(k)
	return res
def add_func_keys(filename):
	res = []
	for l in load_func_keys_from_file(filename):
		res.append({
        "command":"addKbKey",
        "kbkey":{
                "keycode":l.keycode,
                "cmactioncode":l.cmactioncode,
                "keyvalue":l.keyvalue,
                "context":l.context}})
	return res
def add_invent_keys(alias,mstore):
	document,dkey=alias.Document(),alias.DocumentKey0()
	order,okey=alias.DocumentOrder(),alias.DocumentOrderKey0()
	dkey.set([DOMINO_KEYBOARD_TYPE,mstore,""])
	k1=dkey.clone()
	kp,pp=alias.ProductKey4(),alias.Product()
	res = []
	b=document.get_ge(dkey)
	while b:
		if k1.cmp(dkey,2): break
		if document.param("404") in ["1","2"]:
			codes = []
			if document.param("ТОВАР"): 
				code1=document.param("ТОВАР")
				kp.set([code1])
				if pp.get_equal(kp) and pp.price(4):
					codes.append(ARTIX_PRODUCT_CODE_PREFIX+code1)
				else:
					print(code1)
			okey.set(document)
			okey.set_line(0)
			k=okey.clone()
			b = order.get_ge(okey)
			while b:
				if k.cmp(okey,3): break
				kp.set([order.code])
				if pp.get_equal(kp) and pp.price(4):
					codes.append(ARTIX_PRODUCT_CODE_PREFIX+order.code)
				else:
					print(order.code)
				b = order.get_next(okey)
			res.append({"hotkey": {
						"hotkeycode": int(document.document),
						"hotkeyname": document.param("420"),
						"inventcodes":codes
					   },
						"command": "addHotKey"})
		b=document.get_next(dkey)
	return res
def get_list_update(alias,name):
	last = datetime.now()
	k,p=alias.ProtocolKey2(),alias.Protocol()
	k.set([name,last,last])
	b=p.get_le(k)
	if not b or p.user_name != name or datetime.combine(p.date,p.time) < last+timedelta(days=-2) : return None,None
	ret=[]
	k=alias.ProtocolKey1()
	last = datetime.combine(p.date,p.time)+timedelta(seconds=-50)
	k.set([last,last])
	b=p.get_ge(k)
	while b:
		if p.object_name[:len(DOMINO_PROTOCOL_PRODUCT_PREFIX)] == DOMINO_PROTOCOL_PRODUCT_PREFIX:
			code = p.object_name[len(DOMINO_PROTOCOL_PRODUCT_PREFIX):len(DOMINO_PROTOCOL_PRODUCT_PREFIX)+5]
			if code not in ret:
				ret.append(code)
		last = datetime.combine(p.date,p.time)
		b=p.get_next(k)
	return ret,last
def set_last(alias,name,last):
	p=alias.Protocol()
	p.set_object_name("EXCHANGE")
	p.set_user_name(name)
	p.set_date(last)
	p.set_time(last)
	p.insert()
def add_updates(alias,mstore,username):
	codes,last = get_list_update(alias,username)
	now=datetime.now()
	ret = []
	if not last:
		ret += [{"command": "clearInventory"}]
		ret += add_units()
		ret += add_invents(a,store)
	elif codes:
		print("%s: Данные с %s необходимо обновить %d тов."%(datetime.now().strftime("%d/%m/%y %H:%M:%S"),last.strftime("%d/%m/%y %H:%M:%S"),len(codes)))
		for code in codes:
			r = add_invent(alias,mstore,code)
			if r:
				ret += r
	return ret,now
def lock(filename):
	try:
		if path.isfile(filename):
			flock(open(filename,"r"),LOCK_EX | LOCK_NB)
	except:
		try:
			d=int(open(filename,"r").read()) or -999
		except: d=-999
		return d
	open(filename,"w").write("%d"%(getpid()))
	global h
	h=open(filename,"r")
	flock(h,LOCK_EX)
	return 0
if __name__ == "__main__":
	pid=lock(lock_file)
	if pid: 
		print("lock by proccess %d"%(pid))
		exit(1)
	a=Alias()
	store=local_store()
	artix=ArtixExchange(store,"kassa-99")
	username=gethostname()
	last=None
	commands = []
	#last = get_protocols(a,)
	len_=len(argv)
	if len_ not in [2,3] or (len_ == 2 and argv[1] not in ["products","cashiers","updates"]) or (len_ == 3 and argv[1] not in ["product"]):
		send_error("Неправильный параметр %s"(argv))
	if len_ == 1 or (len_ == 2 and argv[1] == "updates"):
		ret,last = add_updates(a,store,username)
		commands += ret
	elif argv[1] == "products":
		last = datetime.now()
		commands += [{"command": "clearInventory"}]
		commands += add_units()
		commands += add_invents(a,store)
	elif argv[1] == "cashiers":
		commands += [{"command": "clearMCashUser"}]
		commands += add_users(a,store)
	elif argv[1] == "product":
		commands += add_invent(a,store,argv[2])
	if commands:
		if not artix.send(commands):
			send_error("Ошибка обновления справочников")
		else:
			if last:
			    set_last(a,username,last)
			print("ok")
	#else:
	#	print("empty ok")
