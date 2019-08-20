#!/bin/env python2.7

import httplib2
import json, base64
import json


http = httplib2.Http(".cache")
base64string = base64.encodestring('%s:%s' % ('admin', 'admin'))[:-1]
authheader =  "Basic %s" % base64string
jsonHeader = {'Content-Type':'application/json', "Accept": "application/json", 'shopcode': "111", "Authorization": authheader}
method = 'POST'

url = 'http://192.168.6.199:28080/aifexchange/loaddicts?abc=321'

k = 10
dictionary = []
for i in range(k):
    command = {"command":"addInventItem", "invent":{ "deptCode":1, "measureCode":"796", "minPrice":0, "inventcode":"0000000007%s" % i, "isInventItem":True, "price":130+i,
            "taxGroupCode":2, "isInvent":True, "discAutoScheme":0, "barcode":"1111111%s" % i, "extendedOptions":"extendedOptions", "rtext":"rtext %s" % i, "name":"Name %s" % i } }
    dictionary.append(command)


body = json.dumps(dictionary)
print body[0:30]

http.add_credentials('admin', 'admin')
#http.add_credentials('login', 'login')
response, content = http.request(url, method, headers=jsonHeader, body=body)
if response.status == 200:
    print json.loads(content)
else:
    print response, content