#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import re

text=open("/home/mirror/work/domino/src/include/domino_db.h1").read()
s=re.compile("typedef\s+struct\s+\w*\s*\{[\w,\s,\;,\,\[,\]]+\}\s+(?P<name>\w+)\_(?P<type>rec|key\d+)\_t",re.M+re.S)
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
	if Type == "Rec":
		ClassName=Name
	else:
		ClassName=Name+Type
	structName=name+"_"+type_+"_t"
	r = [ClassName]*8
	#r.append(structName)
	print("""
static PyTypeObject %sType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.%s",             /*tp_name*/
    sizeof(%s),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)%s_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "%s objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    %s_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)%s_init,      /* tp_init */
    0,                         /* tp_alloc */
    %s_new,                 /* tp_new */
};"""%tuple(r))
