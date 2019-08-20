#!/bin/env python3

from exchange import ArtixExchange
from pydomino import Alias,local_store
from xml.dom.minidom import parse
from sys import argv

ARTIX_UNPACK_PRICE_INDEX=50
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
		bar["packingmeasure"]=domino_units[product.unit]
		bar["packingprice"]=product.price(DOMINO_RETAIL_PRICE_INDEX)
	return bar
def get_product(product,barcode,barcodekey,store_pref):
	if not product.price(DOMINO_RETAIL_PRICE_INDEX): return None
	if product.is_parcel and product.code[6:8] != store_pref: return None
	barcodekey.set_code(product.code)
	if not barcode.get_equal(barcodekey): return None
	data = {"inventcode":(not product.is_parcel and ARTIX_PRODUCT_CODE_PREFIX or "") + product.code,
			"measureCode":domino_units[product.tech_unit or product.unit],
			"isInventItem":True,
			"price":product.price(DOMINO_RETAIL_PRICE_INDEX)*product.coef,
			"barcodes":[get_barcode(product,barcode)],
			"cquant": product.coef,
			"name": product.name}
	if product.coef > 1:
		data["barcodes"][0]["packingmeasure"]=domino_units[product.unit]
		data["barcodes"][0]["packingprice"]=product.price(DOMINO_RETAIL_PRICE_INDEX)
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
if __name__ == "__main__":
	a=Alias()
	store=local_store()
	artix=ArtixExchange(store,"kassa-99")
	commands = []
	#last = get_protocols(a,)
	#if len(argv) > 1 and argv[1] == "all":
	#	commands += [{"command": "clearInventory"}]
	#	commands += add_units()
	#		commands += add_invents(a,store)
	#	else:
		
	
	#commands += [{"command": "clearInventory"}]
	#commands += [{"command": "clearMCashUser"}]
	#commands += [{"command": "clearKbKey"}]
	commands += [{"command": "clearHotKey"}]
	#commands += add_units()
	#commands += add_invents(a,store)
	#commands += add_users(a)
	#commands += add_func_keys("kb.xml")
	commands += add_invent_keys(a,store)
	#print(commands)
	print(artix.send(commands))
