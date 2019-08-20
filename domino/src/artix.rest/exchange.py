import httplib2
import urllib
import base64
import json
import time

class ArtixExchange:
	def __init__(self,shopcode,hostname,port=28080,user="admin",password="admin"):
		self.host=hostname
		self.shopcode=shopcode
		self.user=user
		self.port=port
		self.password=password
	def send(self,commands):
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
