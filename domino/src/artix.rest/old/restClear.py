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

dictionary = []
dictionary.append({ "command": "clearInventory"})

print(dictionary)
print("clearing %s ..."%(serverHost))
body = json.dumps(dictionary)
answer = None
try:
	response, content = http.request(url, method, headers=jsonHeader, body=body)
	if response.status == 200:
		answer = json.loads(content.decode("utf-8"))
	print(response)
except: pass
if answer and answer['success']:
	print("Ok")
else:
	print(answer)