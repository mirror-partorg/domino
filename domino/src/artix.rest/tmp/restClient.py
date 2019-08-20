#!/bin/env python2

import httplib2
import urllib
import base64
import json
import time

dictionaryId = str(time.time())
print('dictionaryId = %s' % dictionaryId)
shopcode = "5"
serverHost = '192.168.6.199'
#'localhost'
serverHttpPort = 28080
login = 'admin'
password = 'admin'

http = httplib2.Http(".cache")
base64string = base64.encodestring('%s:%s' % (login, password))[:]
#[:-1]
authheader =  "Basic %s" % base64string
authheader =  "Basic %s" % "YWRtaW46YWRtaW4="

#authheader =  "Basic admin:admin"
# % base64string
print(authheader)
jsonHeader = {'Content-Type':'application/json', "Accept": "application/json", "Authorization": authheader}
method = 'POST'

url = 'http://%s:%s/aifexchange/loaddicts?shopcode=%s&dictionaryid=%s' % (serverHost, serverHttpPort, shopcode, dictionaryId)
print(url)



countRows = 10
dictionary = []
for i in range(countRows):
    command = {"command":"addInventItem", "invent":{ "deptCode":1, "measureCode":"796", "minPrice":0, "inventcode":"0000000007%s" % i, "isInventItem":True, "price":130+i,
            "taxGroupCode":2, "isInvent":True, "discAutoScheme":0, "barcode":"8718343465%s" % i, "extendedOptions":"extendedOptions", "rtext":"rtext %s" % i, "name":"Name %s" % i } }
    dictionary.append(command)


body = json.dumps(dictionary)

response, content = http.request(url, method, headers=jsonHeader, body=body)
if response.status == 200:
    print(json.loads(content))
else:
    print(response, content)
