#!/usr/bin/env python
# -*- coding: utf-8 -*-

from gi.repository import Gtk
from gi.repository import GtkDomino

class Handler:
    def onDeleteWindow(self, *args):
        Gtk.main_quit(*args)

def show(a,b):
    print("sad")

builder = Gtk.Builder()
builder.add_from_file("/home/mirror/work/domino/src/src/handbook/one.glade")
builder.connect_signals(Handler())

window = builder.get_object("window1")
window.set_position(Gtk.WindowPosition.CENTER)

window.show_all()


Gtk.main()
