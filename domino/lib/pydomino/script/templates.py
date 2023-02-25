
#!/usr/bin/env python
# -*- coding: utf-8 -*-
from string import Template

class FieldInfo:
	def __init__(self,getter,proc,desc):
		self.getter=getter
		self.set_src=proc
		self.desc = desc

object_name_sub={
"Checksum": "CheckSum",
"Sklad":"Store",
"Otdel":"Division",
}
private_field_name=[
"nprice",
"nquant",
"nsum",
]
field_name_sub={
"Analitother":"AnalitOther",
"Analitpartner":"AnalitParnter",
"Analitsub":"AnalitSubaccount",
"BarCode":"Barcode",
"BaseParcel":"IsParcel",
"Credit_pCode": "CreditPartnerCode",
"Credit_p_gCode": "CreditPartnerRegionCode",
"Debit_pCode": "DebitPartnerCode",
"Debit_p_gCode": "DebitPartnerRegionCode",
"CycleFl": "Cyclical",
"DateB": "DateStart",
"DateN": "DateFirst",
"DParam": "DocumentParam",
"Dtype": "DocumentType",
"Dtype1": "DocumentOperation",
"Fio" : "FIO",
"GCode": "RegionCode",
"Tech_unitRate":"Coef",
"MCode":"MasterCode",
"SCode":"SlaveCode",
"MDtype":"MasterDocumentType",
"MDocument":"MasterDocument",
"SDtype":"SlaveDocumentType",
"SDocument":"SlaveDocument",
"MfrCode" :"ManufactorCode",
"MfrRegion" :"ManufactorRegionCode",
"Mol":"MOL",
"Mol1":"MOLSecondary",
"Sklad":"Store",
"MSklad":"MasterStore",
"SSklad":"SlaveStore",
"Nsum":"TotalSum",
"Lastvalue":"LastValue",
"Oper":"Operation",
"Othername":"OtherName",
"Otdel":"Division",
"Param":"Parameter",
"SupplierRegion":"SupplierRegionCode",
"PCode":"PartnerCode",
"PDoc":"PartnerDocument",
"PDate":"PartnerDocumentDate",
"PTime":"PartnerDocumentTime",
"P_gCode":"PartnerRegionCode",
"Positivesaldo": "PositiveSaldo",
"ProductCode": "Code",
"Ptype": "ProductType",
"Pwd": "Password",
"Qty":"QTY",
#"Quantity":"Quant",
"SaldoD":"SaldoDebit",
"SaldoK":"SaldoCredit",
"Sk":"Discount",
"Sname":"ShortName",
"Stat":"State",
"Sumincl":"SumInclude",
"TimeB":"TimeStart",
"Type_":"Type",
"TypeDoc":"DocumentType",
"TypeOper":"OperationType",
"Userini":"UserProfile",
"Params":"Param",
"Itog":"Total",
"Klass":"Class",
"Type":"RecordType",
"Sums":"Sum",
}


temp_vars = [
"fieldname",
"name",
"size",
"fracplaces",
]
get_int = Template(
"""
    result = MyLong_FromLong(self->priv->$fieldname);
"""
)
get_raw = Template(
"""
    result = MyLong_FromLong(self->priv->$fieldname);
"""
)

set_int = Template(
"""
    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set $fieldname");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->$fieldname = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->$fieldname = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set $fieldname");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->$fieldname);
"""
)
get_real = Template(
"""
    result = PyFloat_FromDouble(self->priv->$fieldname);
"""
)
set_real = Template(
"""
    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set $fieldname");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->$fieldname = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->$fieldname = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set $fieldname");
            return NULL;
        }
    }
    //result = PyFloat_FromDouble(self->priv->$fieldname);
"""
)
get_currency = Template(
"""
    result = PyFloat_FromDouble(self->priv->$fieldname/10000.);
"""
)
set_currency = Template(
"""
    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set $fieldname");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->$fieldname = MyLong_AsLong(value)*10000;
        else
        if ( PyFloat_Check(value) )
             self->priv->$fieldname = PyFloat_AsDouble(value)*10000;
        else {
            do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set $fieldname");
            return NULL;
        }
    }
    //result = PyFloat_FromDouble(self->priv->$fieldname/10000.);
"""
)
get_char = Template(
"""
    char buf[2];
    buf[0] = self->priv->$fieldname;
    buf[1] = '\\0';
    char *res=do_text(self->alias->alias, buf);
    result = MyString_FromString(res);
    do_free(res);
"""
)
set_char = Template(
"""
    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set $fieldname");
        return NULL;
    }
    if ( value ) {
        char buf[1];
        do_text_set(self->alias->alias, buf, value);
        self->priv->$fieldname=buf[0];
    }
    //char buf[2];
    //buf[0] = self->priv->$fieldname;
    //buf[1] = '\\0';
    //char *res=do_text(self->alias->alias, buf);
    //result = MyString_FromString(res);
    //do_free(res);
"""
)
get_string = Template(
"""
    char *res=do_text(self->alias->alias, self->priv->$fieldname);
    result = MyString_FromString(res);
    do_free(res);
"""
)
set_string = Template(
"""
    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set $fieldname");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->$fieldname, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->$fieldname);
    result = MyString_FromString(res);
    do_free(res);*/
"""
)
get_int_n1 = Template(
"""
    result = PyTuple_New(3);
    PyTuple_SET_ITEM(result, 0, MyLong_FromLong(self->priv->$fieldname[0]));
    PyTuple_SET_ITEM(result, 1, MyLong_FromLong(self->priv->$fieldname[1]));
    PyTuple_SET_ITEM(result, 2, MyLong_FromLong(self->priv->$fieldname[2]));
"""
)
set_int_n = Template(
"""
    PyObject *value = NULL;
    int number;
    static char *kwlist[] = {"number","value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|O", kwlist, &number, &value) ) {
        do_log(LOG_ERR, "Invalid arguments \\"number\\" \\"value\\": wrong type. set $fieldname");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->$fieldname[number] = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->$fieldname[number] = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set $fieldname");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->$fieldname[number]);
"""
)
get_price = Template(
"""
    static char *kwlist[] = {"number", NULL};
    int number;
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &number) ) {
        do_log(LOG_ERR, "Invalid argument \\"number\\": wrong type. get price");
        return NULL;
    }
    double price;
    price = do_${name}_price(self->alias->alias, self->priv, number);
    result = PyFloat_FromDouble(price);
"""
)
set_price = Template(
"""
    PyObject *value = NULL;
    int number;
    static char *kwlist[] = {"number", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|O", kwlist, &number, &value) ) {
        do_log(LOG_ERR, "Invalid arguments \\"number\\" \\"value\\": wrong type. set price");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             do_${name}_price_set(self->alias->alias,self->priv, number, MyLong_AsLong(value));
        else
        if ( PyFloat_Check(value) )
             do_${name}_price_set(self->alias->alias,self->priv, number, PyFloat_AsDouble(value));
        else {
            do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set price");
            return NULL;
        }
    }
    /*double price;
    price = do_${name}_price(self->alias->alias, self->priv, number);
    result = PyFloat_FromDouble(price);*/
"""
)

get_quant = Template(
"""
    static char *kwlist[] = {"number", NULL};
    int number;
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &number) ) {
        do_log(LOG_ERR, "Invalid argument \\"number\\": wrong type. get quant");
        return NULL;
    }
    double quant;
    quant = do_${name}_quant(self->alias->alias, self->priv, number);
    result = PyFloat_FromDouble(quant);
"""
)
get_quant11 = Template(
"""
    result = PyTuple_New(4);
    PyTuple_SET_ITEM(result, 0, PyFloat_FromDouble(do_${name}_quant(self->alias->alias, self->priv, 1)));
    PyTuple_SET_ITEM(result, 1, PyFloat_FromDouble(do_${name}_quant(self->alias->alias, self->priv, 2)));
    PyTuple_SET_ITEM(result, 2, PyFloat_FromDouble(do_${name}_quant(self->alias->alias, self->priv, 3)));
    PyTuple_SET_ITEM(result, 3, PyFloat_FromDouble(do_${name}_quant(self->alias->alias, self->priv, 4)));
"""
)
set_quant = Template(
"""
    PyObject *value=NULL;
    int number;
    static char *kwlist[] = {"number", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|O", kwlist, &number, &value) ) {
        do_log(LOG_ERR, "Invalid arguments \\"number\\" \\"value\\": wrong type. set quant");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             do_${name}_quant_set(self->alias->alias,self->priv, number, MyLong_AsLong(value));
        else
        if ( PyFloat_Check(value) )
             do_${name}_quant_set(self->alias->alias,self->priv, number, PyFloat_AsDouble(value));
        else {
            do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set quant");
            return NULL;
        }    
    }
    /*double quant;
    quant = do_${name}_quant(self->alias->alias, self->priv, number);
    result = PyFloat_FromDouble(quant);*/
"""
)
set_sum = Template(
"""
    PyObject *value=NULL;
    int number;
    static char *kwlist[] = {"number", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|O", kwlist, &number, &value) ) {
        do_log(LOG_ERR, "Invalid arguments \\"number\\" \\"value\\": wrong type. set sum");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             do_${name}_sum_set(self->alias->alias,self->priv, number, MyLong_AsLong(value));
        else
        if ( PyFloat_Check(value) )
             do_${name}_sum_set(self->alias->alias,self->priv, number, PyFloat_AsDouble(value));
        else {
            do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set sum");
            return NULL;
        }    
    }
    /*double sum;
    sum = do_${name}_sum(self->alias->alias, self->priv, number);
    result = PyFloat_FromDouble(sum);*/
"""
)
get_sum = Template(
"""
    static char *kwlist[] = {"number", NULL};
    int number;
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &number) ) {
        do_log(LOG_ERR, "Invalid argument \\"number\\": wrong type. get sum");
        return NULL;
    }
    double sum;
    sum = do_${name}_sum(self->alias->alias, self->priv, number);
    result = PyFloat_FromDouble(sum);
"""
)
get_decimal = Template(
"""
    result=PyFloat_FromDouble(do_pdecimal(self->priv->$fieldname, $size, $fracplaces));
"""
)

set_decimal = Template(
"""
    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set $fieldname");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             do_pdecimal_set(self->priv->$fieldname, $size, $fracplaces,  MyLong_AsLong(value));
        else
        if ( PyFloat_Check(value) )
             do_pdecimal_set(self->priv->$fieldname, $size, $fracplaces,  PyFloat_AsDouble(value));
        else {
            do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set $fieldname");
            return NULL;
        }    
    }
    //result=PyFloat_FromDouble(do_pdecimal(self->priv->$fieldname, $size, $fracplaces));
"""
)
get_decimal_n = Template(
"""
    static char *kwlist[] = {"number", NULL};
    int number;
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &number) ) {
        do_log(LOG_ERR, "Invalid argument \\"number\\": wrong type. get $fieldname");
        return NULL;
    }
    result=PyFloat_FromDouble(do_pdecimal(self->priv->$fieldname[number], $size, $fracplaces));
"""
)
get_decimal_n11 = Template(
"""
    result=PyTuple_New(3);
    PyTuple_SET_ITEM(result, 0, PyFloat_FromDouble(do_pdecimal(self->priv->$fieldname[0], $size, $fracplaces)));
    PyTuple_SET_ITEM(result, 1, PyFloat_FromDouble(do_pdecimal(self->priv->$fieldname[1], $size, $fracplaces)));
    PyTuple_SET_ITEM(result, 2, PyFloat_FromDouble(do_pdecimal(self->priv->$fieldname[2], $size, $fracplaces)));
"""
)
set_decimal_n = Template(
"""
    PyObject *value = NULL;
    int number;
    static char *kwlist[] = {"number","value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|O", kwlist, &number, &value) ) {
        do_log(LOG_ERR, "Invalid arguments \\"number\\" \\"value\\": wrong type. set $fieldname");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             do_pdecimal_set(self->priv->$fieldname[number], $size, $fracplaces,  MyLong_AsLong(value));
        else
        if ( PyFloat_Check(value) )
             do_pdecimal_set(self->priv->$fieldname[number], $size, $fracplaces,  PyFloat_AsDouble(value));
        else {
            do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set $fieldname");
            return NULL;
        }    
    }
    //result=PyFloat_FromDouble(do_pdecimal(self->priv->$fieldname[number], $size, $fracplaces));
"""
)
get_param = Template(
"""
    static char *kwlist[] = {"name", NULL};
    char *name;
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &name) ) {
        do_log(LOG_ERR, "Invalid argument \\"name\\": wrong type. get param");
        return NULL;
    }
    char *res=do_${name}_param(self->alias->alias, self->priv, name);
    result = MyString_FromString(res);
    do_free(res);
"""
)
set_param = Template(
"""
    PyObject *value = NULL;
    static char *kwlist[] = {"name","value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s|O", kwlist, &name, &value) ) {
        do_log(LOG_ERR, "Invalid arguments \\"name\\" \\"value\\": wrong type. set param");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             do_${name}_param_int_set(self->alias->alias, self->priv, name, MyLong_AsLong(value));
        else
        if ( PyFloat_Check(value) )
             do_${name}_param_float_set(self->alias->alias, self->priv, name, PyFloat_AsDouble(value));
        else
        if ( MyString_Check(value) ) {
             char *buf;
             buf = MyString_AsString(value);
             do_${name}_param_set(self->alias->alias, self->priv, name, buf);
             do_free(buf);
        }
        else {
            do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set param");
            return NULL;
        }  
    }  
    /*char *res=do_${name}_param(self->alias->alias, self->priv, name);
    result = MyString_FromString(res);
    do_free(res);*/
"""
)
get_date = Template(
"""
    struct tm tm;
    do_date(self->priv->$fieldname, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);
"""
)
set_date = Template(
"""
    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set $fieldname");
        return NULL;
    }
    struct tm tm;
    BTI_LONG date;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set $fieldname");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->$fieldname = MyLong_AsLong(value);
        } else {
            do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
            self->priv->$fieldname = do_date_set(&date, tm);
        }
    }
    /*do_date(self->priv->$fieldname, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);*/
"""
)
get_time = Template(
"""
    struct tm tm;
    do_time(self->priv->$fieldname, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);
"""
)
set_time = Template(
"""
    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set $fieldname");
        return NULL;
    }
    struct tm tm;
    BTI_LONG time_;
    if ( value ) {
        if ( !PyTime_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set $fieldname");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->$fieldname = MyLong_AsLong(value);
        } else {
            if ( PyTime_Check(value) ) {
                tm.tm_hour = PyDateTime_TIME_GET_HOUR(value);tm.tm_min=PyDateTime_TIME_GET_MINUTE(value);tm.tm_sec=PyDateTime_TIME_GET_SECOND(value);
             }
             else {
                 tm.tm_hour = PyDateTime_DATE_GET_HOUR(value);tm.tm_min=PyDateTime_DATE_GET_MINUTE(value);tm.tm_sec=PyDateTime_DATE_GET_SECOND(value);
             }
             self->priv->$fieldname = do_time_set(&time_, tm);
        }
    }
    /*do_time(self->priv->$fieldname, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);*/
"""
)
get_sqltime = Template(
"""
    struct tm tm;
    do_sqltotime(self->priv->$fieldname, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);
"""
)
set_sqltime = Template(
"""
    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set $fieldname");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyTime_Check(value) && !PyDateTime_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set $fieldname");
            return NULL;
        }
        if ( PyTime_Check(value) ) {
            tm.tm_hour = PyDateTime_TIME_GET_HOUR(value);tm.tm_min=PyDateTime_TIME_GET_MINUTE(value);tm.tm_sec=PyDateTime_TIME_GET_SECOND(value);
        }
        else {
            tm.tm_hour = PyDateTime_DATE_GET_HOUR(value);tm.tm_min=PyDateTime_DATE_GET_MINUTE(value);tm.tm_sec=PyDateTime_DATE_GET_SECOND(value);
        }
        self->priv->$fieldname = do_timetosql(tm);
    }
    /*do_sqltotime(self->priv->$fieldname, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);*/
"""
)
get_sqldate = Template(
"""
    struct tm tm;
    do_sqltodate(self->priv->$fieldname, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);
"""
)
set_sqldate = Template(
"""
    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set $fieldname");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set $fieldname");
            return NULL;
        }
        do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
        self->priv->$fieldname = do_datetosql(tm);
    }
    /*do_sqltodate(self->priv->$fieldname, &tm);
    result = PyDate_FromDate(tm.tm_your+1900r,tm.tm_mon+1,tm.tm_mday);*/
"""
)
def int_proc(name,desc):
	return FieldInfo(
	  get_int.substitute(fieldname=name),
	  set_int.substitute(fieldname=name),
	  desc )
	return retval
def raw_proc(name,desc):
	return FieldInfo(
	  get_raw.substitute(fieldname=name),
	  set_int.substitute(fieldname=name),
	  desc )
	return retval
def date_proc(name,desc):
	return FieldInfo(
	  get_date.substitute(fieldname=name),
	  set_date.substitute(fieldname=name),
	  desc )
	return retval
	return FieldInfo(
	  get_date.substitute(fieldname=name),
	  set_date.substitute(fieldname=name),
	  desc )
	return retval
def time_proc(name,desc):
	return FieldInfo(
	  get_time.substitute(fieldname=name),
	  set_time.substitute(fieldname=name),
	  desc )
	return retval
def currency_proc(name,desc):
	return FieldInfo(
	  get_currency.substitute(fieldname=name),
	  set_currency.substitute(fieldname=name),
	  desc )
	return retval
def char_proc(name,desc):
	return FieldInfo(
	  get_char.substitute(fieldname=name),
	  set_char.substitute(fieldname=name),
	  desc )
	return retval
def string_proc(name,desc):
	return FieldInfo(
	  get_string.substitute(fieldname=name),
	  set_string.substitute(fieldname=name),
	  desc )
	return retval
def int_n3_proc(name,desc):
	return FieldInfo(
	  get_int_n1.substitute(fieldname=name),
	  set_int_n.substitute(fieldname=name),
	  desc )
	return retval
def quant_proc(struct,desc):
	return FieldInfo(
	get_quant.substitute(name=struct),
	set_quant.substitute(name=struct),desc)
def sum_proc(struct,desc):
	return FieldInfo(
	get_sum.substitute(name=struct),
	set_sum.substitute(name=struct),desc)
def decimal_proc(name,size,fracplaces,desc):
	return FieldInfo(
	get_decimal.substitute(fieldname=name,size=size,fracplaces=fracplaces),
	set_decimal.substitute(fieldname=name,size=size,fracplaces=fracplaces),
	desc)
def decimal_n3_proc(name,size,fracplaces,desc):
	return FieldInfo(
	get_decimal_n.substitute(fieldname=name,size=size,fracplaces=fracplaces),
	set_decimal_n.substitute(fieldname=name,size=size,fracplaces=fracplaces),
	desc)
def param_proc(struct,desc):
	return FieldInfo(
	get_param.substitute(name=struct),
	set_param.substitute(name=struct),
	desc)
def sqldate_proc(name,desc):
	return FieldInfo(
	  get_sqldate.substitute(fieldname=name),
	  set_sqldate.substitute(fieldname=name),
	  desc )
	return retval
def sqltime_proc(name,desc):
	return FieldInfo(
	  get_sqltime.substitute(fieldname=name),
	  set_sqltime.substitute(fieldname=name),
	  desc )
	return retval
def real_proc(name,desc):
	return FieldInfo(
	  get_real.substitute(fieldname=name),
	  set_real.substitute(fieldname=name),
	  desc )
	return retval
def price_proc(struct,desc):
	return FieldInfo(
	 get_price.substitute(name=struct),
	 set_price.substitute(name=struct),desc)

class_h_header = Template(
"""
#ifndef ${name}_H_INCLUDED
#define ${name}_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"
"""
)
class_h_struct = Template(
"""
typedef struct {
    PyObject_HEAD
    ${ident}_${suffix}_t *priv;
    Alias *alias;
} $name;
PyTypeObject *get${name}Type();
"""
)
class_h_tail = Template(
"""
#endif // ${name}_H_INCLUDED
"""
)
class_c_header = Template(
"""
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "${ident}.h"
"""
)
class_c_class_init = Template(
"""
static void ${name}_dealloc(PyObject* self)
{
    do_free(((${name}*)self)->priv);
    Py_DECREF(((${name}*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("${name} free\\n");
}
static PyObject *${name}_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("$name new\\n");
    $name *self;
    self = PyObject_NEW(${name}, type);
    //self = ($name *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof($c_name));
    return (PyObject *)self;
}
static int ${name}_init($name *self, PyObject *args, PyObject *kwds)
{
    //LOG("$name init\\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \\"alias\\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    $init_vars
    PyDateTime_IMPORT;
    return 0;
}
"""
)
class_c_clone_full = Template(
"""
static PyObject *${name}_clone($name *self)
{
    ${name} *result;
    result = (${name}*)PyObject_CallFunction((PyObject*)get${name}Type(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}
"""
)
class_c_clone_key = Template(
"""
static PyObject *${name}_clone($name *self)
{
    ${name} *result;
    result = (${name}*)PyObject_CallFunction((PyObject*)get${name}Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}
"""
)
init_params = Template(
"""
    do_${ident}_params_clear(self->alias->alias, self->priv);
"""
)
init_record = Template(
"""
    self->priv->size=sizeof(self->priv->data);
"""
)
init_char = Template(
"""
    self->priv->$name=' ';
"""
)
init_string = Template(
"""
    do_text_set_empty(self->priv->$name);
"""
)
class_c_type= Template(
"""
static PyTypeObject ${name}Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.${name}",             /*tp_name*/
    sizeof(${name}),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)${name}_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)${name}_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "$name objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ${name}_methods,             /* tp_methods */
    0,
    ${name}_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)${name}_init,      /* tp_init */
    0,                         /* tp_alloc */
    ${name}_new,                 /* tp_new */
};
PyTypeObject *get${name}Type()
{
    return &${name}Type_;
}
"""
)
class_c_field_header = Template(
"""
static PyObject *${name}_${fieldname}(${name}* self, void *unused)
{
    PyObject *result;
"""
)
class_c_field_header_raw = Template(
"""
static PyObject *${name}_${fieldname}_raw(${name}* self, void *unused)
{
    PyObject *result;
"""
)
class_c_field_tail = Template(
"""
    return result;
}
"""
)
class_c_get_field_header = Template(
"""
static PyObject *${name}_get_${fieldname}(${name}* self, PyObject *args, PyObject *kwds)
{
    PyObject *result;
"""
)
class_c_get_field_tail = Template(
"""
    return result;
}
"""
)

class_c_set_field_header = Template(
"""
static PyObject *${name}_set_${fieldname}(${name}* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;
"""
)

class_c_set_field_tail = Template(
"""
    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}
"""
)
class_c_params = Template(
"""
static PyObject *${name}_set_param(${name}* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    char *name;
    static char *kwlist[] = {"name", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sO|", kwlist, &name, &value) )
        return NULL;
    if ( MyString_Check(value) ) {
        char *val;
        val = MyString_AsString(value);
        do_${ident}_param_set(self->alias->alias,self->priv, name, val);
        do_free(val);
    }
    else
    if ( MyLong_Check(value) )
        do_${ident}_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(value));
    else
    if ( PyFloat_Check(value) )
        do_${ident}_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(value));
    else {
        do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *${name}_set_params(${name}* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_${ident}_params_clear(self->alias->alias,self->priv);
        int i;
        PyObject *pKeys = PyDict_Keys(value);
        for ( i = 0; i < PyList_Size(pKeys); ++i ) {
             PyObject *pKey = PyList_GetItem(pKeys, i);
             PyObject *pValue;
             PyObject *tmp_name;
             tmp_name = NULL;
             char *name;
             if ( !pKey ) {
                do_log(LOG_ERR, "Invalid key");
                break;
             }
             if ( MyString_Check(pKey) )
                  name = MyString_AsString(pKey);
             else {
                 do_log(LOG_ERR, "Invalid argument \\"key\\": wrong type");
                return NULL;
             }
             pValue = PyDict_GetItem(value, pKey);
             if ( !pValue ) {
                do_log(LOG_ERR, "Invalid value");
                break;
             }
             if ( MyString_Check(pValue) ) {
                 char *val;
                 val = MyString_AsString(pValue);
                 do_${ident}_param_set(self->alias->alias,self->priv, name, val);
                 do_free(val);
             }
             else
             if ( MyLong_Check(pValue) )
                 do_${ident}_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_${ident}_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(pValue));
             else {
                 do_log(LOG_ERR, "Invalid argument \\"value iter\\": wrong type");
                 return NULL;
             }
             do_free(name);
             if ( tmp_name )
                 Py_DECREF(tmp_name);
        }
    }
    else {
        do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *${name}_get_param(${name}* self, PyObject *args, PyObject *kwds)
{
    char *name;
    static char *kwlist[] = {"name", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &name) )
        return NULL;
    char *buf;
    buf = do_${ident}_param(self->alias->alias,self->priv, name);
    PyObject *result;
    result = MyString_FromString(buf);
    do_free(buf);
    return result;
}

static PyObject *${name}_params_clear(${name}* self)
{
    do_${ident}_params_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *${name}_get_params(${name}* self, void *unused)
{   
    PyObject *res;
    char *params = do_alias_text_(self->alias->alias, self->priv->data.params, self->priv->size - (sizeof(self->priv->data) - sizeof(self->priv->data.params)));
   
    res = do_getparams(params);
    do_free(params);
    return res;
}
"""
)
class_c_prices = Template(
"""

static PyObject *${name}_prices_clear(${name}* self)
{
    do_${ident}_price_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *${name}_get_prices(${name}* self, void *unused)
{   
    PyObject *result;
    PyObject *val, *key;
    result = PyDict_New();
    int i;
    int number;
    double price;

    for ( i = 0; i < ${size}; i++ ) {
        number = self->priv->data.nprice[i];
        if ( !number ) break;
        price = do_${ident}_price(self->alias->alias, self->priv, number);
        val = PyFloat_FromDouble(price);
        key = MyLong_FromLong(number);
        PyDict_SetItem(result, key, val);
    }
    return result;
}
static PyObject *${name}_set_prices(${name}* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_${ident}_price_clear(self->alias->alias,self->priv);
        int i;
        PyObject *pKeys = PyDict_Keys(value);
        for ( i = 0; i < PyList_Size(pKeys); ++i ) {
             PyObject *pKey = PyList_GetItem(pKeys, i);
             PyObject *pValue;
             int number;
             if ( !pKey ) {
                do_log(LOG_ERR, "Invalid key");
                break;
             }
             if ( MyLong_Check(pKey) )
                  number = MyLong_AsLong(pKey);
             else {
                 do_log(LOG_ERR, "Invalid argument \\"key\\": wrong type");
                 return NULL;
             }
             pValue = PyDict_GetItem(value, pKey);
             if ( !pValue ) {
                do_log(LOG_ERR, "Invalid value");
                break;
             }
             if ( MyLong_Check(pValue) )
                 do_${ident}_price_set(self->alias->alias,self->priv, number, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_${ident}_price_set(self->alias->alias,self->priv, number, PyFloat_AsDouble(pValue));
             else {
                 do_log(LOG_ERR, "Invalid argument \\"value iter\\": wrong type");
                 return NULL;
             }
        }
    }
    else {
        do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
"""
)
class_c_quants = Template(
"""

static PyObject *${name}_quants_clear(${name}* self)
{
    do_${ident}_quant_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *${name}_get_quants(${name}* self, void *unused)
{   
    PyObject *result;
    PyObject *val, *key;
    result = PyDict_New();
    int i;
    int number;
    double quant;

    for ( i = 0; i < ${size}; i++ ) {
        number = self->priv->data.nquant[i];
        if ( !number ) break;
        quant = do_${ident}_quant(self->alias->alias, self->priv, number);
        val = PyFloat_FromDouble(quant);
        key = MyLong_FromLong(number);
        PyDict_SetItem(result, key, val);
    }
    return result;
}
static PyObject *${name}_set_quants(${name}* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_${ident}_quant_clear(self->alias->alias,self->priv);
        int i;
        PyObject *pKeys = PyDict_Keys(value);
        for ( i = 0; i < PyList_Size(pKeys); ++i ) {
             PyObject *pKey = PyList_GetItem(pKeys, i);
             PyObject *pValue;
             int number;
             if ( !pKey ) {
                do_log(LOG_ERR, "Invalid key");
                break;
             }
             if ( MyLong_Check(pKey) )
                  number = MyLong_AsLong(pKey);
             else {
                 do_log(LOG_ERR, "Invalid argument \\"key\\": wrong type");
                 return NULL;
             }
             pValue = PyDict_GetItem(value, pKey);
             if ( !pValue ) {
                do_log(LOG_ERR, "Invalid value");
                break;
             }
             if ( MyLong_Check(pValue) )
                 do_${ident}_quant_set(self->alias->alias,self->priv, number, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_${ident}_quant_set(self->alias->alias,self->priv, number, PyFloat_AsDouble(pValue));
             else {
                 do_log(LOG_ERR, "Invalid argument \\"value iter\\": wrong type");
                 return NULL;
             }
        }
    }
    else {
        do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
"""
)
class_c_saldo_sums = Template(
"""
static PyObject *Saldo_get_debit_sums(Saldo* self, void *unused)
{   
    PyObject *result;
    PyObject *val, *key;
    result = PyDict_New();
    int i;
    int number;
    double quant;

    for ( i = 0; i < 3; i++ ) {
        number = i;
        quant = do_pdecimal(self->priv->data.debit_sum[number], 8, 2);
        val = PyFloat_FromDouble(quant);
        key = MyLong_FromLong(number);
        PyDict_SetItem(result, key, val);
    }
    return result;
}
static PyObject *Saldo_get_credit_sums(Saldo* self, void *unused)
{   
    PyObject *result;
    PyObject *val, *key;
    result = PyDict_New();
    int i;
    int number;
    double quant;

    for ( i = 0; i < 3; i++ ) {
        number = i;
        quant = do_pdecimal(self->priv->data.credit_sum[number], 8, 2);
        val = PyFloat_FromDouble(quant);
        key = MyLong_FromLong(number);
        PyDict_SetItem(result, key, val);
    }
    return result;
}
"""
) 
class_c_prowod_sums = Template(
"""

static PyObject *Prowod_sums_clear(Prowod* self)
{
    do_prowod_sum_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *Prowod_get_sums(Prowod* self, void *unused)
{   
    PyObject *result;
    PyObject *val, *key;
    result = PyDict_New();
    int i;
    int number;
    double quant;

    for ( i = 0; i < 3; i++ ) {
        number = i;
        quant = do_prowod_sum(self->alias->alias, self->priv, number);
        val = PyFloat_FromDouble(quant);
        key = MyLong_FromLong(number);
        PyDict_SetItem(result, key, val);
    }
    return result;
}
static PyObject *Prowod_set_sums(Prowod* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_prowod_sum_clear(self->alias->alias,self->priv);
        int i;
        PyObject *pKeys = PyDict_Keys(value);
        for ( i = 0; i < PyList_Size(pKeys); ++i ) {
             PyObject *pKey = PyList_GetItem(pKeys, i);
             PyObject *pValue;
             int number;
             if ( !pKey ) {
                do_log(LOG_ERR, "Invalid key");
                break;
             }
             if ( MyLong_Check(pKey) )
                  number = MyLong_AsLong(pKey);
             else {
                 do_log(LOG_ERR, "Invalid argument \\"key\\": wrong type");
                 return NULL;
             }
             if ( number > 2 ) continue;
             pValue = PyDict_GetItem(value, pKey);
             if ( !pValue ) {
                do_log(LOG_ERR, "Invalid value");
                break;
             }
             if ( MyLong_Check(pValue) )
                 do_prowod_sum_set(self->alias->alias,self->priv, number, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_prowod_sum_set(self->alias->alias,self->priv, number, PyFloat_AsDouble(pValue));
             else {
                 do_log(LOG_ERR, "Invalid argument \\"value iter\\": wrong type");
                 return NULL;
             }
        }
    }
    else {
        do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
"""
)
class_c_prowod2_sums = Template(
"""

static PyObject *Prowod2_sums_clear(Prowod2* self)
{
    do_prowod2_sum_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *Prowod2_get_sums(Prowod2* self, void *unused)
{   
    PyObject *result;
    PyObject *val, *key;
    result = PyDict_New();
    int i;
    int number;
    double quant;

    for ( i = 0; i < 5; i++ ) {
        number = self->priv->data.nsum[i];
        if ( !number ) break;
        quant = do_prowod2_sum(self->alias->alias, self->priv, number);
        val = PyFloat_FromDouble(quant);
        key = MyLong_FromLong(number);
        PyDict_SetItem(result, key, val);
    }
    return result;
}
static PyObject *Prowod2_set_sums(Prowod2* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_prowod2_sum_clear(self->alias->alias,self->priv);
        int i;
        PyObject *pKeys = PyDict_Keys(value);
        for ( i = 0; i < PyList_Size(pKeys); ++i ) {
             PyObject *pKey = PyList_GetItem(pKeys, i);
             PyObject *pValue;
             int number;
             if ( !pKey ) {
                do_log(LOG_ERR, "Invalid key");
                break;
             }
             if ( MyLong_Check(pKey) )
                  number = MyLong_AsLong(pKey);
             else {
                 do_log(LOG_ERR, "Invalid argument \\"key\\": wrong type");
                 return NULL;
             }
             if ( number > 2 ) continue;
             pValue = PyDict_GetItem(value, pKey);
             if ( !pValue ) {
                do_log(LOG_ERR, "Invalid value");
                break;
             }
             if ( MyLong_Check(pValue) )
                 do_prowod2_sum_set(self->alias->alias,self->priv, number, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_prowod2_sum_set(self->alias->alias,self->priv, number, PyFloat_AsDouble(pValue));
             else {
                 do_log(LOG_ERR, "Invalid argument \\"value iter\\": wrong type");
                 return NULL;
             }
        }
    }
    else {
        do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
"""
)
class_c_document_sums = Template(
"""
static PyObject *Document_get_sum(Document* self, PyObject *args, PyObject *kwds)
{
    PyObject *result;

    static char *kwlist[] = {"number", NULL};
    int number;
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &number) ) {
        do_log(LOG_ERR, "Invalid argument \\"number\\": wrong type. get data.sumincl");
        return NULL;
    }
    result = PyFloat_FromDouble(do_document_sum(self->alias->alias,self->priv,number));

    return result;
}
static PyObject *Document_set_sum(Document* self, PyObject *args, PyObject *kwds)
{

    double value;
    int number;
    static char *kwlist[] = {"number","value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "id|", kwlist, &number, &value) ) {
        do_log(LOG_ERR, "Invalid arguments \\"number\\" \\"value\\": wrong type. set data.sumincl");
        return NULL;
    }
    do_document_sum_set(self->alias->alias,self->priv,number,value);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Document_sums_clear(Document* self)
{
    do_document_sum_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *Document_get_sums(Document* self, void *unused)
{   
    PyObject *result;
    PyObject *val, *key;
    result = PyDict_New();
    int i;
    char *sum;
    do_list_t *list;
    struct {
       int number;
       double value;
    } rec, *item;
    
    sum = do_document_param(self->alias->alias,self->priv, "SUM_MAP");
    list = do_list_new(TRUE);    

    char *cp = sum, *head;
    for (i = 0; *cp != '\\0'; i++) {
        head = cp;
        while (*cp != '\\0' && *cp != ',') cp++;
        if (head < cp + 1) {
            rec.number = atoi(head);
            do_list_add_alloc(list, &rec, sizeof(rec));
        }
        if (*cp != '\\0') cp++;
    }
    do_free(sum);
    sum = do_document_param(self->alias->alias,self->priv, "SUM");
    cp = sum;
    i = 0;
    while (*cp != '\\0') {
        head = cp;
        while (*cp != '\\0' && *cp != ',') cp++;
        item=list->list[i++];
        if (head != cp) {
           if ( *cp != '\\0' ) {
               *cp = '\\0';
               item->value=  do_atof(head);
               cp++;
           }
           else {
               item->value=  do_atof(head);
               break;
           }
        }
        else {
            item->value=0;
            if ( *cp != '\\0' ) 
               cp++;
        }
    }
    do_free(sum);
    
    for ( i = 0; i < list->count; i++ ) {
        item=list->list[i];
        val = PyFloat_FromDouble(item->value);
        key = MyLong_FromLong(item->number);
        PyDict_SetItem(result, key, val);
    }
    do_list_free(list);
    return result;
}
static PyObject *Document_set_sums(Document* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_document_sum_clear(self->alias->alias,self->priv);
        int i;
        PyObject *pKeys = PyDict_Keys(value);
        for ( i = 0; i < PyList_Size(pKeys); ++i ) {
             PyObject *pKey = PyList_GetItem(pKeys, i);
             PyObject *pValue;
             int number;
             if ( !pKey ) {
                do_log(LOG_ERR, "Invalid key");
                break;
             }
             if ( MyLong_Check(pKey) )
                  number = MyLong_AsLong(pKey);
             else {
                 do_log(LOG_ERR, "Invalid argument \\"key\\": wrong type");
                 return NULL;
             }
             pValue = PyDict_GetItem(value, pKey);
             if ( !pValue ) {
                do_log(LOG_ERR, "Invalid value");
                break;
             }
             if ( MyLong_Check(pValue) )
                 do_document_sum_set(self->alias->alias,self->priv, number, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_document_sum_set(self->alias->alias,self->priv, number, PyFloat_AsDouble(pValue));
             else {
                 do_log(LOG_ERR, "Invalid argument \\"value iter\\": wrong type");
                 return NULL;
             }
        }
    }
    else {
        do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
"""
)
class_c_operation_header = Template(
"""
static PyObject *${name}_${oper}(${name}* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;
"""
)
class_c_operation_header_key = Template(
"""
static PyObject *${name}_${oper}(${name}* self, PyObject *args, PyObject *kwds)
{
    int status;

"""
)
class_c_operation_keys = Template(
"""
    if ( Py_TYPE(key) == get${name}Key${index}Type() )
        status = do_${ident}_get${index}(self->alias->alias, self->priv, ((${name}Key${index}*)key)->priv, $oper_id);
    else
"""
)
class_c_operation_keys_tail = Template(
"""    
    {
        do_log(LOG_ERR, "Invalid argument \\"key\\": wrong type");
        return NULL;
    }
"""
)
class_c_operation = Template(
"""
    status = do_${ident}_key${index}(self->alias->alias, self->priv, $oper_id);
"""
)
class_c_operation_tail = Template(
"""
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}
"""
)
class_c_operation_iter_header_key = Template(
"""
static PyObject *${name}_iter_${oper}(${name}* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    ${ident}_key${index}_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_${ident}_key${index}(self->alias->alias, self->priv, $oper_id);
    while ( status == DO_OK ) {
"""
)
class_c_operation_iter_cmp_key = Template(
"""
       if ( depth >= ${depth} ) {
           if ( do_cmp(key_cmp.${field}, 
                 self->priv->${field}))
               break;
       }
"""
)
class_c_operation_iter_tail_key = Template(
"""
 
        item = ${name}_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_${ident}_key${index}(self->alias->alias, self->priv, $oper_id_next);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}
"""
)
class_c_operation_iter_header = Template(
"""
static PyObject *${name}_iter_${oper}(${name}* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", "depth", NULL};
    int status;
    int depth;
    void *key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &key, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
"""
)
class_c_operation_iter_keys_1 = Template(
"""
    if ( Py_TYPE(key) == get${name}Key${index}Type() ) {
        key_cmp = (${ident}_key${index}_t*)do_malloc(sizeof(${ident}_key${index}_t));
        memcpy(key_cmp, ((${name}Key${index}*)key)->priv, sizeof(${ident}_key${index}_t));
        status = do_${ident}_get${index}(self->alias->alias, self->priv, ((${name}Key${index}*)key)->priv, $oper_id);
    }
    else
"""
)
class_c_operation_iter_keys_tail_1 = Template(
"""    
    {
        do_log(LOG_ERR, "Invalid argument \\"key\\": wrong type");
        return NULL;
    }
"""
)
class_c_operation_iter_body_get = Template(
"""
    while ( status == DO_OK ) {
""")
class_c_operation_iter_cmp_header = Template(
"""
        if ( Py_TYPE(key) == get${name}Key${index}Type() ) {
"""
)
class_c_operation_iter_cmp = Template(
"""       
            if ( depth >= ${depth} ) {
                if ( do_cmp(((${ident}_key${index}_t*)key_cmp)->${field}, 
                    ((${name}Key${index}*)key)->priv->${field}))
                   break;
            }
"""
)
class_c_operation_iter_cmp_next = Template(
"""
        }
        else
"""
)
class_c_operation_iter_data = Template(
"""
     item = ${name}_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     
"""
)
class_c_operation_iter_key_next = Template(
"""
        if ( Py_TYPE(key) == get${name}Key${index}Type() ) {
            status = do_${ident}_get${index}(self->alias->alias, self->priv, ((${name}Key${index}*)key)->priv, $oper_id_next);
        }
        else
"""
)
class_c_operation_iter_keys_next_tail_1 = Template(
"""    
        {
            do_log(LOG_ERR, "Invalid argument \\"key\\": wrong type");
            return NULL;
        }
"""
)
class_c_operation_iter_tail = Template(
"""
    }
    if ( status == DO_ERROR ) {
        do_free(key_cmp);
        Py_DECREF(retval);
        return NULL;
    }
    do_free(key_cmp);
    //Py_INCREF(retval);
    return retval;
}
"""
)

class_c_operation = Template(
"""
    status = do_${ident}_key${index}(self->alias->alias, self->priv, $oper_id);
"""
)
class_c_operation_tail = Template(
"""
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}
"""
)
class_c_update = Template(
"""
static PyObject *${name}_${oper}(${name}* self)
{
    int status;
    status = do_${ident}_${oper}(self->alias->alias${suffix});
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}
"""
)
class_c_cmp_header = Template(
"""
static PyObject *${name}_cmp(${name}* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
"""
)
class_c_cmp_type = Template (
"""        
    if ( Py_TYPE(obj) == get${name}Type() ) {
    
"""
)
class_c_cmp_field = Template(
"""
       if ( !res && depth >= ${index} ) {
           res = do_cmp(self->priv->${field}, ((${name}*)obj)->priv->${field});
       }
"""
)
class_c_cmp_int_field = Template(
"""
       if ( !res && depth >= ${index} ) {
           res = self->priv->${field} - ((${name}*)obj)->priv->${field};
       }
"""
)
class_c_cmp_type_tail = Template(
"""
       return MyLong_FromLong(res);
    }
"""
)
class_c_cmp_tail = Template(
"""
    do_log(LOG_ERR, "Invalid argument \\"object\\": wrong type");
    return NULL;
}
"""
)

class_c_set_alias = Template(
"""
static PyObject *${name}_set_alias(${name}* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((${name}*)self)->alias);
    Py_INCREF(obj);
    ((${name}*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}
"""
)

class_c_set_header = Template(
"""
static PyObject *${name}_set(${name}* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == get${name}Type() ) {
        memcpy(self->priv, (($name*)obj)->priv, sizeof(*self->priv));
    }
"""
)
class_c_set_check_type = Template(
"""
    else 
    if ( Py_TYPE(obj) == get${name}Type() ) {
"""
)
class_c_set_check_type_copy = Template(
"""
        do_cpy(self->priv->${ident_master}, (($name*)obj)->priv->${ident_slave});
"""
)
class_c_set_check_type_tail = Template(
"""
    }
"""
)
class_c_set_check_list = Template(
"""
    else 
    if ( PyList_Check(obj) ||  PyTuple_Check(obj) ) {
        int i;
        int size;
        size = PyList_Check(obj) ? PyList_Size(obj) : PyTuple_Size(obj);
        for ( i = 0; i < size; i++ ) {
            PyObject *value;
            value =  PyList_Check(obj) ? PyList_GetItem(obj, i) : PyTuple_GetItem(obj, i);
            switch (i) {
"""
)
class_c_set_check_list_copy = Template(
"""
                case ${index}:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_${field}", "O", value) )
                        return NULL;
                    break;
"""
)
class_c_set_check_list_tail = Template(
"""
                default:
                    do_log(LOG_ERR, "%s size larger than the number of fields", PyList_Check(obj) ? "List":"Tuple");
                    return NULL;
            }
        }
    }
"""
)
class_c_set_check_dict = Template(
"""
    else 
    if ( PyDict_Check(obj) ) {
        PyObject *fields;
        int i;
        
        fields = PyDict_Keys(obj);
        for ( i = 0; i < PyList_Size(fields); i++ ) {
            PyObject *name;
            PyObject *value;
            char *field_name;
            
            name = PyList_GetItem(fields, i);
            value = PyDict_GetItem(obj, name); 
            field_name = MyString_AsString(name);
            if ( field_name[0] == '\\0' ) {
                do_log(LOG_ERR, "Dictionary key is not a string");
                return NULL;
            } 

"""
)
class_c_set_check_dict_copy = Template(
"""
            else
            if ( !strcmp("${field}", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_${field}", "O", value) )
                    return NULL;
            }
"""
)
class_c_set_check_dict_tail = Template(
"""
            else {
                do_log(LOG_ERR, "Class \\"${name}\\"does not contain \\"%s\\"", field_name);
                return NULL;
            }
            do_free(field_name);
        }
    }
"""
)
class_c_set_tail = Template(
"""    
    else
    {
        do_log(LOG_ERR, "Invalid argument \\"object\\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
"""
)
class_c_str_header = Template(
"""
static PyObject *${name}_fields(${name}* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ${name}");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;

"""
)
class_c_str_field = Template(
"""
    value =  ${value};
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("${name}", value);
    buf1 = do_strdup_printf("%s\\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;
"""
)
class_c_str_field_price = Template(
"""
    value = ${name}_get_prices(self, NULL);
    buf=get_prices_desc(value);
    buf1 = do_strdup_printf("%s\\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;
"""
)
class_c_str_field_quant = Template(
"""
    value = ${name}_get_quants(self, NULL);
    buf = get_quants_desc(value, "quants");
    buf1 = do_strdup_printf("%s\\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;
"""
)
class_c_str_field_sum = Template(
"""
    value = ${name}_get_sums(self, NULL);
    buf = get_quants_desc(value, "sums");
    buf1 = do_strdup_printf("%s\\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;
"""
)
class_c_str_field_params = Template(
"""
    value = ${name}_get_params(self, NULL);
    buf = get_params_desc(value);
    buf1 = do_strdup_printf("%s\\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;
"""
)
class_c_str_field_coef = Template(
"""
    buf = do_strdup_printf("Coef:%f", do_product_coef(self->alias->alias, self->priv));
    buf1 = do_strdup_printf("%s\\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;
"""
)
class_c_str_field_suminclude = Template(
"""
    
    buf = do_strdup_printf("SumInclude:[%d,%d,%d]", self->priv->data.sumincl[0],self->priv->data.sumincl[1],self->priv->data.sumincl[2]);
    buf1 = do_strdup_printf("%s\\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;
"""
)
class_c_str_field_debit_sum = Template(
"""
    
    buf = do_strdup_printf("${name}:[%f,%f,%f]", 
    do_pdecimal(self->priv->data.$ident[0], 8, 2),
    do_pdecimal(self->priv->data.$ident[1], 8, 2),
    do_pdecimal(self->priv->data.$ident[2], 8, 2));
    buf1 = do_strdup_printf("%s\\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;
"""
)
class_c_str_field_total = Template(
"""
    
    buf = do_strdup_printf("${name}:%f", 
    do_pdecimal(self->priv->data.$ident, 7, 3));
    buf1 = do_strdup_printf("%s\\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;
"""
)
class_c_str_field_discount = Template(
"""
    
    buf = do_strdup_printf("${name}:%f", 
    do_pdecimal(self->priv->data.$ident, 3, 0));
    buf1 = do_strdup_printf("%s\\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;
"""
)

class_c_str_tail = Template(
"""    
    buf = do_strdup_printf("%s\\n",str);
    retval = MyString_FromString(buf);
    do_free(str);
    do_free(buf);
    return retval;
}
"""
)

class_c_getset_header = Template(
"""
static PyGetSetDef ${name}_getset[] = {
"""
) 

class_c_getset_field = Template(
"""
    {"${field_lower}",(getter)${name}_${field}},
"""
) 
class_c_getset_field_raw = Template(
"""
    {"${field_lower}_raw",(getter)${name}_${field}_raw},
"""
) 
class_c_getset_tail = Template(
"""
    {NULL}
};
"""
)

class_c_methods_header = Template(
"""
static PyMethodDef ${name}_methods[] = {
    {"set", (PyCFunction)${name}_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)${name}_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)${name}_clone, METH_NOARGS, "clone"},
"""
) 
class_c_methods_cmp = Template(
"""
    {"cmp", (PyCFunction)${name}_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},
"""
) 
class_c_methods_field = Template(
"""
    {"set_${field_lower}", (PyCFunction)${name}_set_${field}, METH_VARARGS|METH_KEYWORDS, "${name}_set_${field}"},
"""
)
class_c_methods1_field = Template(
"""
    {"${field}", (PyCFunction)${name}_get_${field}, METH_VARARGS|METH_KEYWORDS,"${name}_get_${field}"},
"""
)
class_c_methods_oper = Template(
"""
    {"get_${oper}", (PyCFunction)${name}_${oper}, METH_VARARGS|METH_KEYWORDS, "${name}_${oper}"},
"""
)
class_c_methods_oper_iter = Template(
"""
    {"gets_${oper}", (PyCFunction)${name}_iter_${oper}, METH_VARARGS|METH_KEYWORDS, "${name}_iter_${oper}"},
"""
)

class_c_methods_oper_update= Template(
"""
    {"${oper}", (PyCFunction)${name}_${oper}, METH_VARARGS|METH_KEYWORDS, "${name}_${oper}"},
"""
)
class_c_methods_param = Template(
"""
    {"set_param", (PyCFunction)${name}_set_param, METH_VARARGS|METH_KEYWORDS, "do_${name}_param_set"},
    {"param", (PyCFunction)${name}_get_param, METH_VARARGS|METH_KEYWORDS, "do_${name}_param"},
    {"clear_params", (PyCFunction)${name}_params_clear, METH_NOARGS, "do_${name}_param_clear"},
    {"set_params", (PyCFunction)${name}_set_params, METH_VARARGS|METH_KEYWORDS, "do_${name}_set_params"},
"""
)
class_c_methods_prices = Template(
"""
    {"clear_prices", (PyCFunction)${name}_prices_clear, METH_NOARGS, "do_${name}_prices_clear"},
    {"price", (PyCFunction)${name}_get_price, METH_VARARGS|METH_KEYWORDS, "do_${name}_price"},
    {"set_prices", (PyCFunction)${name}_set_prices, METH_VARARGS|METH_KEYWORDS, "do_${name}_set_prices"},
"""
)
class_c_methods_quants = Template(
"""
    {"clear_quants", (PyCFunction)${name}_quants_clear, METH_NOARGS, "do_${name}_quants_clear"},
    {"quant", (PyCFunction)${name}_get_quant, METH_VARARGS|METH_KEYWORDS, "do_${name}_quant"},
    {"set_quants", (PyCFunction)${name}_set_quants, METH_VARARGS|METH_KEYWORDS, "do_${name}_set_quants"},
"""
)
class_c_methods_sums = Template(
"""
    {"clear_sums", (PyCFunction)${name}_sums_clear, METH_NOARGS, "do_${name}_sums_clear"},
    {"sum", (PyCFunction)${name}_get_sum, METH_VARARGS|METH_KEYWORDS, "do_${name}_sum"},
    {"set_sums", (PyCFunction)${name}_set_sums, METH_VARARGS|METH_KEYWORDS, "do_${name}_set_sums"},
"""
)
class_c_methods_tail = Template(
"""
    {NULL}
};
"""
)
alias_header = Template (
"""
#include <Python.h>
#include "alias.h"
#include "util.h"
#include "db.h"

static void Alias_dealloc(PyObject* self)
{
    LOG("Alias free %d\\n",self);
    if ( ((Alias*)self)->alias )
        do_alias_free(((Alias*)self)->alias);
    ((Alias*)self)->alias = NULL;
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Alias_clear(PyObject* self)
{
    LOG("Alias clear\\n");
    if ( ((Alias*)self)->alias )
        do_alias_free(((Alias*)self)->alias);
    ((Alias*)self)->alias = NULL;
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *Alias_dublicate_allow(PyObject* self)
{
    do_alias_set_dublicate_allow( ((Alias*)self)->alias, TRUE);
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *Alias_dublicate_not_allow(PyObject* self)
{
    do_alias_set_dublicate_allow( ((Alias*)self)->alias, FALSE);
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *Alias_start_tran(PyObject* self)
{
    LOG("Alias clear\\n");
    int res = do_alias_tran_begin( ((Alias*)self)->alias);
    return PyBool_FromLong(res);
}
static PyObject *Alias_end_tran(PyObject* self)
{
    LOG("Alias clear\\n");
    int res = do_alias_tran_end( ((Alias*)self)->alias);
    return PyBool_FromLong(res);
}
static PyObject *Alias_abort_tran(PyObject* self)
{
    LOG("Alias clear\\n");
    int res = do_alias_tran_abort( ((Alias*)self)->alias);
    return PyBool_FromLong(res);
}
static PyObject *Alias_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Alias *self;

    self = PyObject_NEW(Alias, type);
    LOG("Alias new %d\\n",self);
    if ( self == NULL )
        return NULL;
    self->alias = NULL;
    return (PyObject *)self;
}

static int Alias_init(Alias *self, PyObject *args, PyObject *kwds)
{
    //LOG("Alias init\\n");

    static char *kwlist[] = {"name", NULL};
    char *name = NULL;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist,
                                      &name) )
        return -1;
    product_struct_t product;
    if ( sizeof(product) != 2470 ) {
	do_log(LOG_ERR, "The library is compiled without __attribute__((packed)) sizeof product struct %d", sizeof(product));
	return -1;
    }
    self->alias = domino_alias_new(name);
    if ( !self->alias )
        return -1;
    if ( !do_alias_open(self->alias, TRUE) ) 
        return -1;
    return 0;
}

static PyObject *Alias_connect(Alias *self)
{
    LOG("Alias connect\\n");

    if ( !do_alias_open(self->alias, TRUE) )
        return NULL;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Alias_get_name(Alias* self)
{
    PyObject *result;

    result = MyString_FromString(do_alias_get_name(self->alias));

    return result;
}
static PyObject *Alias_get_host(Alias* self)
{
    PyObject *result;

    result = MyString_FromString(do_alias_get_host(self->alias));

    return result;
}
static PyObject *Alias_utf8(Alias* self)
{
    PyObject  *result;

    result = Py_BuildValue("i", do_alias_utf8(self->alias));

    return result;
}
static PyObject *Alias_connected(Alias* self)
{
    PyObject  *result;

    result = Py_BuildValue("i", do_alias_conected(self->alias));

    return result;
}
"""
)
alias_object = Template(
"""
static PyObject *Alias_${name}(Alias* self)
{
    return PyObject_CallFunction((PyObject*)get${name}Type(),
                                    "O", self);
}   
"""
) 
alias_method_header = Template(
"""
static PyMethodDef Alias_methods[] = {
    {"get_name", (PyCFunction)Alias_get_name, METH_NOARGS,
     "do_alias_get_name"
    },
    {"get_host", (PyCFunction)Alias_get_host, METH_NOARGS,
     "do_alias_get_host"
    },
    {"utf8", (PyCFunction)Alias_utf8, METH_NOARGS,
     "do_alias_utf8"
    },
    
    {"dublicate_allow", (PyCFunction)Alias_dublicate_allow, METH_NOARGS,// METH_VARARGS|METH_KEYWORDS,
     "dublicate allow"
    },
    {"dublicate_not_allow", (PyCFunction)Alias_dublicate_not_allow, METH_NOARGS,// METH_VARARGS|METH_KEYWORDS,
     "dublicate not allow"
    },
    {"transaction_start", (PyCFunction)Alias_start_tran, METH_NOARGS,// METH_VARARGS|METH_KEYWORDS,
     "transactiob start"
    },
    {"transaction_stop", (PyCFunction)Alias_end_tran, METH_NOARGS,// METH_VARARGS|METH_KEYWORDS,
     "transactiob stop"
    },
    {"transaction_abort", (PyCFunction)Alias_abort_tran, METH_NOARGS,// METH_VARARGS|METH_KEYWORDS,
     "transactiob abort"
    },
    {"connect", (PyCFunction)Alias_connect, METH_NOARGS,// METH_VARARGS|METH_KEYWORDS,
     "do_alias_open"
    },
    {"clear", (PyCFunction)Alias_clear, METH_NOARGS,// METH_VARARGS|METH_KEYWORDS,
     "do_alias_clear"
    },
    {"connected", (PyCFunction)Alias_connected, METH_NOARGS,// METH_VARARGS|METH_KEYWORDS,
     "do_alias_connected"
    },
"""
)
alias_method_tail = Template(
"""
    {NULL}  /* Sentinel */
};

"""
)
alias_type = Template(
"""
static PyTypeObject AliasType = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Alias",             /*tp_name*/
    sizeof(Alias),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Alias_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Alias objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    Alias_methods,             /* tp_methods */
    0,//Alias_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Alias_init,      /* tp_init */
    0,                         /* tp_alloc */
    Alias_new,                 /* tp_new */
};
PyTypeObject *getAliasType()
{
    return &AliasType;
}

"""
)
alias_h = Template(
"""
#ifndef ALIAS_H_INCLUDED
#define ALIAS_H_INCLUDED

#include <Python.h>
#include <domino.h>

typedef struct {
    PyObject_HEAD
    do_alias_t *alias;
} Alias;
PyTypeObject *getAliasType();


#endif // ALIAS_H_INCLUDED
"""
)
alias_method= Template(
"""
    {"${name}", (PyCFunction)Alias_${name},  METH_NOARGS, "Alias_${name}"},
"""
)
util_h = Template(
"""
#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#ifdef _DEBUG
#define LOG(...) fprintf (stderr, __VA_ARGS__)
#else
#define LOG(...)
#endif // _DEBUG

#if PY_MAJOR_VERSION >= 3
#define MyString_Check(x) PyUnicode_Check(x)
#define MyLong_Check(x) PyLong_Check(x)
#define MyLong_AsLong(x) PyLong_AsLong(x)
#define MyLong_FromLong(x) PyLong_FromLong(x)
#else
#define MyString_Check(x) (PyUnicode_Check(x) || PyString_Check(x))
#define MyLong_Check(x) (PyLong_Check(x) || PyInt_Check(x))
#define MyLong_AsLong(x) (PyLong_Check(x) ? PyLong_AsLong(x) : PyInt_AsLong(x))
#define MyLong_FromLong(x) PyLong_FromLong(x)
#endif


#ifdef __cplusplus
extern "C"
{
#endif

PyObject* do_getparams(const char* src);
char *get_desc(const char *name, PyObject *value);
char *get_prices_desc(PyObject *value);
char *get_quants_desc(PyObject *value, const char* name);
char *get_params_desc(PyObject *value);
char *MyString_AsString(PyObject *value);
PyObject *MyString_FromString(const char *value);
//PyObject *do_log(LOG_ERR, const char *msgfmt, ...);
PyObject *getDominoErrorObject();
#ifdef USE_LOUDMOTH 
PyObject *send_xmpp_message(PyObject *args, PyObject *kwds);
#endif

#ifdef __cplusplus
}
#endif

#endif // UTIL_H_INCLUDED

"""
)
util_c = Template(
"""


#include <Python.h>
#include "util.h"
#include <dolib.h>
#include <datetime.h>

static PyObject *Error = NULL;
PyObject *getDominoErrorObject()
{
    if ( !Error ) {
        Error = PyErr_NewException("pydomino.error", NULL, NULL);
        Py_INCREF(Error);
    }

    return Error;
}
PyObject* do_getparams(const char* src)
{
    PyObject *result;
    PyObject *val;
    result = PyDict_New();

    char *cp = (char*)src, *head;
    char name[1024];
    char value[4098];
    while (*cp != '\\0') {
        head = cp;
        while (*cp != '\\0' && *cp != '\\n' && *cp !='\\r' && *cp != '=') cp++;
        if (*cp != '=' ) {
            while (*cp == '\\n' || *cp =='\\r') cp++;
            continue;
        }
        strncpy(name, head, cp - head);
        name[cp - head] = '\\0';
        head = ++cp;
        while (*cp != '\\0' && *cp != '\\n' && *cp !='\\r') cp++;
        strncpy(value, head, cp - head);
        value[cp - head] = '\\0';
        if ( strlen(name) > 0 ) {
            val = MyString_FromString(value);
            PyDict_SetItemString(result, name, val);
        }
        while (*cp == '\\n' || *cp =='\\r') cp++;
    }
    return result;
}
char *get_desc(const char *name, PyObject *value)
{
    char *buf;
    PyDateTime_IMPORT;    
    if ( MyString_Check(value) ) {
        char *val;
        val = MyString_AsString(value);
        buf = do_strdup_printf("%s:\\"%s\\"", name, val);
        do_free(val);
    }
    else
    if ( MyLong_Check(value) )
        buf = do_strdup_printf("%s:%ld", name, MyLong_AsLong(value));
    else
    if ( PyFloat_Check(value) )
        buf = do_strdup_printf("%s:%f", name, PyFloat_AsDouble(value));
    else
    if ( PyDate_Check(value) )
        buf = do_strdup_printf("%s:%2.2d/%2.2d/%d", name,  PyDateTime_GET_DAY(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_YEAR(value));
    else
    if ( PyTime_Check(value) ) {
        buf = do_strdup_printf("%s:%2.2d:%2.2d:%2.2d", name, PyDateTime_TIME_GET_HOUR(value), PyDateTime_TIME_GET_MINUTE(value), PyDateTime_TIME_GET_SECOND(value));
    }
    else
    if ( PyDateTime_Check(value) )
        buf = do_strdup_printf("%s:%2.2d/%2.2d/%d %2.2d %2.2d %2.2d", name,  PyDateTime_GET_DAY(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_YEAR(value), PyDateTime_DATE_GET_HOUR(value), PyDateTime_DATE_GET_MINUTE(value), PyDateTime_DATE_GET_SECOND(value));
    else {
        do_log(LOG_ERR, "Invalid type field \\"%s\\"", name);
        buf = do_strdup_printf("%s:unknow", name);
    }
    return buf;
}

char *get_prices_desc(PyObject *value) 
{
    char *buf;
    int i;
    PyObject *pKeys = PyDict_Keys(value);
    buf = NULL;
    for ( i = 0; i < PyList_Size(pKeys); ++i ) {
        char *v,*v1;
        PyObject *pKey = PyList_GetItem(pKeys, i);
        PyObject *pValue;
        pValue = PyDict_GetItem(value, pKey);
        v = do_strdup_printf("%ld:%f",  MyLong_AsLong(pKey), PyFloat_AsDouble(pValue));
        if ( buf ) {
            v1 = do_strdup_printf("%s,%s", buf, v);
            do_free(buf);do_free(v);
            buf = v1;
        }
        else 
            buf = v;
    }
    if ( buf ) {
       char *v;
       v = do_strdup_printf("Prices:[%s]", buf);
       do_free(buf);
       buf = v;
    }
    else
       buf=do_strdup("Prices:[]");
    return buf;
}
char *get_quants_desc(PyObject *value, const char *name) 
{
    char *buf;
    PyObject *pKeys = PyDict_Keys(value);
    int i;
    buf = NULL;
    for ( i = 0; i < PyList_Size(pKeys); ++i ) {
        char *v,*v1;
        PyObject *pKey = PyList_GetItem(pKeys, i);
        PyObject *pValue;
        pValue = PyDict_GetItem(value, pKey);
        v = do_strdup_printf("%ld:%f",  MyLong_AsLong(pKey), PyFloat_AsDouble(pValue));
        if ( buf ) {
            v1 = do_strdup_printf("%s,%s", buf, v);
            do_free(buf);do_free(v);
            buf = v1;
        }
        else 
            buf = v;
    }
    if ( buf ) {
       char *v;
       v = do_strdup_printf("%s:[%s]", name, buf);
       do_free(buf);
       buf = v;
    }
    else
       buf=do_strdup_printf("%s:[]",name);
    return buf;
}
char *get_params_desc(PyObject *value) 
{
    char *buf;
    PyObject *pKeys = PyDict_Keys(value);
    int i;
    buf = NULL;
    for ( i = 0; i < PyList_Size(pKeys); ++i ) {
        char *v,*v1;
        PyObject *pKey = PyList_GetItem(pKeys, i);
        PyObject *pValue;
        pValue = PyDict_GetItem(value, pKey);
        char *name;char *val;
        name = MyString_AsString(pKey);
        val = MyString_AsString(pValue);
        v = do_strdup_printf("%s:\\"%s\\"", name, val);
        if ( buf ) {
            v1 = do_strdup_printf("%s,%s", buf, v);
            do_free(buf);do_free(v);
            buf = v1;
        }
        else 
            buf = v;
        do_free(name);do_free(val);
    }
    if ( buf ) {
       char *v;
       v = do_strdup_printf("Params:[%s]", buf);
       do_free(buf);
       buf = v;
    }
    else
       buf=do_strdup("Params:[]");
    return buf;
}
char *MyString_AsString(PyObject *value)
{
#if PY_MAJOR_VERSION >= 3
    if ( PyUnicode_Check(value) ) {
        PyObject *tmp;
        char *retval;
        tmp = PyUnicode_AsUTF8String(value);
        retval = strdup(PyBytes_AsString(tmp));
        Py_DECREF(tmp);
        return retval;
    }
    return strdup("");
#else 
    if ( PyString_Check(value) ) 
        return strdup(PyString_AsString(value));
    
    if ( PyUnicode_Check(value) ) {
        PyObject *tmp;
        char *retval;
        tmp = PyUnicode_AsUTF8String(value);
        retval = strdup(PyString_AsString(tmp));
        Py_DECREF(tmp);
        return retval;
    }
    return strdup("");
#endif    
}
PyObject *MyString_FromString(const char *value)
{
#if PY_MAJOR_VERSION >= 3
    return PyUnicode_DecodeUTF8(value,strlen(value),NULL);
#else
    return PyString_FromString(value);
#endif
}
/*
#define MAX_LOG_LINE 1025
PyObject *do_log(LOG_ERR, const char *msgfmt, ...)
{
    va_list ap;
    va_start(ap, msgfmt);
    char message[MAX_LOG_LINE+1];
    int len = 0;
    len = vsnprintf(message, MAX_LOG_LINE, msgfmt, ap);
    if (len > MAX_LOG_LINE)
        message[MAX_LOG_LINE] = '\\0';
    else
        message[len] = '\\0';
    va_end(ap);
//    fprintf(stderr, "%s\\n", message);
    PyErr_SetString(Error, message);
    return NULL;
}
*/
"""
)
pydomino_c_header = Template(
"""
#include <Python.h>
#include <domino.h>
#include <locale.h>
#include <datetime.h>
#include "util.h"
#include "db.h"
#include "../src/head.h"
#include "../version.h"


static PyObject *Get_local_store(Saldo* self, void *unused)
{
    PyObject *result;

    result = MyString_FromString(domino_unit());
    return result;
}
static PyObject *Get_domino_date(PyObject *obj_, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"value", NULL};
    PyObject *value=0;
    PyDateTime_IMPORT;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, &value) ) {
        do_log(LOG_ERR, "no param value");
        return NULL;
    }
    PyObject *result;
    struct tm tm;
    BTI_LONG date;
    if ( PyDate_Check(value) || PyDateTime_Check(value) ) {
        do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
        do_date_set(&date, tm);
        result = MyLong_FromLong(date);
    }
    else {
        if ( MyLong_Check(value) ) {
            struct tm tm;
            date = MyLong_AsLong(value);
            do_date(date, &tm);
            result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);
        }
        else {
            do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type.");
            return NULL;
        }
    }
    return result;
}
static PyObject *Get_domino_time(PyObject *obj_, PyObject *args, PyObject *kwds)
{

    static char *kwlist[] = {"value", NULL};
    PyObject *value=0;
    PyDateTime_IMPORT;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, &value) ) {
        do_log(LOG_ERR, "no param value");
        return NULL;
    }
    PyObject *result;
    struct tm tm;
    BTI_LONG date;
    if ( PyTime_Check(value) || PyDateTime_Check(value) ) {
        if ( PyTime_Check(value) ) {
             tm.tm_hour = PyDateTime_TIME_GET_HOUR(value);tm.tm_min=PyDateTime_TIME_GET_MINUTE(value);tm.tm_sec=PyDateTime_TIME_GET_SECOND(value);
        }
        else  {
             tm.tm_hour = PyDateTime_DATE_GET_HOUR(value);tm.tm_min=PyDateTime_DATE_GET_MINUTE(value);tm.tm_sec=PyDateTime_DATE_GET_SECOND(value);
        }
        do_time_set(&date, tm);
        result = MyLong_FromLong(date);
    }
    else {
        if ( MyLong_Check(value) ) {
            struct tm tm;
            date = MyLong_AsLong(value);
            do_time(date, &tm);
            result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);
        }
        else {
            do_log(LOG_ERR, "Invalid argument \\"value\\": wrong type.");
            return NULL;
        }
    }
    return result;
}

static PyMethodDef module_methods[] = {
	{"local_store", (PyCFunction)Get_local_store, METH_NOARGS, "local_store"},
	{"domino_date", (PyCFunction)Get_domino_date, METH_VARARGS|METH_KEYWORDS, "domino_date"},
	{"domino_time", (PyCFunction)Get_domino_time, METH_VARARGS|METH_KEYWORDS, "domino_time"},
    {NULL}  /* Sentinel */
};

//static PyObject *DominoError;

/*static PyObject *do_log(LOG_ERR, const char *mgs, int level, void *user_data)
{
    fprintf(stderr, "%s\\n", mgs);
    if ( level == LOG_ERR || level == LOG_CRIT ) {
        PyErr_SetString(DominoError, mgs);
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}*/
static int log_func(const char *mgs, int level, void *user_data)
{
    if ( level == LOG_ERR || level == LOG_CRIT ) {
        PyErr_SetString(getDominoErrorObject(), mgs);
        return FALSE;
    }
    else {
        fprintf(stderr, "%s\\n", mgs);
    }
    return TRUE;
}

#if PY_MAJOR_VERSION >= 3
static PyModuleDef PyDomino_module = {
    PyModuleDef_HEAD_INIT,      /* m_base */
    "pydomino",                 /* m_name */
    NULL,                       /* m_doc */
    -1,                         /* m_size */
    module_methods,             /* m_methods */
};
#endif
PyMODINIT_FUNC
#if PY_MAJOR_VERSION >= 3
PyInit_pydomino(void)
#else
initpydomino(void)
#endif
{
    PyObject* m;

    if ( PyType_Ready(getAliasType()) < 0 )
#if PY_MAJOR_VERSION >= 3
        return NULL;
#else
        return;
#endif         
    setlocale(LC_ALL, "");
    
#if PY_MAJOR_VERSION >= 3
    m = PyModule_Create(&PyDomino_module);
#else    
    m = Py_InitModule3("pydomino", module_methods,
                       "Example module that creates an extension type.");
#endif                       

    if (m == NULL)
#if PY_MAJOR_VERSION >= 3
        return NULL;
#else
        return;
#endif         

    //PyObject *DominoError = getDominoErrorObject();
    //PyErr_NewException("pydomino.error", NULL, NULL);
    //Py_INCREF(DominoError);
    PyModule_AddObject(m, "error", getDominoErrorObject());

    do_log_set_log_func(log_func, NULL);
    //do_log_set_stdout();
    //LOG("domino init\\n");

    if ( !domino_init(NULL, TRUE, DO_PARAM_END) )
         goto init_error;
    Py_INCREF(getAliasType());
    PyModule_AddObject(m, "Alias", (PyObject *)getAliasType());
    PyModule_AddStringConstant(m, "__version__", VERSION " " ADATE);
    

"""
)
pydomino_c_class_init = Template(
"""
    if ( PyType_Ready(get${name}Type()) < 0 )
         goto init_error;

    Py_INCREF(get${name}Type());

    PyModule_AddObject(m, "${name}", (PyObject *)get${name}Type());
"""
)
pydomino_c_tail = Template(
"""
#if PY_MAJOR_VERSION >= 3
    return m;
  init_error:
    Py_CLEAR(m);
    return NULL;
#else
  init_error:
    return;
#endif

}
"""
)
make_header = Template(
"""
include ../../common.mak

LDFLAGS =  -ldomino -ldolib `pkg-config python-${python_version} --libs`
CFLAGS = -D_REENTRANT `pkg-config python-${python_version} --cflags` -D_DEBUG

"""
)

make_tail = Template(
"""
VER = ${version}
NAME = pydomino.so
DLL = pydomino.dll
APP = pydomino
all: pydomino
pydomino:
    $$(CC) $$(AM_FLAGS) -shared -fpic $$(LDFLAGS) $$(CFLAGS) $$(SRCS) -o $$(NAME) -Wl,-soname,$$(NAME)
"""
)
