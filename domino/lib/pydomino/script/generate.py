#!/usr/bin/env python
# -*- coding: utf-8 -*-

from domino_class import *
from myutil import *
from datetime import datetime
import re
import os

re_structs=re.compile("typedef\s+struct\s+\w*\s*\{(?P<body>[\w,\s,\;,\,\[,\]]+)\}\s*(?P<name>\w+)\_(?P<type>struct|key\d+)\_t",re.M+re.S)
re_define_int=re.compile("#define\s+(?P<name>\w+)\s+(?P<value>\d+)")
re_key=re.compile("key(?P<number>\d+)")


def parce_header_file(filename):
	text=open(filename).read()
	defines=re_define_int.finditer(text)
	for define in defines:
		name=define.group("name")
		value=define.group("value")
		text=re.sub(r"\[%s\]"%(name), "["+value+"]", text, flags=re.M+re.S)
		
	iters=re_structs.finditer(text)
	objs=dict()
	unlink=[]
	for struct in iters:
		name=struct.group("name")
		type_=struct.group("type")
		is_key=type_ != "struct"
		body=struct.group("body")
		if is_key:
			dd=re_key.match(type_)
			if not dd:
				Error("ERROR key %s %s"%(type_,name));
			key_number=int(dd.group("number"))
		if not is_key:
			objs[name]=[body,dict()]
		else:
			if name in objs:
				objs[name][1][key_number]=body
			else:
				unlink.append([name, key_number,body])
	for i in unlink:
		if i[0] not in objs:
			Error("Не найден объект для ключа %s %d"%(i[0],i[1]))
		objs[i[0]][key_number]=body
	for name in objs:
		obj = objs[name]
		for i in range(max(obj[1])+1):
			if i not in obj[1]:
				Error("У объекта \"%s\" нет ключа %d"%(name,i))
	return objs
def mkdoc(records,path1):
	names=sorted(records.keys())
	html ="""<html xmlns="http://www.w3.org/1999/xhtml" xmlns:html="http://www.w3.org/1999/xhtml"><head><meta http-equiv="Content-Type" content="text/html; charset=UTF-8">"""
	html+="""<link rel="stylesheet" type="text/css" media="all" href="https://www.gnome.org/wp-content/themes/gnome-grass/style.css"></link>"""
	html+="""
</div></div></div><div class="clearfix"></div><div id="container" class="gtk-doc two_columns"><div class="content"><div class="book">
<div class="titlepage">
<div>
<div><table cellpadding="2" width="100%" cellspacing="0" id="top" class="navigation"><tbody><tr><th valign="middle"><p class="title">PyDomino Reference Manual</p></th></tr></tbody></table></div>
<hr></hr>
	"""
	html+="""<br><h2>Alias(name)</h2></br>"""
	for name in names:
		html+="""<li><a href="index.html#%s">%s</a> - описание</li>"""%(name,name)
	for name in names:
		html+="""<div class="refsect2">
				 <a name="%s"></a><h3>%s()</h3>"""%(name,name)
		html+="""<pre class="synopsis">"""
		for field in records[name].fields:
			html+=field.__str__()
		html+="</pre>"
		for key in records[name].keys:
			record=records[name].keys[key]
			html+="""<li><a href="index.html#%s">%s</a> - описание</li>"""%(record.name,record.name)
			html+="""<pre class="synopsis">"""
			for field in record.fields:
				html+="&nbsp;&nbsp;&nbsp;"+field.__str__()
			html+="</pre>"
	html+="""</html>"""
	f=open(path1+"doc/index.html","w+")
	f.write(html)
	f.close()
def mksrc(records, path1, manual_records):
	path=path1+"var"
	f=open(path+"/db.h","wt+")
	for name in records:
		f.write("#include \"%s.h\"\n"%(records[name].ident))
		records[name].mksrc(path)
	f.close()
	f=open(path+"/../src/head.h","wt+")
	for name in manual_records:
		f.write("#include \"%s.h\"\n"%(name))
	f.close()
	f=open(path+"/alias.h","wt+")
	f.write(alias_h.substitute())
	f.close()
	f=open(path+"/alias.c","wt+")
	f.write(alias_header.substitute())
	for name in records:
		f.write(alias_object.substitute(name=name))
		for key in records[name].keys:
			f.write(alias_object.substitute(name=records[name].keys[key].name))
	f.write(alias_method_header.substitute())
	for name in records:
		f.write(alias_method.substitute(name=name))
		for key in records[name].keys:
			f.write(alias_method.substitute(name=records[name].keys[key].name))
	f.write(alias_method_tail.substitute())
	f.write(alias_type.substitute())
	f.close()
	f=open(path+"/util.h","wt+")
	f.write(util_h.substitute())
	f.close()
	f=open(path+"/util.c","wt+")
	f.write(util_c.substitute())
	f.close()
	f=open(path+"/pydomino.c","wt+")
	f.write(pydomino_c_header.substitute())
	for name in records:
		f.write(pydomino_c_class_init.substitute(name=name))
		for key in records[name].keys:
			f.write(pydomino_c_class_init.substitute(name=records[name].keys[key].name))
	for name in manual_records:
		if name not in ["misc"]:
			f.write(pydomino_c_class_init.substitute(name=name_upper(name)))
	#f.write(pydomino_c_class_init.substitute(name="Utils"))
	f.write(pydomino_c_tail.substitute())
	f.close()
	f=open(path+"/files.lst","wt+")
	#f.write("SRCS=")
	#i=0
	names=sorted(records.keys())
	for name in names:
		f.write("./var/%s.c\n"%(records[name].ident))
	for name in manual_records:
		f.write("./src/%s.c\n"%(name))
	f.write("var/alias.c\nvar/pydomino.c\nvar/util.c")#%(path,path,path))
	f.close()
	f=open(path+"/_info.txt","wt+")
	f.write(datetime.now().__str__())
	f.close()

if __name__ == "__main__":
	try:
		objs = parce_header_file("../../include/domino_db.h1")
		path="./"
	except:
		objs = parce_header_file("../../../include/domino_db.h1")
		path="../"
	records=dict()
	i=0
	for name in objs:
		rec = DominoRecord(name, objs[name][0], objs[name][1])
		records[rec.name]=rec
	records["Document"].slaves.append([records["DocumentOrder"],["dtype","dtype1","sklad","document"]])
	records["DocumentOrder"].slaves.append([records["Document"],["dtype","dtype1","sklad","document"]])
	records["Document"].slaves.append([records["DocumentData"],["dtype","sklad","document"]])
	records["DocumentData"].slaves.append([records["Document"],["dtype","sklad","document"]])
	records["Document"].slaves.append([records["DocumentView"],["dtype","sklad","document"]])
	records["DocumentView"].slaves.append([records["Document"],["dtype","sklad","document"]])

	records["Partner"].slaves.append([records["PartnerData"],["g_code","code"]])
	records["PartnerData"].slaves.append([records["Partner"],["g_code","code"]])

	records["Product"].slaves.append([records["ProductData"],["code"]])
	records["ProductData"].slaves.append([records["Product"],["code"]])
	records["Product"].slaves.append([records["ProductView"],["code"]])
	records["ProductView"].slaves.append([records["Product"],["code"]])
	
	records["Class"].slaves.append([records["Group"],["class_id"]])
	records["Group"].slaves.append([records["Class"],["class_id"]])
	records["Class"].slaves.append([records["Subgroup"],["class_id"]])
	records["Subgroup"].slaves.append([records["Class"],["class_id"]])

	records["Group"].slaves.append([records["Subgroup"],["class_id","group_id"]])
	records["Subgroup"].slaves.append([records["Group"],["class_id","group_id"]])
	
	records["DocumentOrder"].keys[0].slaves_keys.append(records["Document"].keys[0])
	records["Document"].keys[0].slaves_keys.append(records["DocumentOrder"].keys[0])
	records["DocumentOrder"].keys[1].slaves_keys.append(records["Document"].keys[0])
	records["Document"].keys[0].slaves_keys.append(records["DocumentOrder"].keys[1])
	records["DocumentOrder"].keys[2].slaves_keys.append(records["Document"].keys[0])
	records["Document"].keys[0].slaves_keys.append(records["DocumentOrder"].keys[2])

	
	manual_records = []
	for f in os.listdir(path+"src/"):
		if f[-2:] == ".h" and f[:-2] != "head":
			manual_records.append(f[:-2])
	mkdoc(records,path)
	mksrc(records,path,manual_records)
