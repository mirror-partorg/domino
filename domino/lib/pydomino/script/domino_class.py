#!/usr/bin/env python
#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from templates import *
from myutil import *

re_empty_line=re.compile(r"\s*$")
re_field=re.compile(r"\s*(?P<type>\w+)\s+(?P<name>\w+)(\[(?P<size0>\w+)\](\[(?P<size1>\w+)\]\s*;|\s*;)|\s*;)")
re_field_null=re.compile(r"null\w+")
re_date=re.compile("\w*date\w*")
re_time=re.compile("\w*time\w*")

operations= { "equal":"DO_GET_EQUAL",
              "next" :"DO_GET_NEXT",
              "prev" :"DO_GET_PREVIOUS",
              "gt"   :"DO_GET_GT",
              "ge"   :"DO_GET_GE",
              "lt"   :"DO_GET_LT",
              "le"   :"DO_GET_LE",
              "first":"DO_GET_FIRST",
              "last" :"DO_GET_LAST" }
iter_operations= { "equal":["DO_GET_EQUAL","DO_GET_NEXT"],
              "gt"   :["DO_GET_GT","DO_GET_NEXT"],
              "ge"   :["DO_GET_GE","DO_GET_NEXT"],
              "lt"   :["DO_GET_LT","DO_GET_PREVIOUS"],
              "le"   :["DO_GET_LE","DO_GET_PREVIOUS"],
              "first":["DO_GET_FIRST","DO_GET_NEXT"],
              "last" :["DO_GET_LAST","DO_GET_PREVIOUS"], }
update_operations= { "insert":1,
                     "update":1,
                     "delete":0}


record_with_param=["product","partner","document","otdel","product_data",
                   "sklad","document_prow","product_link","document_order",
                   "user","document_type","prowod2","prowod","partner_data",
                   "region","document_data","subgroup"]
record_with_date=["document","product","discount","saldo","document_order",
                  "prowod2","prowod","partner_data","protocol","sklad"]
record_with_time=["document","product","discount","saldo","document_order",
                  "prowod2","partner_data","protocol"]
record_with_price=["product","document_order"]
record_with_quant=["document_order","stock"]
record_with_sums=["document","prowod2","prowod"]
sized_fields=["sum_include","price","sum","param","debit_sum","credit_sum"]


def byte_parce(struct,ident,c_prefix,size):
	name = c_prefix+ident
	if not size:
		return int_proc(name, "Integer")
	if ident == "sumincl":
		if struct not in ["subaccount", "account"]:
			ErrorField(struct, ident)
		return int_n3_proc(name, "Integer[3]")
	elif ident == "quant":
		if struct not in record_with_quant:
			ErrorField(struct, ident)
		return quant_proc(struct, "Real(%d,%d)"%(struct=="document_order" and 7 or 8,3))
	elif ident == "sk":
		if struct not in ["barcode"]:
			ErrorField(struct, ident)
		return decimal_proc(name,3,0,"Real(3,0)")
	elif ident == "total":
		if struct not in ["barcode"]:
			ErrorField(struct, ident)
		return decimal_proc(name,7,3,"Real(7,3)")
	elif ident == "tech_unit_rate":
		if struct not in ["product"]:
			ErrorField(struct, ident)
		return decimal_proc(name,4,3,"Real(4,3)")
	elif ident in ["debit_sum","credit_sum"]:
		if struct not in ["saldo"]:
			ErrorField(struct, ident)
		return decimal_n3_proc(name,8,2,"Real[3](8,2)")
	elif ident == "sums":
		if struct not in ["prowod2","prowod"]:
			ErrorField(struct, ident)
		if struct == "prowod2":
			return sum_proc(struct,"Real(8,2)")
		else:
			return decimal_n3_proc(name,8,2,"Real[3](8,2)")
	ErrorField(struct, ident)
def char_parce(struct,ident,c_prefix,size):
	name = c_prefix+ident
	if not size:
		return char_proc(name, "Char")
	if ident == "params":
		if struct not in record_with_param:
			ErrorField(struct, ident)
		return param_proc(struct, "String[name]")
	return string_proc(name, "String(%d)"%(size[0]))
def currency_parce(struct,ident,c_prefix,size):
	name = c_prefix+ident
	if not size:
		return currency_proc(name, "Char")
	ErrorField(struct, ident)
def long_parce(struct,ident,c_prefix,size):
	name = c_prefix+ident
	if not size:
		if re_date.match(ident) or ident in ["life_time","month"]:
			if struct in ["shift","checksum","check","realization"]:
				return sqldate_proc(name,"Date")
			if struct not in record_with_date:
				ErrorField(struct, ident)
			return date_proc(name,"Date")
		elif re_time.match(ident):
			if struct in ["shift","checksum","check","realization"]:
				return sqltime_proc(name,"Time")
			if struct not in record_with_time:
				ErrorField(struct, ident)
			return time_proc(name,"Time")
		else:
			return int_proc(name, "Integer")
	ErrorField(struct, ident)
def real_parce(struct,ident,c_prefix,size):
	name = c_prefix+ident
	if not size:
		if struct not in ["realization","shift","checksum","check"]:
			ErrorField(struct, ident)
		return real_proc(name, "Real")
	if ident == "price":
		if struct not in record_with_price:
			ErrorField(struct, ident)
		return price_proc(struct, "Real[]")
		
	ErrorField(struct, ident)
def sint_parce(struct,ident,c_prefix,size):
	name = c_prefix+ident
	if not size:
		return int_proc(name, "Integer")
	ErrorField(struct, ident)
def word_parce(struct,ident,c_prefix,size):
	name = c_prefix+ident
	ErrorField(struct, ident)

domino_type =  {
"BTI_BYTE" : byte_parce, 
"BTI_CHAR" : char_parce,
"BTI_CURRENCY" : currency_parce,
"BTI_LONG" : long_parce,
"BTI_REAL" : real_parce,
"BTI_SINT" : sint_parce,
"BTI_ULONG" : sint_parce,
"BTI_WORD" : word_parce,
}


class DominoField:
	def __init__(self, master, name, ident, type_,size):
		self.master = master
		self.name = name
		self.ident = ident
		self.type_ = type_
		self.size = size
		self.info = None
	def get_info(self):
		if not self.info:
			proc = domino_type[self.type_]
			self.info=proc(self.master.ident, self.ident, self.master.c_prefix, self.size)
		return self.info
	def __str__(self):
		self.get_info()
		if self.name=="Param":
			d="params"
		else:
			d=name_lower(self.name)
		retval = "    %s(%s) %s - [%s]\n"%(self.name,d,self.info.desc, self.ident)
		return retval
class DominoStruct:
	def __init__(self, name, ident, c_name, c_prefix, body, master):
		self.name=name
		self.ident=ident
		self.c_name=c_name
		self.c_prefix=c_prefix
		self.fields = []
		self.master = master
		self.slaves = []
		self.slaves_keys = []

		self.parce(body)
	def __str__(self):
		retval = "  %s (%s %s)\n"%(self.name,self.ident,self.c_name)
		for field in self.fields:
			retval +="%s"%(field)
		return retval
	def parce(self, body):
		for line in body.split("\n"):
			if re_empty_line.match(line): continue
			field=re_field.match(line)
			if not field:
				Error("Ошибка распознования переменной в строке \"%s\""%(line))
			ident = field.group("name")
			if ident in private_field_name: continue
			if name_is_upper(ident): continue
			if re_field_null.match(ident): continue
			type_ = field.group("type")
			name = name_upper(ident)
			if name in field_name_sub:
				name = field_name_sub[name]
			if type_ not in domino_type:
				Error("Неизвестный тип \"%s\""%(type_))
			try: size0=int(field.group("size0") and field.group("size0") or 1)
			except: Error("Неизвестный размер массива в строке \"%s\""%(line))
			try: size1=int(field.group("size1") and field.group("size1") or 1)
			except: Error("Неизвестный размер массива в строке \"%s\""%(line))
			size=None
			if size1 > 1:
				size=[size0,size1]
			elif size0 > 1:
				size=[size0]
			self.fields.append(DominoField(self,name_lower(name),ident,type_,size)) 
	def mksrc(self,f, prefix, keys, index):
		init_vars = ""
		if not self.master:
			init_vars+=init_record.substitute()
		for field in self.fields:
			if field.ident == "params":
				init_vars+=init_params.substitute(ident=self.ident)
			elif field.type_ == "BTI_CHAR":
				if field.size:
					init_vars+= init_string.substitute(name=prefix+field.ident)
				else:
					init_vars+= init_char.substitute(name=prefix+field.ident)
		f.write(class_c_class_init.substitute(name=self.name,init_vars=init_vars,c_name=self.c_name))
		if not self.master:
			f.write(class_c_clone_full.substitute(name=self.name,init_vars=init_vars,c_name=self.c_name))
		else:
			f.write(class_c_clone_key.substitute(name=self.name,init_vars=init_vars,c_name=self.c_name))
		for field in self.fields:
			info=field.get_info()
			if field.ident == "params":
				f.write(class_c_params.substitute(name=self.name,ident=self.ident))
			else:
				if info.getter:
					if field.ident in ["price","quant","sum"] or field.name in ["sum","debit_sum","credit_sum"]:
						f.write(class_c_get_field_header.substitute(name=self.name,fieldname=field.name))
						f.write(info.getter)
						f.write(class_c_get_field_tail.substitute())
					else:
						f.write(class_c_field_header.substitute(name=self.name,fieldname=field.name))
						f.write(info.getter)
						f.write(class_c_field_tail.substitute())
						if field.ident in ["date","time"]:
							f.write(class_c_field_header_raw.substitute(name=self.name,fieldname=field.name))
							#name=
							#print(self.name,field.name,field.ident,field.master)
							f.write(raw_proc("Key" in self.name and field.name or "data."+field.name, "Integer").getter)
							f.write(class_c_field_tail.substitute())
				f.write(class_c_set_field_header.substitute(name=self.name,fieldname=field.name))
				f.write(info.set_src)
				f.write(class_c_set_field_tail.substitute())
		if not self.master and self.ident in record_with_price:
			f.write(class_c_prices.substitute(name=self.name,ident=self.ident,size=(self.ident=="document_order") and "8" or "self->priv->data.total_price"))
		if not self.master and self.ident in record_with_quant:
			f.write(class_c_quants.substitute(name=self.name,ident=self.ident,size=(self.ident=="document_order") and "4" or "6"))
		if not self.master and self.ident in record_with_sums:
			if self.ident == "document":
				f.write(class_c_document_sums.substitute())
			elif self.ident == "prowod":
				f.write(class_c_prowod_sums.substitute())
			else:
				f.write(class_c_prowod2_sums.substitute())
		if not self.master and self.ident == "saldo":
			f.write(class_c_saldo_sums.substitute())
		# operation
		for oper in operations:
			if keys:
				f.write(class_c_operation_header.substitute(name=self.name,oper=oper))
			else:
				f.write(class_c_operation_header_key.substitute(name=self.name,oper=oper))
			if keys:
				for i in keys:
					f.write(class_c_operation_keys.substitute(name=self.name,index=i,ident=self.ident,oper_id=operations[oper]))
				f.write(class_c_operation_keys_tail.substitute())
			else:
				f.write(class_c_operation.substitute(ident=self.ident, index=index,oper_id=operations[oper]))
			f.write(class_c_operation_tail.substitute())
		for oper in iter_operations:
			if keys:
				f.write(class_c_operation_iter_header.substitute(name=self.name,oper=oper))
				for i in keys:
					f.write(class_c_operation_iter_keys_1.substitute(name=self.name,index=i,ident=self.ident,oper_id=iter_operations[oper][0]))
				f.write(class_c_operation_iter_keys_tail_1.substitute())
				f.write(class_c_operation_iter_body_get.substitute())
				for i in keys:
					f.write(class_c_operation_iter_cmp_header.substitute(name=self.name,index=i,ident=self.ident))
					depth=1
					for field in self.keys[i].fields:
						f.write(class_c_operation_iter_cmp.substitute(name=self.name,index=i,depth=depth,ident=self.ident,field=field.ident))
						depth+=1
					f.write(class_c_operation_iter_cmp_next.substitute())
				f.write(class_c_operation_iter_keys_next_tail_1.substitute())
				f.write(class_c_operation_iter_data.substitute(name=self.name))
				for i in  keys:
					f.write(class_c_operation_iter_key_next.substitute(name=self.name,index=i,ident=self.ident,oper_id_next=iter_operations[oper][1]))
				f.write(class_c_operation_iter_keys_next_tail_1.substitute())
				f.write(class_c_operation_iter_tail.substitute())
			else:
				f.write(class_c_operation_iter_header_key.substitute(name=self.name,oper=oper,ident=self.ident,index=index,oper_id=iter_operations[oper][0]))
				depth=1
				for field in self.fields:
					f.write(class_c_operation_iter_cmp_key.substitute(depth=depth,field=field.ident))
					depth+=1
				f.write(class_c_operation_iter_tail_key.substitute(name=self.name,oper=oper,ident=self.ident,index=index,oper_id_next=iter_operations[oper][1]))
		if keys:
			for oper in update_operations:
				f.write(class_c_update.substitute(name=self.name,oper=oper,ident=self.ident,suffix=(update_operations[oper] and ", self->priv" or "")))
		if self.master:
			f.write(class_c_cmp_header.substitute(name=self.name))
			f.write(class_c_cmp_type.substitute(name=self.name))
			for i in range(len(self.fields)):
				if self.fields[i].type_ not in ["BTI_CHAR", "BTI_LONG", "BTI_BYTE", "BTI_ULONG", "BTI_SINT"]:
					print("Error key field type is %s %s"%(self.fields[i].type_,self.fields[i].ident))
				if self.fields[i].type_ == "BTI_BYTE" and self.fields[i].size:
					print("Error key field type is %s %s"%(self.fields[i].type_,self.fields[i].ident))
				if self.fields[i].type_ == "BTI_CHAR":
					f.write(class_c_cmp_field.substitute(name=self.name,index=i+1,field=self.fields[i].ident))
				else:
					f.write(class_c_cmp_int_field.substitute(name=self.name,index=i+1,field=self.fields[i].ident))
			f.write(class_c_cmp_type_tail.substitute())
			for slave in self.slaves_keys:
				f.write(class_c_cmp_type.substitute(name=slave.name))
				for i in range(min(len(self.fields),len(slave.fields))):
					if self.fields[i].type_ not in ["BTI_CHAR", "BTI_LONG", "BTI_BYTE", "BTI_ULONG", "BTI_SINT"]:
						print("Error key field type is %s %s"%(self.fields[i].type_,self.fields[i].ident))
					if self.fields[i].type_ == "BTI_BYTE" and self.fields[i].size:
						print("Error key field type is %s %s"%(self.fields[i].type_,self.fields[i].ident))
					if self.fields[i].type_ == "BTI_CHAR":
						f.write(class_c_cmp_field.substitute(name=slave.name,index=i+1,field=self.fields[i].ident))
					else:
						f.write(class_c_cmp_int_field.substitute(name=slave.name,index=i+1,field=self.fields[i].ident))
				f.write(class_c_cmp_type_tail.substitute())
			f.write(class_c_cmp_tail.substitute())
		
		# set alias func
		f.write(class_c_set_alias.substitute(name=self.name))
		# set func
		f.write(class_c_set_header.substitute(name=self.name))
		if keys:
			for i in keys:
				f.write(class_c_set_check_type.substitute(name="%sKey%d"%(self.name,i)))
				for f0 in self.fields:
					for f1 in self.keys[i].fields:
						if f0.ident==f1.ident:
							f.write(class_c_set_check_type_copy.substitute(ident_master="data."+f0.ident,ident_slave=f0.ident,name="%sKey%d"%(self.name,i)))
				f.write(class_c_set_check_type_tail.substitute())
		if self.master:
			f.write(class_c_set_check_type.substitute(name=self.master.name))
			for f0 in self.fields:
				for f1 in self.master.fields:
					if f0.ident==f1.ident:
						f.write(class_c_set_check_type_copy.substitute(ident_master=f0.ident,ident_slave="data."+f0.ident,name=self.master.name))
			f.write(class_c_set_check_type_tail.substitute())
			f.write(class_c_set_check_list.substitute())
			index=0 
			for f0 in self.fields:
				f.write(class_c_set_check_list_copy.substitute(name=self.name,field=f0.name,index=index))
				index+=1
			f.write(class_c_set_check_list_tail.substitute())
		if self.master:
			slaves = self.master.slaves
		else:
			slaves = self.slaves
		for sl in slaves:
			slave = sl[0]
			f.write(class_c_set_check_type.substitute(name=slave.name))
			for f0 in self.fields:
				for f1 in sl[1]:
					if f0.ident==f1:
						ident_master = ((not self.master) and "data." or "")+f0.ident
						ident_slave = ((not slave.master) and "data." or "")+f0.ident
						f.write(class_c_set_check_type_copy.substitute(ident_master=ident_master,ident_slave=ident_slave,name=slave.name))
			f.write(class_c_set_check_type_tail.substitute())
			for i in slave.keys:
				slave_key = slave.keys[i]
				f.write(class_c_set_check_type.substitute(name="%sKey%d"%(slave.name,i)))
				for f0 in self.fields:
					for f1k in slave_key.fields:
						if f1k.ident not in sl[1]: continue
						f1=f1k.ident
						if f0.ident==f1:
							ident_master = ((not self.master) and "data." or "")+f0.ident
							ident_slave = ((not slave_key.master) and "data." or "")+f0.ident
							f.write(class_c_set_check_type_copy.substitute(ident_master=ident_master,ident_slave=ident_slave,name=slave_key.name))
				f.write(class_c_set_check_type_tail.substitute())
		f.write(class_c_set_check_dict.substitute())
		for f0 in self.fields:
			if f0.name == "param" :
				f.write(class_c_set_check_dict_copy.substitute(name=self.name,field=f0.name,ident="param"))
				f.write(class_c_set_check_dict_copy.substitute(name=self.name,field="params",ident="params"))
			elif f0.name == "quant":
				f.write(class_c_set_check_dict_copy.substitute(name=self.name,field=f0.name,ident="quant"))
				f.write(class_c_set_check_dict_copy.substitute(name=self.name,field="quants",ident="quants"))
			elif f0.name == "price":
				f.write(class_c_set_check_dict_copy.substitute(name=self.name,field=f0.name,ident="price"))
				f.write(class_c_set_check_dict_copy.substitute(name=self.name,field="prices",ident="prices"))
			elif f0.name == "sum":
				f.write(class_c_set_check_dict_copy.substitute(name=self.name,field=f0.name,ident="sum"))
				f.write(class_c_set_check_dict_copy.substitute(name=self.name,field="sums",ident="sums"))
			elif f0.name in sized_fields:
				f.write(class_c_set_check_dict_copy.substitute(name=self.name,field=f0.name,ident=f0.name))
			else:
				f.write(class_c_set_check_dict_copy.substitute(name=self.name,field=f0.name,ident=f0.name))
		if self.name == "Document":
			f.write(class_c_set_check_dict_copy.substitute(name=self.name,field="sum",ident="sum"))
			f.write(class_c_set_check_dict_copy.substitute(name=self.name,field="sums",ident="sums"))
		f.write(class_c_set_check_dict_tail.substitute(name=self.name))

		f.write(class_c_set_tail.substitute())
		
		# __str__ func
		f.write(class_c_str_header.substitute(name=self.name))
		for field in self.fields:
			if field.ident == "price" and self.name != "Check":
				f.write(class_c_str_field_price.substitute(name=self.name))
			elif field.ident == "quant" and self.name != "Check":
				f.write(class_c_str_field_quant.substitute(name=self.name))
			elif field.ident == "sums" and self.name != "Check":
				f.write(class_c_str_field_sum.substitute(name=self.name))
			elif field.ident == "params":
				f.write(class_c_str_field_params.substitute(name=self.name))
			else:
				if field.size and field.type_ != "BTI_CHAR":
					if field.name == "sum_include" and self.name in ["Subaccount","Account"]:
						f.write(class_c_str_field_suminclude.substitute(name=self.name,ident=self.ident))
					elif field.name == "coef" and self.name in ["Product"]:
						f.write(class_c_str_field_coef.substitute(name=self.name,ident=self.ident))
					elif field.name in ["debit_sum", "credit_sum"] and self.name == "Saldo":
						f.write(class_c_str_field_debit_sum.substitute(name=field.name,ident=field.ident))
					elif field.name == "total" and self.name == "Barcode":
						f.write(class_c_str_field_total.substitute(name=field.name,ident=field.ident))
					elif field.name == "discount" and self.name == "Barcode":
						f.write(class_c_str_field_discount.substitute(name=field.name,ident=field.ident))
					else:
						print("Error field %s %s"%(field.name, self.name))
				else:
					f.write(class_c_str_field.substitute(name=field.name,value="%s_%s(self,NULL)"%(self.name,field.name)))
		f.write(class_c_str_tail.substitute())
		f.write(class_c_getset_header.substitute(name=self.name))
		for field in self.fields:
			info=field.get_info()
			#if info.getter:
			if field.ident == "params":
				f.write(class_c_getset_field.substitute(name=self.name,field="get_params",field_lower="params"))
			elif field.ident == "quant":
				f.write(class_c_getset_field.substitute(name=self.name,field="get_quants",field_lower="quants"))
			elif field.ident == "price":
				f.write(class_c_getset_field.substitute(name=self.name,field="get_prices",field_lower="prices"))
			elif field.ident == "sum" or field.name == "sum":
				f.write(class_c_getset_field.substitute(name=self.name,field="get_sums",field_lower="sums"))
			elif field.ident == "debit_sum":
				f.write(class_c_getset_field.substitute(name=self.name,field="get_debit_sums",field_lower="debit_sums"))
			elif field.ident == "credit_sum":
				f.write(class_c_getset_field.substitute(name=self.name,field="get_credit_sums",field_lower="crebit_sums"))
			else:
				f.write(class_c_getset_field.substitute(name=self.name,field=field.name,field_lower=name_lower(field.name)))
				if field.ident in ["date","time"]:
					f.write(class_c_getset_field_raw.substitute(name=self.name,field=field.name,field_lower=name_lower(field.name)))
		if self.name=="Document":
			f.write(class_c_getset_field.substitute(name=self.name,field="get_sums",field_lower="sums"))
		f.write(class_c_getset_tail.substitute())
		f.write(class_c_methods_header.substitute(name=self.name))
		if self.master:
			f.write(class_c_methods_cmp.substitute(name=self.name))
		if not self.master and self.ident in record_with_price:
			f.write(class_c_methods_prices.substitute(name=self.name))
		if not self.master and self.ident in record_with_quant:
			f.write(class_c_methods_quants.substitute(name=self.name))
		if not self.master and self.ident in record_with_sums:
			f.write(class_c_methods_sums.substitute(name=self.name))
			if self.ident == "document":
				f.write(class_c_methods_field.substitute(field="sum",name="Document",field_lower="sum"))
		for field in self.fields:
			if field.ident == "params":
				f.write(class_c_methods_param.substitute(name=self.name))
			else:
				f.write(class_c_methods_field.substitute(name=self.name,field=field.name,field_lower=name_lower(field.name)))
		if self.ident == "saldo":
			f.write(class_c_methods1_field.substitute(field="credit_sum",name="Saldo",field_lower="credit_sum"))
			f.write(class_c_methods1_field.substitute(field="debit_sum",name="Saldo",field_lower="debit_sum"))
		for oper in operations:
			f.write(class_c_methods_oper.substitute(name=self.name,oper=oper))
		for oper in iter_operations:
			f.write(class_c_methods_oper_iter.substitute(name=self.name,oper=oper))
		if not self.master:
			for oper in update_operations:
				f.write(class_c_methods_oper_update.substitute(name=self.name,oper=oper))
		f.write(class_c_methods_tail.substitute())
		f.write(class_c_type.substitute(name=self.name))

class DominoRecord(DominoStruct):
	def __init__(self, ident, body, keys_body):
		self.keys = {}
		self.master = None
		self.slaves = []
		name=name_upper(ident)
		if name in object_name_sub:
			name=object_name_sub[name]
		DominoStruct.__init__(self,name,ident, "%s_rec_t"%(ident), "data.", body, None)
		for i in keys_body:
			self.keys[i] = DominoStruct("%sKey%d"%(self.name,i), self.ident, "%s_key%d_t"%(self.ident,i), "", keys_body[i], self)
	def __str__(self):
		retval="%s\n%s\n"%(self.name,DominoStruct.__str__(self))
		for name in self.keys:
			retval+="%s"%(self.keys[name])
		return retval
	def mksrc(self,path):
		header=open("%s/%s.h"%(path,self.ident), "wt+")
		header.write(class_h_header.substitute(name=self.name))
		header.write(class_h_struct.substitute(name=self.name,ident=self.ident,suffix="rec"))
		for i in self.keys:
			key=self.keys[i]
			header.write(class_h_struct.substitute(name=key.name,ident=key.ident,suffix="key%d"%(i)))
		header.write(class_h_tail.substitute(name=self.name))
		header.close
		src=open("%s/%s.c"%(path,self.ident), "wt+")
		src.write(class_c_header.substitute(ident=self.ident))
		DominoStruct.mksrc(self,src, "data.", self.keys.keys(), None)
		for i in self.keys:
			key=self.keys[i]
			key.mksrc(src,"",None,i)
		src.close()

