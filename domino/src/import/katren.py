#!/bin/env python3.4
#-*- coding: UTF-8 -*-

from gi.repository import Gtk, Gio, Gdk,GObject, Pango

from pydomino import Alias
from log import Log
import platform
import os
import sys
from locale import currency
import tempfile
import re
from datetime import datetime,timedelta
from invoice import Invoice,InvoiceOrder
import gtktreeviewcelldatafunc as datafunc
from dominoentry import DateEntry,date_format,datetime_format,hardware_keycode_is_keyval
_pix = 300
first_start = True
max_up_days=14


suppliers = {
"protek":[2,"CP1251","%d.%m.%Y"],
#"katren":[6,"CP1251","%d.%m.%Y"]
}

gtk_minor = Gtk.get_minor_version()

if platform.system() == "Windows":
	share = "z:\\\\"
else:
	share = "/mnt/domino"

invoice_path = os.path.join(share, "domino","mail","invoice")
mask = re.compile("(?P<filename>(?P<number>[0-9-]+)\.txt)")
re_geometry = re.compile("(?P<width>[0-9]+)x(?P<height>[0-9]+)\+(?P<left>[0-9]+)\+(?P<top>[0-9]+)")
log = Log()

def list_files(path,firmname,filemask):
	res = []
	for filename in os.listdir(os.path.join(path,firmname)):
		if mask.match(filename):
			res.append(os.path.join(path,firmname,filename))
	return res
def read_file_protek(filename,codepage,firm,date_format):
	log.log("Разбор заголовка \"%s\""%(filename))
	lines = open(filename,encoding = codepage).readlines()
	head = lines[1].split(";")
	invoice = Invoice()
	invoice.firm_code = firm[0]
	invoice.firm_name = firm[1]
	invoice.number = head[0]
	invoice.date = datetime.strptime(head[1],date_format)
	invoice.doc_datetime = datetime.now()
	if invoice.doc_datetime.year != invoice.date.year or invoice.doc_datetime.month != invoice.date.month:
		y = invoice.date.month < 12 and invoice.date.year or invoice.date.year + 1
		m = invoice.date.month < 12 and invoice.date.month + 1 or 1
		d=(date(y,m,1)+timedelta(days=-1))
		invoice.doc_datetime=datetime(d.year,d.month,d.day,23,59,59)
	invoice.payment_days = int(head[16])
	invoice.sum_2 = float(head[3])
	invoice.sum_vat = float(head[5])
	indx = 4
	for line in lines[3:]:
		log.log("Строка:%s"%(indx))
		l = line.split(";")
		order = InvoiceOrder()
		order.product_code = l[0]
		order.product_barcode = l[9]
		order.product_name = l[1]
		order.product_manufactor = l[2]
		order.product_coutry = l[3]
		order.quant = float(l[4])
		order.vat = float(l[6])
		order.price_40 = float(l[7])
		order.sum_2 = float(l[14])
		certificate = l[12].split("^")[0].split("|")
		order.series = certificate[0]
		log.log(certificate[4])
		order.lifetime = datetime.strptime(certificate[4],date_format)
		order.certificate = certificate[1]
		order.certificate_date = datetime.strptime(certificate[2],date_format)
		order.certificate_owner = certificate[3]
		invoice.orders.append(order)
		indx+= 1
	return invoice
def get_partner_payment_days(alias):
	a = Alias()
	k,p = a.PartnerKey0(), a.Partner()
	k.set(["03",invoice.firm_code])
	p.set_equal(k)
	k,o = a.DivisionKey0(),a.Division()
	k.set([""])
	o.get_equal(k)
	days = 0
	param = "ОТСРОЧ"+o.param("07")
	if p.Param(param):
		try: days = int(p.Param(param))
		except: pass
	if not days:
		try: days = int(p.Param("ОТСРОЧ"))
		except: pass
	return days
class MyWindow(Gtk.Window):
	def __init__(self, **kwargs):
		Gtk.Window.__init__(self, **kwargs)
		self.set_icon_name("bookcase")
		self.settings =  Gio.Settings.new("org.glekar.domino.import")
	def change_font_style(self, default, font):
		if first_start and default: return
		style_provider = Gtk.CssProvider()
		data = bytes("* { font: %s; }"%(default and "default" or font), "utf-8")
		style_provider.load_from_data(data)
		Gtk.StyleContext.add_provider_for_screen(
			Gdk.Screen.get_default(), 
			style_provider,     
			Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION
		)
	def save_geometry(self,settings):
		if self.get_window():
			geometry = "%dx%d+%d+%d"%(self.get_size()[0],self.get_size()[1],self.get_position()[0],self.get_position()[1])
			is_maximized = self.get_window().get_state() & Gdk.WindowState.MAXIMIZED
			if not is_maximized:
				settings.set_string("geometry", geometry)
			settings.set_boolean("maximized",is_maximized)
	def set_geometry(self,settings):
		maximized = settings.get_boolean("maximized")
		if maximized:
			self.maximize()
		else:
			self.unmaximize()
		geometry_string = settings.get_string("geometry")
		m = re_geometry.match(geometry_string)
		if m:
			self.move(int(m.group("left")),int(m.group("top")))
			self.set_default_size(int(m.group("width")),int(m.group("height")))
		pass
class SettingWindow(MyWindow):
	def __init__(self,parent):
		MyWindow.__init__(self, title = "Параметры")
		self.set_transient_for(parent)
		self.set_position(Gtk.WindowPosition.CENTER_ON_PARENT)
		vbox = Gtk.Box(spacing = 0,orientation = Gtk.Orientation.VERTICAL)
		vbox.set_border_width(16)
		self.add(vbox)
		note = Gtk.Notebook()
		grid = Gtk.Grid()
		grid.set_row_spacing(16)
		grid.set_column_spacing(16)
		grid.set_border_width(16)
		l = Gtk.Label("Шрифт")
		e = Gtk.FontButton()
		s = Gtk.Switch()
		grid.attach(l,0,0,1,1)
		grid.attach(e,2,0,1,1)
		grid.attach(s,1,0,1,1)
		note.insert_page(grid,Gtk.Label("Основные"),-1)
		vbox.pack_start(note,True,True,0)
		self.parent = parent
		self.coef_editor = e
		s.set_active(not self.settings.get_boolean("default-font"))
		e.set_font_name(self.settings.get_string("font"))
		s.connect("notify::active", self.font_changed1)
		e.connect("font-set", self.font_changed)
		self.font_switch = s
		self.font = e
	def font_changed1(self,arg,pp):
		self.font_changed(arg)
	def font_changed(self,arg):
		self.settings.set_boolean("default-font", not self.font_switch.get_active())
		self.settings.set_string("font", self.font.get_font_name())
		self.change_font_style(self.settings.get_boolean("default-font"),self.settings.get_string("font"))
class ImportWindow(MyWindow):
	def __init__(self):
		MyWindow.__init__(self, title = "Импорт накладных")
		self.add_my_css_style()
		self.change_font_style(self.settings.get_boolean("default-font"),self.settings.get_string("font"))
		self.lsettings = self.settings.get_child("invoices")
		self.connect("delete-event", self.real_close)
		self.connect("key-press-event", self.key_press)
		vbox = Gtk.Box(spacing = 0,orientation = Gtk.Orientation.VERTICAL)
		self.add(vbox)
		tool = self.make_tool()
		if gtk_minor >=  10:
			self.set_titlebar(tool)
		else:
			self.set_hide_titlebar_when_maximized(True)
			vbox.pack_start(tool,False,False,0)
		hbox = Gtk.Box(spacing = 16,orientation = Gtk.Orientation.HORIZONTAL)
		vbox.pack_start(hbox,True,True,0)
		hbox.set_border_width(16)
		supplier_view,widget = self.make_supplier_view()
		hbox.pack_start(widget,True,True,0)
		self.note = note = Gtk.Notebook()
		note.set_show_tabs(False)
		invoice_view,widget = self.make_invoice_view()
		note.insert_page(widget, None, 0)
		widget,self.log_view = self.make_log_view()
		note.insert_page(widget, None, 1)
		widget = Gtk.Label()
		widget.set_markup("<b>Накладных не обнаружено</b>") 
		note.insert_page(widget, None, 2)
		hbox.pack_start(note,True,True,0)
		supplier_view.get_selection().connect("changed", self.supplier_changed, invoice_view.get_model())
		self.fill_supplier(supplier_view.get_model())
		self.set_geometry(self.lsettings)
		self.supplier_view = supplier_view
		self.invoice_view = invoice_view
		GObject.idle_add(self.grab_supplier_view)
	def key_press(self,widget,event):
		#keyname = Gdk.keyval_name(event.keyval)
		if event.keyval == Gdk.KEY_F5:
			self.refresh(None)
			return True
		if True or event.state & Gdk.ModifierType.CONTROL_MASK:
			if hardware_keycode_is_keyval(event, Gdk.KEY_Q):
				Gtk.main_quit()
				return True
		return False
	def add_my_css_style(self):
		pass
	def grab_supplier_view(self):
		self.supplier_view.grab_focus()
	def make_tool(self):
		if gtk_minor >=  10:
			tool = Gtk.HeaderBar()
		else:
			tool = Gtk.Toolbar()
		button = Gtk.ToolButton()
		button.set_icon_name("view-refresh-symbolic")
		button.connect("clicked", self.refresh)
		button1 = Gtk.ToolButton()
		button1.set_icon_name("emblem-system-symbolic")
		button1.connect("clicked", self.property_)
		if gtk_minor >=  10:
			tool.pack_end(button)
			tool.pack_end(button1)
			tool.set_show_close_button(True)
			tool.set_title("Импорт накладных")
			
		else:
			sep = Gtk.SeparatorToolItem()
			sep.set_expand(True)
			sep.set_draw(False)
			tool.insert(sep,-1)
			tool.set_style(Gtk.ToolbarStyle.ICONS)
			tool.set_icon_size(Gtk.IconSize.MENU)
			style_context = tool.get_style_context()
			if style_context:
				style_context.add_class(Gtk.STYLE_CLASS_PRIMARY_TOOLBAR)
			tool.insert(button,-1)
			tool.insert(button1,-1)
		return tool
	def property_(self,action):
		win = SettingWindow(self)
		win.show_all()
	def refresh(self,action):
		self.supplier_changed(self.supplier_view.get_selection(), self.invoice_view.get_model())
	def make_supplier_view(self):
		view = Gtk.TreeView()
		view.connect("row-activated", self.supplier_row_activate)
		view.set_headers_visible(False)
		view.set_model(Gtk.ListStore(str,int,str,str,str))
		selection = view.get_selection()
		renderer = Gtk.CellRendererText()
		column = Gtk.TreeViewColumn("Поставщик", renderer, text = 0)
		view.append_column(column)
		renderer = Gtk.CellRendererPixbuf()
		column = Gtk.TreeViewColumn("", renderer)
		view.append_column(column)
		scroll = Gtk.ScrolledWindow()
		scroll.set_size_request(_pix,_pix*2)
		scroll.set_shadow_type(Gtk.ShadowType.IN)
		scroll.add(view)
		return (view,scroll)
	def make_invoice_view(self):
		scroll = Gtk.ScrolledWindow()
		scroll.set_size_request(_pix*2,_pix*2)
		scroll.set_shadow_type(Gtk.ShadowType.IN)
		view = Gtk.TreeView()
		view.connect("row-activated", self.invoice_row_activate)
		model = Gtk.ListStore(str,int,float,str,int)
		view.set_model(model)
		scroll.add(view)
		renderer = Gtk.CellRendererText()
		column = Gtk.TreeViewColumn("Накладная", renderer, text = 0)
		column.set_sort_column_id(0)
		view.append_column(column)
		renderer = Gtk.CellRendererText()
		column = Gtk.TreeViewColumn("Дата", renderer, text = 1)
		column.set_cell_data_func(renderer,datafunc.timestamp_date,1)
		column.set_sort_column_id(1)
		view.append_column(column)
		renderer = Gtk.CellRendererText()
		renderer.set_property("xalign", 1)
		column = Gtk.TreeViewColumn("Сумма", renderer, text = 2)
		column.set_cell_data_func(renderer,datafunc.money,2)
		column.set_sort_column_id(2)
		view.append_column(column)
		renderer = Gtk.CellRendererText()
		column = Gtk.TreeViewColumn("Товар", renderer, text = 3)
		column.set_sort_column_id(3)
		view.append_column(column)
		return (view,scroll)
	def make_log_view(self):
		vbox = Gtk.Box(spacing = 6,orientation = Gtk.Orientation.VERTICAL)
		scroll = Gtk.ScrolledWindow()
		scroll.set_size_request(_pix*2,-1)
		scroll.set_shadow_type(Gtk.ShadowType.IN)
		view = Gtk.TextView()
		scroll.add(view)
		label = Gtk.Label()
		label.set_property("xalign",0)
		label.set_markup("<b>Ошибки:</b>")
		vbox.pack_start(label, False, False, 0)
		vbox.pack_start(scroll, True, True, 0)
		return vbox, view
	def real_close(self,e,d):
		self.save_geometry(self.lsettings)
	def fill_supplier(self,model):
		a = Alias()
		k = a.PartnerKey0()
		p = a.Partner()
		model.clear()
		self.supplier_view = {}
		list_ = []
		for name in suppliers:
			k.set(["03",suppliers[name][0]])
			p.get_equal(k)
			list_.append([p.name,suppliers[name][0],name,suppliers[name][1],suppliers[name][2]])  
		model.clear()
		list_.sort(key = lambda x:x[2])
		for i in list_:
			model.append(i)
	def fill_invoices(self,firm,invoice_model,name):
		invoice_model.clear()
		invoices = []
		log.clear()
		self.invoices = []
		try:
			l = list_files(invoice_path, firm, mask)
			invoide = None
			for f in l:
				invoice = eval("read_file_%s(f,suppliers[firm][1],(suppliers[firm][0],name),suppliers[firm][2])"%firm)
				invoices.append([invoice,f])
			if not invoices:
				self.note.set_current_page(2)
				return
		except Exception as inst:
			log.log(inst.__str__())
			self.log_view.get_buffer().set_text(log.messages)
			self.note.set_current_page(1)
			return
		self.note.set_current_page(0)
		self.invoices = invoices
		invoices.sort(key = lambda x: x[0].number)
		#invoices.sort(key = lambda x: x.date)
		for indx in range(len(invoices)):
			invoice = invoices[indx]
			product = not invoice[0].orders and "" or invoice[0].orders[0].product_name
			invoice_model.append([invoice[0].number,invoice[0].date.timestamp(),invoice[0].sum_2,product,indx])
		if self.get_focus() !=  self.invoice_view and len(suppliers) == 1:
			GObject.idle_add(self.invoice_view.grab_focus)
	def supplier_changed(self,sel, invoice_model):
		model, tree_paths = sel.get_selected_rows()
		if not tree_paths:
			return
		iter_ = model.get_iter(tree_paths[0])
		firm = model.get_value(iter_,2)
		self.fill_invoices(firm,invoice_model, model.get_value(iter_,0))
	def supplier_row_activate(self, view, path, col):
		model = self.invoice_view.get_model()
		if model.get_iter_first():
			GObject.idle_add(self.invoice_view.grab_focus)
	def invoice_row_activate(self, view, path, col):
		model = self.invoice_view.get_model()
		iter_ = model.get_iter(path)
		indx = model.get_value(iter_, 4)
		invoice,filename = self.invoices[indx]
		win = InvoiceWindow(self,invoice,filename)
		win.show_all()
class InvoiceWindow(MyWindow):
	def __init__(self, parent, invoice, filename):
		MyWindow.__init__(self, title = "{.firm_name} № {.number} от {} ".format(invoice, invoice, invoice.date.strftime(date_format)))
		self.set_transient_for(parent)
		self.invoice = invoice
		self.filename = filename
		self.set_default_size(800,600)
		self.set_position(Gtk.WindowPosition.CENTER_ON_PARENT)
		self.lsettings = self.settings.get_child("invoice")
		self.set_geometry(self.lsettings)
		self.connect("delete-event", self.real_close)
		self.connect("key-press-event", self.key_press)
		if gtk_minor >=  10:
			tool = Gtk.HeaderBar()
			tool.set_show_close_button(True)
			tool.set_title(self.get_title()) 
			self.set_titlebar(tool)
		vbox = Gtk.Box(spacing = 6,orientation = Gtk.Orientation.VERTICAL)
		self.add(vbox)
		vbox.set_border_width(16)
		head = self.make_header()
		vbox.pack_start(head, False, True, 0)
		widget,self.view = self.make_view()
		vbox.pack_start(widget, True, True, 0)
		self.fill_model()
	def real_close(self,e,d):
		self.save_geometry(self.lsettings)
	def key_press(self,widget,event):
		keyname = Gdk.keyval_name(event.keyval)
		if event.state & Gdk.ModifierType.CONTROL_MASK:
			if keyname == "q":
				self.destroy()
				return True
		return False
	def make_header(self):
		invoice = self.invoice
		grid = Gtk.Grid()
		grid.set_column_spacing(6)
		grid.set_row_spacing(6)
		l = Gtk.Label()
		l.set_markup_with_mnemonic("П_оступил:")
		l.set_property("xalign", 0)
		grid.attach(l,0,0,1,1)
		e = DateEntry()
		e.add_validate_funcs(self.check_doc_date, self.get_error_check_doc_date)
		e.connect("go", self.go_next)
		e.set_value(invoice.doc_datetime)
		grid.attach(e,1,0,1,1)
		l = Gtk.Label()
		self.desc = l
		self.entry_doc_date = e
		self.make_desc()
		grid.attach(l,2,0,1,1)
		return grid
	def go_next(self,entry):
		if entry == self.entry_doc_date:
			self.view.grab_focus()
	def check_doc_date(self, entry):
		return (entry.value >= self.invoice.date) and (entry.value + timedelta(days=-max_up_days) < self.invoice.date) 
	def get_error_check_doc_date(self, entry):
		if not (entry.value >= self.invoice.date): 
			return "Дата поступления товара {} меньше даты накладной {}.".format(entry.value.strftime("%d.%m.%y"),self.invoice.date.strftime("%d.%m.%y"))  
		elif not (entry.value + timedelta(days=-max_up_days) < self.invoice.date):
			return "Дата поступления товара {} превышает дату накладной {} более чем на {} дней.".format(entry.value.strftime("%d.%m.%y"),self.invoice.date.strftime("%d.%m.%y"),max_up_days)  
		return ""
	def make_desc(self):
		invoice = self.invoice
		d=invoice.doc_datetime #!!
		if invoice.payment_date:
			days =invoice.doc_datetime - invoice.date
			pay = invoice.payment_date + timedelta(days=((days > 0 ) and days or 0))
		elif invoice.payment_days:
			pay = invoice.doc_datetime + timedelta(days=invoice.payment_days)
		else:
			days = get_partner_payment_days(Alias(), invoice.firm_code)
			if days:
				invoice.payment_days = days
				pay = invoice.payment_date + timedelta(days=days)
			else:
				pay = None
		if pay: str_ = "Оплата до: <b>{}</b> ".format(pay.strftime(date_format))
		else: str_ = ""
		str_ += "Сумма НДС: <b>{}</b> Сумма: <b>{}</b>".format(currency(invoice.sum_vat,grouping=True),currency(invoice.sum_2,grouping=True))
		self.desc.set_markup(str_)
	def make_view(self):
		view = Gtk.TreeView()
		view.set_search_column(2)
		#view.set_grid_lines(Gtk.TreeViewGridLines.BOTH) 
		model = Gtk.ListStore(int,str,str,str,float,float,float,str,int,float)
		view.set_model(model)
		view.set_fixed_height_mode(False)
		renderer = Gtk.CellRendererText()
		renderer.set_property("xalign", 1)
		column = Gtk.TreeViewColumn("№", renderer, text = 0)
		column.set_sort_column_id(0)
		view.append_column(column)
		renderer = Gtk.CellRendererText()
		column = Gtk.TreeViewColumn("Код", renderer, text = 1)
		column.set_sort_column_id(1)
		view.append_column(column)
		
		renderer = Gtk.CellRendererText()
		renderer.set_property("wrap-mode",Pango.WrapMode.WORD)
		renderer.set_property("wrap-width",390)
		column = Gtk.TreeViewColumn("Наименование", renderer, text = 2)
		#column.set_resizable(True)
		column.set_sizing(Gtk.TreeViewColumnSizing.FIXED)
		column.set_fixed_width(1000)
		
		column.set_sort_column_id(2)
		renderer = Gtk.CellRendererText()
		view.append_column(column)
		renderer.set_property("xalign", 0.5)
		column = Gtk.TreeViewColumn("ЕИ", renderer, text = 3)
		column.set_sort_column_id(3)
		view.append_column(column)
		renderer = Gtk.CellRendererText()
		renderer.set_property("xalign", 0.5)
		column = Gtk.TreeViewColumn("Кол-во", renderer, text = 4)
		column.set_cell_data_func(renderer,datafunc.quantity,4)
		column.set_sort_column_id(4)
		view.append_column(column)
		renderer = Gtk.CellRendererText()
		renderer.set_property("xalign", 1)
		column = Gtk.TreeViewColumn("Закуп", renderer, text = 5)
		column.set_cell_data_func(renderer,datafunc.money_no_symbol,5)
		column.set_sort_column_id(5)
		view.append_column(column)
		renderer = Gtk.CellRendererText()
		renderer.set_property("xalign", 1)
		column = Gtk.TreeViewColumn("Розница", renderer, text = 6)
		column.set_cell_data_func(renderer,datafunc.money_no_symbol,6)
		column.set_sort_column_id(6)
		view.append_column(column)
		renderer = Gtk.CellRendererText()
		renderer.set_property("xalign", 0.5)
		column = Gtk.TreeViewColumn("Тип", renderer, text = 7)
		column.set_sort_column_id(7)
		view.append_column(column)
		renderer = Gtk.CellRendererText()
		renderer.set_property("xalign", 0.5)
		column = Gtk.TreeViewColumn("НДС", renderer, text = 8)
		column.set_sort_column_id(8)
		view.append_column(column)
		renderer = Gtk.CellRendererText()
		renderer.set_property("xalign", 1)
		column = Gtk.TreeViewColumn("Сумма", renderer, text = 9)
		column.set_cell_data_func(renderer,datafunc.money_no_symbol,9)
		column.set_sort_column_id(9)
		view.append_column(column)
		scroll = Gtk.ScrolledWindow()
		#scroll.set_size_request(_pix,_pix*2)
		scroll.set_shadow_type(Gtk.ShadowType.IN)
		scroll.add(view)
		return scroll,view
	def fill_model(self):
		model = self.view.get_model()
		for indx in range(len(self.invoice.orders)):
			order = self.invoice.orders[indx]
			model.append([
				indx+1,
				"00000",#!!
				order.product_name,
				"УПАК",#!!
				order.quant,
				order.sum_2/order.quant,
				order.sum_2/order.quant*1.20,#!!
				"ЖНВЛС",#!!
				order.vat,#!!
				order.sum_2,
			])
if __name__ == "__main__":
	win = ImportWindow()
	win.connect("delete-event", Gtk.main_quit)
	win.show_all()
	Gtk.main()
	

