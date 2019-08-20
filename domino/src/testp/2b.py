#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import re
med=re.compile("\s*(?P<type>\w+)\s+(?P<name>\w+)\[(?P<size>\w+)\](\[(?P<size2>\w+)\]\s*;|\s*;)")
med1=re.compile("\s*(?P<type>\w+)\s+(?P<name>\w+)\s*;")

def make_method(ClassName,structName,st):
	i=0
	res=""
	get_str="""{"get_%s", (PyCFunction)%s_get_%s, METH_NOARGS,"do_alias_get_%s"},
	"""
	set_str="""{"set_%s", (PyCFunction)%s_set_%s, METH_VARARGS|METH_KEYWORDS,"do_alias_get_%s"},
	"""
	for l in st.split("\n"):
		i=i+1
		if re.match("\s*$",l):
			continue
		dd=med.match(l)
		size=1
		if not dd:
			dd=med1.match(l)
			if not dd:
				print("ERROR ",l,i);
				continue
		else:
			try: size=int(dd.group("size"))
			except: size=-1
		name=dd.group("name")
		tt=dd.group("type")
		res=res+get_str%(name,ClassName,name,name)
		res=res+set_str%(name,ClassName,name,name)
		"""if tt="BTI_BYTE":
		elif tt="BTI_CHAR":
		elif tt="BTI_CURRENCY":
		elif tt="BTI_LONG":
		elif tt="BTI_REAL":
		elif tt="BTI_SINT":
		elif tt="BTI_ULONG":
		elif tt="BTI_WORD":
		else:
				print("ERROR type",l,i);"""
	return res

text=open("/home/mirror/work/domino/src/include/domino_db.h1").read()
#s=re.compile("typedef\s+struct\s+\w*\s*\{(?P<body>[\w,\s,\;,\,\[,\]]+)\}\s+(?P<name>\w+)\_(?P<type>rec|key\d+)\_t",re.M+re.S)
s=re.compile("typedef\s+struct\s+\w*\s*\{(?P<body>[\w,\s,\;,\,\[,\]]+)\}\s+(?P<name>\w+)\_(?P<type>struct|key\d+)\_t",re.M+re.S)
s2=re.compile("(?P<first>\w+)\_(?P<last>\w+)")
d=s.finditer(text)
i=0
for f in d:
	name=f.group("name")
	type_=f.group("type")
	Type=f.group("type")[0].upper()+f.group("type")[1:]
	d1=s2.match(name)
	if d1:
		Name=d1.group("first")[0].upper()+d1.group("first")[1:]+d1.group("last")[0].upper()+d1.group("last")[1:]
	else:
		Name=name[0].upper()+name[1:]
	if Type == "Struct":
		ClassName=Name
	else:
		ClassName=Name+Type
	structName=name+"_"+type_+"_t"
	r = [ClassName]*8
	#r.append(structName)
	method=make_method(ClassName,structName,f.group("body"))
	i=i+1
	print("""
static PyMethodDef %s_methods[] = {
	%s
 {NULL}  /* Sentinel */
};"""%(ClassName,method))
#	break
