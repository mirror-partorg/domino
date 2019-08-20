#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import re

re_structs=re.compile("typedef\s+struct\s+\w*\s*\{(?P<body>[\w,\s,\;,\,\[,\]]+)\}\s+(?P<name>\w+)\_(?P<type>struct|key\d+)\_t",re.M+re.S)
re_name_long=re.compile("(?P<first>\w+)\_(?P<last>\w+)")

med=re.compile("\s*(?P<type>\w+)\s+(?P<name>\w+)\[(?P<size>\w+)\](\[(?P<size2>\w+)\]\s*;|\s*;)")
med1=re.compile("\s*(?P<type>\w+)\s+(?P<name>\w+)\s*;")


def first_upper(s):
	return s[0].upper()+s[1:]
def name_upper(s):
	d=re_name_long.match(s)
	if d: return first_upper(d.group("first"))+first_upper(d.group("last"))
	else: return first_upper(s)

if __name__ == "__main__":
	text=open("/home/mirror/work/domino/src/include/domino_db.h1").read()
	iters=re_structs.finditer(text)
	i=0
	classes=dict()
	for struct in iters:
		name=struct.group("name")
		type_=struct.group("type")
		body=struct.group("body")
		if type_ != "struct":
			Type=first_upper(type_)
		else: Type = None
		Name=name_upper(name)
		print(Name,Type)
		if not Type:
			classes[Name] = [name+"_rec_t", body, []]
		else:
			classes[Name][2].append([Type,type_+"_t", body])
		#ClassName=
		i=i+1
	print(i)

	"""Name=name[0].upper()+name[1:]
	if Type == "Struct":
		ClassName=Name
	else:
		ClassName=Name+Type
	structName=name+"_"+type_+"_t"
	r = [ClassName]*8
	#r.append(structName)
	method=make_method(ClassName,structName,f.group("body"))
	i=i+1
	print("
static PyMethodDef %s_methods[] = {
	%s
 {NULL}  /* Sentinel */
};"%(ClassName,method))
#	break"""

