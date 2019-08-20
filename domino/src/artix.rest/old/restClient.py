#!/bin/env python3

import httplib2
import urllib
import base64
import json
import time
from pydomino import Alias


dictionaryId = str(time.time())
shopcode = "5"
serverHost = 'kassa-99'
#'localhost'
serverHttpPort = 28080
login = b'admin:admin'

http = httplib2.Http(".cache")
base64string = base64.encodestring(login)
authheader =  "Basic %s" % "YWRtaW46YWRtaW4="
jsonHeader = {'Content-Type':'application/json', "Accept": "application/json", "Authorization": authheader}
method = 'POST'

url = 'http://%s:%s/aifexchange/loaddicts?shopcode=%s&dictionaryid=%s' % (serverHost, serverHttpPort, shopcode, dictionaryId)

print("Reading product ...")

a=Alias()
kb,b,kp,p = a.BarcodeKey0(),a.Barcode(),a.ProductKey4(),a.Product()
dictionary = [{"command": "clearInventory"}]
f=b.get_ge(kb)
i=0
while f:
	kp.set([b.code])
	p.get_equal(kp)
	if p.price(4) > 0:
		#name=p.name.encode,"utf-8")
		#print(name.decode("utf-8"))
		#print(name.decode("cp1252"))
		#print(name.decode("utf-8"))
		name = "1 "+p.name#.encode("cp1251")
		command = {"command":"addInventItem", 
		"invent":{ "deptCode":1, "measureCode":"796", "minPrice":0, 
		   "inventcode":p.code,
		    "isInventItem":True,
		    "price":p.price(4)*p.coef,
            "taxGroupCode":2,
            "isInvent":True,
            "discAutoScheme":0,
            "barcode":b.barcode,
            "extendedOptions":"extendedOptions",
            #"rtext":p.name, 
            "name": name}
           }
		s={"invent": { "inventcode": p.code, "name":name,
	     "barcode": b.barcode,
	     "price": p.price(4)*p.coef,
	     "isInventitem":True,
	     
	     "deptCode":1, 
	     "measureCode":"796", 
	     "minPrice":0, 
	     "taxGroupCode":2, 
	     "isInvent":True, 
	     "discAutoScheme":0, 
	     "extendedOptions":"extendedOptions",
	     "rtext":"rtext %s" % i,
	     
	   }}
		if p.coef > 1:
		  command["invent"]["additionalprices"]=[{"pricecode":7,"price":p.price(4)}]
		i+=1
		#if i > 100:
		#	break
		dictionary.append(command)
	f=b.get_next(kb)
#print(dictionary)
#countRows = 10
#dictionary = []
#for i in range(countRows):
#    command = {"command":"addInventItem", "invent":{ "deptCode":1, "measureCode":"796", "minPrice":0, "inventcode":"0000000007%s" % i, "isInventItem":True, "price":130+i,
#            "taxGroupCode":2, "isInvent":True, "discAutoScheme":0, "barcode":"8718343465%s" % i, "extendedOptions":"extendedOptions", "rtext":"rtext %s" % i, "name":"Name %s" % i } }
#    dictionary.append(command)

print("Loadding to %s ..."%(serverHost))
#for i in dictionary:
#	print(i,",")
#dsfddfs
body = json.dumps(dictionary)#.decode("cp1251"))
answer = None
try:
	response, content = http.request(url, method, headers=jsonHeader, body=body)
	if response.status == 200:
		answer = json.loads(content.decode("cp1251"))
	print(response)
except: pass
if answer and answer['success']:
	print("Ok")
else:
	print(answer)