#!/bin/env python 
# -*- coding: UTF-8 -*-

from gi.repository import Gtk,Gdk,GObject
from datetime import datetime,timedelta
import re

date_format = "%d.%m.%y"
datetime_format = "%d.%m.%y %H:%M:%S"
date_masks = [
"(?P<d>[0-3][0-9])(?P<m>[0-1][0-9])(?P<y>[0-9][0-9])",
"(?P<d>[0-3][0-9])(?P<m>[0-1][0-9])(?P<y>[1-2][0-9][0-9][0-9])",
"(?P<d>[0-3][0-9])\.(?P<m>[0-1][0-9])\.(?P<y>[0-9][0-9])",
"(?P<d>[0-3][0-9])\.(?P<m>[0-1][0-9])\.(?P<y>[1-2][0-9][0-9][0-9])",
"(?P<d>[0-3][0-9])\/(?P<m>[0-1][0-9])\/(?P<y>[0-9][0-9])",
"(?P<d>[0-3][0-9])\/(?P<m>[0-1][0-9])\/(?P<y>[1-2][0-9][0-9][0-9])",
]
time_masks = [
"(?P<H>[0-9][0-9])\:(?P<M>[0-9][0-9])\:(?P<S>[0-9][0-9])",
"(?P<H>[0-9][0-9])(?P<M>[0-9][0-9])(?P<S>[0-9][0-9])"
]
date_mask = []
for mask in date_masks:
	date_mask.append([re.compile("{}$".format(mask)),False])
	for tmask in time_masks:
		date_mask.append([re.compile("{}\s+{}$".format(mask,tmask)),True])

class DateEntry1(Gtk.Box):
	def __init__(self):
		Gtk.Box.__init__(self,spacing = 0,orientation = Gtk.Orientation.HORIZONTAL)
		self.entry = Gtk.Entry()
		self.button = Gtk.Button()
		#image = Gtk.Image.new_from_icon_name("go-down-symbolic", Gtk.IconSize.MENU)
		image = Gtk.Image.new_from_icon_name("content-loading-symbolic", Gtk.IconSize.MENU)
		self.button.add(image)
		self.pack_start(self.entry, True, True, 0)
		self.pack_start(self.button, False, True, 0)

class MyEntry(Gtk.Entry):
	__gsignals__ = {'go': (GObject.SIGNAL_RUN_FIRST, None, ())}
	def __init__(self):
		Gtk.Entry.__init__(self)
		self.connect("activate",self.activate)
		self.connect("focus-out-event",self.focus_out)
		self.connect("notify::text", self.text_changed)
		self.connect("icon-press",self.icon_press_error)
		self.list_validate = [] 
		self.last_error=""
	def icon_press_error(self,entry,pos,event):
		if pos == Gtk.EntryIconPosition.PRIMARY and self.last_error:
			parent = self.get_toplevel()
			dialog = Gtk.MessageDialog(parent, 0, Gtk.MessageType.ERROR,
										Gtk.ButtonsType.OK,self.last_error)
			dialog.set_icon_name("bookcase")
			dialog.run()
			dialog.destroy()
	def focus_out(self, entry, s):
		if self.validate():
			self.update_value()
		self.select_region(0,0)
	def update_value(self):
		pass
	def validate(self):
		ctx = self.get_style_context()
		for v in self.list_validate:
			f1,f2 = v[0],v[1]
			if not f1(self): 
				ctx.add_class("invalid")
				self.last_error = f2(self)
				self.set_icon_from_icon_name(Gtk.EntryIconPosition.PRIMARY,"software-update-urgent-symbolic")
				return False
		self.set_icon_from_icon_name(Gtk.EntryIconPosition.PRIMARY,"")
		ctx.remove_class("invalid")
		return True
	def text_changed(self,arg,pp):
		self.validate()
	def activate(self,arg):
		if self.validate():
			self.emit("go")
	def add_validate_funcs(self, validate_func, error_func):
		self.list_validate.append([validate_func, error_func])
class DateEntry(MyEntry):
	def __init__(self):
		MyEntry.__init__(self)
		ctx = self.get_style_context()
		self.set_icon_from_icon_name(Gtk.EntryIconPosition.SECONDARY,"content-loading-symbolic")
		self.set_icon_tooltip_text(Gtk.EntryIconPosition.SECONDARY,"Календарь")
		self.connect("icon-press",self.icon_press)
		self.entry=self
		self.set_size_request(200,-1)
		self.add_validate_funcs(self.str_to_date,None)
	def update_value(self):
		if not self.value.hour and not self.value.minute and not self.value.second:
			self.set_text(self.value.strftime(date_format))
		else:
			self.set_text(self.value.strftime(datetime_format))
	def set_value(self, d):
		self.value = d
		self.update_value()
	def get_value(self):
		return self.value
	def icon_press(self,entry,pos,event):
		if pos == Gtk.EntryIconPosition.SECONDARY:
			self.calendar()
	def calendar(self):
		parent = self.get_toplevel()
		dialog = Gtk.Dialog(title="Выбор даты")
		dialog.add_buttons("Отмена",Gtk.ResponseType.CANCEL,"Сегодня",1, "Ок", Gtk.ResponseType.ACCEPT)
		dialog.set_icon_name("bookcase")
		dialog.set_transient_for(parent)
		dialog.set_position(Gtk.WindowPosition.CENTER_ON_PARENT)
		dialog.set_modal(True)
		calendar = Gtk.Calendar()
		dialog.get_content_area().add(calendar)
		calendar.connect("day-selected-double-click",self.calendar_event,dialog)
		if self.value:
			calendar.select_month(self.value.month - 1, self.value.year);
			calendar.select_day(self.value.day)
		dialog.show_all()
		result = dialog.run()
		if result == Gtk.ResponseType.ACCEPT:
			y,m,d=calendar.get_date()
			self.set_value(datetime(y,m+1,d))
		elif result == 1:
			self.set_value(datetime.today())
		dialog.destroy()
	def calendar_event(self,ai,dialog):
		dialog.response(Gtk.ResponseType.ACCEPT)
	def str_to_date(self,obj):
		fail = True 
		text = self.get_text()
		for mask in date_mask:
			m1 = mask[0].match(text)
			if m1:
				y,m,d = int(m1.group("y")),int(m1.group("m")),int(m1.group("d"))
				if (m < 1) or (m > 12) or (d < 1) or (d > 31): continue
				if mask[1] and m1.group("H"):
					h,mi,s = int(m1.group("H")),int(m1.group("M")),int(m1.group("S"))
					if (h > 23) or (mi > 59) or (s > 59): continue
				if y < 100: y+=2000
				try:
					if mask[1] and m1.group("H"):
						d1 = datetime(y,m,d,h,mi,s)
					else:
						d1 = datetime(y,m,d)
					self.value = d1
					fail = False
					break
				except: continue
		return not fail
def hardware_keycode_is_keyval(event, keyval):
	#keymap = Gdk.Keymap.get_for_display (Gdk.Display.get_default())
	keymap = Gdk.Keymap.get_for_display(event.window)
	entries = keymap.get_entries_for_keycode(event.hardware_keycode)
	print(entries)
	print(keyval)
	return  keyval in entries

sad=Gtk.Entry()
ctx = sad.get_style_context()
str_="""
 GtkEntry.invalid 
 { 
    background: #ffeaea; 
 }
 GtkEntry.invalid:selected  
 { 
    background-color: %s; 
 }
"""%(ctx.get_background_color(Gtk.StateFlags.SELECTED).to_string())
cssProvider = Gtk.CssProvider()
cssProvider.load_from_data(bytes(str_,"UTF-8"))
screen = Gdk.Screen.get_default()
styleContext = Gtk.StyleContext()
styleContext.add_provider_for_screen(screen, cssProvider,
	Gtk.STYLE_PROVIDER_PRIORITY_USER)
