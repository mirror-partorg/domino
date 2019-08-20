#!/usr/bin/env python
# -*- coding: utf-8 -*-

import re
import sys

re_name_long=re.compile("(?P<first>\w+)\_(?P<last>\w+)")
re_upper=re.compile("[A-Z|_]+$")


def first_upper(s):
	return s[0].upper()+s[1:]
def name_upper(s):
	d=re_name_long.match(s)
	if d: return first_upper(d.group("first"))+first_upper(d.group("last"))
	else: return first_upper(s)
def name_lower(s):
	r=s[0].lower()
	for l in s[1:]:
		if l.isupper():
			r+="_"+l.lower()
		else:
			r+=l
	return r
def name_is_upper(s):
	for i in s:
		if i.isalpha() and i.islower():
			return False
	return True
def Error(str):
	print(str)
	sys.exit(1)
def ErrorField(struct_name,field_name):
	print("Неизвестное поле \"%s\" для структуры \"%s\""%(field_name, struct_name))
	sys.exit(1)
