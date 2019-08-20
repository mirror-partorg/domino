#!/usr/bin/python
# -*- coding: utf-8 -*-

import httplib2
import urllib
import base64
import json
import time
from pydomino import Alias,local_store,domino_date,domino_time
from xml.dom.minidom import parse
from sys import argv,exit,version_info
from os import system,getpid,path,chdir,fork,kill,listdir
from socket import gethostname
from datetime import datetime,timedelta
from fcntl import flock,LOCK_EX,LOCK_NB,LOCK_UN,LOCK_SH
from threading import Timer,Thread
from signal import SIGKILL
from MySQLdb import connect

from bottle import route, run, response, request, WSGIRefServer
from httplib import INTERNAL_SERVER_ERROR
import time
import traceback
WAITING_TIME_SEC = 60*10
STATUS_ERROR = 'STATUS_ERROR'                       # Ошибка загрузки/выгрузки справочника
STATUS_READY = 'STATUS_READY'                       # Справочник успешно загружен/выгружен
STATUS_READY_WITH_ERROR = 'STATUS_READY_WITH_ERROR' # Справочник загружен с ошибками
STATUS_DURING = 'STATUS_DURING'                     # Справочник в данный момент загружается/выгружается
results = {'exchanger': 0, 'nes': 0} # -1 - error, 0 - during, 1 - OK
mail_path = '/linuxcash/net/domino/domino/mail/for_kas/discount/'

from re import compile,IGNORECASE
f_match=compile("^d(?P<time>[0-9]{14})\.yaml$",IGNORECASE)


class MyServer(WSGIRefServer):
    def run(self, app): # pragma: no cover
        from wsgiref.simple_server import WSGIRequestHandler, WSGIServer
        from wsgiref.simple_server import make_server
        import socket

        class FixedHandler(WSGIRequestHandler):
            def address_string(self): # Prevent reverse DNS lookups please.
                return self.client_address[0]
            def log_request(*args, **kw):
                if not self.quiet:
                    return WSGIRequestHandler.log_request(*args, **kw)

        handler_cls = self.options.get('handler_class', FixedHandler)
        server_cls  = self.options.get('server_class', WSGIServer)

        if ':' in self.host: # Fix wsgiref for IPv6 addresses.
            if getattr(server_cls, 'address_family') == socket.AF_INET:
                class server_cls(server_cls):
                    address_family = socket.AF_INET6

        srv = make_server(self.host, self.port, app, server_cls, handler_cls)
        self.srv = srv ### THIS IS THE ONLY CHANGE TO THE ORIGINAL CLASS METHOD!
        srv.serve_forever()

    def shutdown(self): ### ADD SHUTDOWN METHOD.
        print_log("shutdown")
        self.srv.shutdown()

def setResult(status, typeStatus):
	global results
	if status:
		if status in [STATUS_ERROR, STATUS_READY_WITH_ERROR]:
			results[typeStatus] = -1
		elif status == STATUS_READY:
			results[typeStatus] = 1
server = None
server_close_request = False
def restMethod(result, typeStatus):
	global server,server_close_request
	response.content_type = 'application/json'
	try:
		if (request.body.len > 0):
			body = json.loads(request.body.read(), encoding='utf8')
			status = body.get('status', None)
			print_log('%s status: %s' % (typeStatus, status))
			setResult(status, typeStatus)
			if typeStatus == "nes":
				if status == STATUS_READY:
					server_close_request = True
					print_log("Загружено в базу данных")
				elif status == STATUS_ERROR:
					server_close_request = True
	except:
		print_log("%s"%traceback.format_exc())
		response.status = INTERNAL_SERVER_ERROR
		server_close_request = True
	return json.dumps(result, ensure_ascii=False)
@route('/CSrest/rest/dicts/backOfficeState/:shopCode', method='POST')
def restStatus(shopCode):
    result = str(time.time())
    return restMethod(result, 'exchanger')
@route('/CSrest/rest/dicts/backOfficeState/:shopCode/:dictStateId', method='PUT')
def restStatus(shopCode, dictStateId):
    result = dictStateId
    return restMethod(result, 'exchanger')
@route('/CSrest/rest/dicts/cashLoad/:shopCode/:cashCode/:dictStateId', method='PUT')
def restLoadStatus(shopCode, cashCode, dictStateId):
    result = dictStateId
    return restMethod(result, 'nes')
def runRest():
	global server
	server = MyServer(host="127.0.0.1", port=15080)
	run(server=server, quiet=True)

lock_file="/tmp/load_handbook.lock"
log_file="/usr/local/domino/log/load.log"
tmp_dir="/usr/local/domino/tmp"
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
MAX_=2147483646
optimist=True

def get_catalogs(alias):
	res={}
	k,r=alias.DocumentOrderKey0(),alias.DocumentOrder()
	indx={}
	k.set(["LT","LST","000002",0])
	k_=k.clone()
	b=r.get_ge(k)
	while b:
		if k_.cmp(k,3): break
		if r.code != "00001":
			if r.code not in res:
				res[r.code]=[]
			res[r.code].append(int(r.param("INDX")))
		b=r.get_next(k)
	return res
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
		address = '127.0.0.1:15080'
		url = 'http://%s:%s/aifexchange/loaddicts?shopcode=%s&dictionaryid=%s&addressstatusserver=%s' % (self.host, self.port, self.shopcode, dictionaryId, address)
		body = json.dumps(commands)
		answer = None
		try:
			response, content = http.request(url, method, headers=jsonHeader, body=body)
			if response.status == 200:
				answer = json.loads(content.decode("cp1251"))
			else: 
				send_error("send status %d"%response.status)
		except Exception as inst:
			send_error("error \"%s\""%(inst.__str__()))
		if not answer:
			send_error("not answer")
		elif not answer['success']:
			send_error("not answer['success']")
		return answer and answer['success']
def send_error(msg):
	print_log("Error: %s"%(msg))
	system("%s \"%s %s: %s\""%(send_error_programm,gethostname(),argv[0],msg.replace("\""," ")))
	kill(getpid(), SIGKILL)
def print_log(str_):
	f=open(log_file,"a+")
	f.write("%s: %s\n"%(datetime.now().strftime("%d/%m/%y %H:%M:%S"),str_))
	f.close()
	#print(str_)
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
	print_log("load units %d"%(len(commands)))
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
	if not product.is_parcel and (product.param("ЖВ") == "1" and product.param("ADS") != "1"): return None
	barcodekey.set_code(product.code)
	if not barcode.get_equal(barcodekey): return None
	data = {"inventcode": (not product.is_parcel and ARTIX_PRODUCT_CODE_PREFIX or "") + product.code,
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
def add_product(product,barcode,barcodekey,mstore,catalogs):
	store_pref="%2.2d"%(int(mstore))
	data = get_product(product,barcode,barcodekey,store_pref)
	if not data: return None
	ret = [{"command":"addInventItem", 
			 "invent":data}]
	code=product.code[:5]
	if code in catalogs:
		for indx in catalogs[code]:
			ret.append({"command": "addInventCatalog",
			 "inventcatalog": {"tmccode": data["inventcode"],
	                        "catalogId": indx}})
	return ret
def add_catalogs(alias,store,catalogs):
	ret = [] 
	for code in catalogs:
		ret+=add_invent(alias,store,code,catalogs)
		#ret.append({"command": "addInventCatalog",
		# "inventcatalog": {"tmccode": ARTIX_PRODUCT_CODE_PREFIX + code,
		#                   "catalogId": catalogs[code]}})
	return ret
def add_invent(alias,mstore,code,catalogs):
	productkey,product=alias.ProductKey4(),alias.Product()
	barcodekey,barcode=alias.BarcodeKey1(),alias.Barcode()
	productkey.set_code(code)
	commands = []
	if not product.get_equal(productkey): 
		return commands
	command = add_product(product,barcode,barcodekey,mstore,catalogs)
	if command: commands+=command
	if not product.is_parcel:
		while product.get_next(productkey):
			if product.code[:DOMINO_BASE_CODE_LEN] != code: break
			command = add_product(product,barcode,barcodekey,mstore,catalogs)
			if command: commands+=command
	print_log("load product %s %d"%(code,len(commands)))
	return commands
def add_invents(alias,mstore,catalogs):
	print_log("start load products")
	productkey,product=alias.ProductKey4(),alias.Product()
	barcodekey,barcode=alias.BarcodeKey1(),alias.Barcode()
	res = []
	productkey.set([""])
	b = product.get_ge(productkey)
	i = 0
	while b:
		com = add_product(product,barcode,barcodekey,mstore,catalogs)
		if com:
			res+=com
		#i+=1
		#if not i % 100: print(i)
		#if i > 100: break
		b = product.get_next(productkey)
	#print(i)
	print_log("load products %d"%(len(res)))
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
		if store == mstore or mstore == partner.param("ПОДР1") or partner.param("ПРО_ROLE")=="8":
			if partner.param("ПРО_ROLE")=="8": partner.set_param("ПРО_ROLE","2")
			d1,d2=None,None
			try: d1=datetime.strptime(partner.param("ST_DATE"),"%d/%m/%Y")
			except: pass
			try: d2=datetime.strptime(partner.param("RE_DATE"),"%d/%m/%Y")
			except: pass
			if not d1 or (d1 > datetime.now()) or (d2 and d2 < datetime.now()):
				data=add_user(partner=partner)
				if data:
					res += data
		b=partner.get_next(k)
	print_log("load users %d"%(len(res)))
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
         "password":len(partner.param("ПРО_LOGIN"))>4 and ARTIX_CASHIER_PASSWORD_MASK%(partner.param("ПРО_LOGIN")) or partner.param("ПРО_LOGIN"),
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
					print_log("product \"%s\" not found"%(code1))
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
					print_log("product \"%s\" not found or not price"%(order.code))
				b = order.get_next(okey)
			res.append({"hotkey": {
						"hotkeycode": int(document.document),
						"hotkeyname": document.param("420"),
						"inventcodes":codes
					   },
						"command": "addHotKey"})
		b=document.get_next(dkey)
	print_log("load units %d"%(len(res)))
	return res
def get_list_update(alias,name):
	k,r=alias.ProductDataKey0(),alias.ProductData()
	lastvalue = get_last_value(alias)
	if not lastvalue:
		last=datetime(2003,4,7,0,0,0)
		k.set(["00001",77,MAX_])
		if k.get_lt() and k.code == "00001" and k.record_type == 77:
			datetime
			lastvalue=k.number
		return None,lastvalue,None,True
	k.set(["00001",77,lastvalue])
	k_=k.clone()
	b=r.get_gt(k)
	ret=[]
	last=None
	while b:
		#print(p)
		if k_.cmp(k,2): break
		lastvalue = k.number
		if not last:
			last=datetime.combine(domino_date(int(r.param("3"))),domino_time(int(r.param("4"))))
		if r.param("1") == "1":
			code=r.param("2")[:5]
			if code not in ret:
				ret.append(code)
		b=r.get_next(k)
	return ret,lastvalue,last,False
def get_last_enum(alias):
	name="POS."+gethostname()
	k,r=alias.EnumKey0(),alias.Enum()
	k.set([name])
	if not r.get_equal(k):
		r.set_name(name)
		r.set_last_value(0)
		r.set_step(1)
		r.set_low_bound(0) 
		r.set_hi_bound(MAX_)
		r.set_cyclical(0)
		r.set_format("N11")
		r.insert()
	return r
def get_last_value(alias):
	enum=get_last_enum(alias)
	return enum.last_value
def set_last_value(alias,value):
	enum=get_last_enum(alias)
	enum.set_last_value(value)
	enum.update()
def set_last(alias,name,last):
	p=alias.Protocol()
	p.set_object_name("EXCHANGE")
	p.set_user_name(name)
	p.set_date(last)
	p.set_time(last)
	p.insert()
def add_updates(alias,mstore,username,catalogs):
	codes,lastvalue,last,fullupdate = get_list_update(alias,username)
	now=datetime.now()
	ret = []
	if fullupdate:
		print_log("Полное обновление")
		ret += [{"command": "clearInventory"}]
		ret += add_units()
		ret += add_invents(a,store,catalogs)
	elif codes:
		print_log("Данные с %s необходимо обновить %d тов."%(last.strftime("%d/%m/%y %H:%M:%S"),len(codes)))
		len_=0
		for code in codes:
			print_log("product %s"%(code))
			r = add_invent(alias,mstore,code,catalogs)
			if r:
				ret += r
				len_+=1
		print_log("Данные с %s необходимо действительно обновить %d тов. %d обьектов "%(last.strftime("%d/%m/%y %H:%M:%S"),len_,len(ret)))
	if len(ret):
		print_log("load updates %d"%(len(ret)))
	return ret,lastvalue,fullupdate
def lock(filename):
	first_run=False
	try:
		if path.isfile(filename):
			flock(open(filename,"r"),LOCK_EX | LOCK_NB)
		else:
			first_run=True
	except:
		try:
			d=int(open(filename,"r").read()) or -999
		except: d=-999
		return d,False
	open(filename,"w").write("%d"%(getpid()))
	global h
	h=open(filename,"r")
	flock(h,LOCK_EX)
	return 0,first_run
def send_artix(artix,commands,last):
	if not artix.send(commands):
		send_error("Ошибка обновления справочников")
	else:
		if last:
			set_last_value(a,last)
		print_log("Загружено в очередь")
	return True
def timeout_close():
	print_log("Timeout close")
	send_error("Время ожидания ответа от загрузчика NES закончилось")
def read_last_discount():
	l=[]
	for f in listdir(mail_path):
		m=f_match.match(f)
		if m:
			l.append(f)
	if l:
		f=sorted(l)[-1]
		m=f_match.match(f)
		time=datetime.strptime(m.group("time"),"%Y%m%d%H%M%S")
		disc=open(path.join(mail_path,f)).read()
		return disc,time
	return None,None
def get_update_time():
	con=connect(host="127.0.0.1", db="dictionaries", user="netroot", passwd="netroot", charset="utf8")
	cur=con.cursor()
	cur.execute("SELECT update_time FROM discountsystem WHERE discountsystemid=1 LIMIT 1")
	k=cur.fetchone()
	return k and k[0] or None
def load_discount_in_db(disc,time):
	print_log("Загрузка системы скидок за %s"%(time))
	try:
		con=connect(host="127.0.0.1", db="dictionaries", user="netroot", passwd="netroot", charset="utf8")
		cur=con.cursor()
		cur.execute("UPDATE discountsystem1 SET discsystem=%s,update_time=%s WHERE discountsystemid=1 LIMIT 1",(disc,time))
		cur.execute("UPDATE discountsystem2 SET discsystem=%s,update_time=%s WHERE discountsystemid=1 LIMIT 1",(disc,time))
		cur.execute("COMMIT")
		print_log("Система скидок загружена")
	except:
		send_error("Невозможно записать систему скидок в БД")
def load_discount():
	discount,time=read_last_discount()
	ltime=get_update_time()
	if not ltime or ltime < time:
		load_discount_in_db(discount,time)
if __name__ == "__main__":
	pid,first_run=lock(lock_file)
	if pid: 
		print_log("Lock by proccess %d"%(pid))
		exit(15)
	chdir(tmp_dir)
	a=Alias()
	store=local_store()
	artix=ArtixExchange(store,"127.0.0.1")
	username=gethostname()
	last=None
	len_=len(argv)
	commands = []
	commands_sec = []
	no_wait = argv[-1] == "no-wait"
	if no_wait: len_-=1
	if first_run:
		print_log("first run sleep")
		time.sleep(90)
		print_log("first run")
		load_discount()
		commands += [{"command": "clearMCashUser"}]
		commands += add_users(a,store)
		commands += [{"command": "clearHotKey"}]
		commands += add_invent_keys(a,store)
		commands += [{"command": "clearInventCatalog"}]
		commands += add_catalogs(a,store,get_catalogs(a))
		ret,last,fullupdate = add_updates(a,store,username,get_catalogs(a))
		commands += ret
		if not fullupdate and not optimist:
			if not send_artix(artix,commands,last):
				exit(1)
			commands = [{"command": "clearInventory"}]
			commands += [{"command": "clearInventCatalog"}]
			commands += add_units()
			commands += add_invents(a,store,get_catalogs(a))
	if len_ == 1 or (len_ == 2 and argv[1] == "updates"):
		ret,last,fullupdate = add_updates(a,store,username,get_catalogs(a))
		commands += ret
	elif argv[1] == "products":
		last = None
		commands += [{"command": "clearInventory"}]
		commands += [{"command": "clearInventCatalog"}]
		commands += add_units()
		commands += add_invents(a,store,get_catalogs(a))
	elif argv[1] == "catalogs":
		last = None
		commands += [{"command": "clearInventCatalog"}]
		commands += add_catalogs(a,store,get_catalogs(a))
	elif argv[1] == "users":
		commands += [{"command": "clearMCashUser"}]
		commands += add_users(a,store)
	elif argv[1] == "keys":
		commands += [{"command": "clearHotKey"}]
		commands += add_invent_keys(a,store)
	elif argv[1] == "product":
		commands += add_invent(a,store,argv[2],get_catalogs(a))
	elif argv[1] == "discount":
		load_discount()
	else:
		send_error("Неправильные параметры [%s]"%(argv[1:]))
	if commands:
		pid = fork()
		if pid > 0 and not no_wait:
			Thread(target=runRest).start()
			remain_sec = WAITING_TIME_SEC
			while remain_sec > 0 and not server_close_request:
				time.sleep(1)
				remain_sec-=1
			server.shutdown()
			if not server_close_request:
				send_error("Время ожидания ответа от загрузчика NES закончилось")
		else:
			send_artix(artix,commands,last)
