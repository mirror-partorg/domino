#!/bin/env python
#-*- coding: UTF-8 -*-

import locale
from datetime import datetime


def money(col,cell,model,iter_,column_number):
	value=model.get_value(iter_, column_number)
	if value:
		str_=locale.currency( value, grouping=True)
	cell.set_property("text",str_)
	else: str_=""
def money_no_group(col,cell,model,iter_,column_number):
	value=model.get_value(iter_, column_number)
	if value:
		str_=locale.currency( value, grouping=False )
	else: str_=""
	cell.set_property("text",str_)
def money_no_symbol(col,cell,model,iter_,column_number):
	value=model.get_value(iter_, column_number)
	if value:
		str_=locale.currency( value, grouping=True, symbol=False )
	else: str_=""
	cell.set_property("text",str_)
def money_no_group_no_symbol(col,cell,model,iter_,column_number):
	value=model.get_value(iter_, column_number)
	if value:
		str_=locale.currency( value, grouping=False, symbol=False )
	else: str_=""
	cell.set_property("text",str_)
def quantity(col,cell,model,iter_,column_number):
	value=round(model.get_value(iter_, column_number),3)
	ivalue=int(value)
	if value:
		str_="{}".format(ivalue == value and ivalue or value)
	else: str_=""
	cell.set_property("text",str_)
def timestamp(col,cell,model,iter_,column_number):
	value=datetime.fromtimestamp(model.get_value(iter_, column_number))
	str_=value.strftime("%d.%m.%Y %H:%M:%S")
	cell.set_property("text",str_)
def timestamp_date(col,cell,model,iter_,column_number):
	value=datetime.fromtimestamp(model.get_value(iter_, column_number))
	str_=value.strftime("%d.%m.%Y")
	cell.set_property("text",str_)
def timestamp_time(col,cell,model,iter_,column_number):
	value=datetime.fromtimestamp(model.get_value(iter_, column_number))
	str_=value.strftime("%H:%M:%S")
	cell.set_property("text",str_)
def timestamp_date_human(col,cell,model,iter_,column_number):
	value=datetime.fromtimestamp(model.get_value(iter_, column_number))
	str_=value.strftime("%d %b %y")
	cell.set_property("text",str_)
