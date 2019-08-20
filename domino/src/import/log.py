#!/bin/env python
#-*- UTF-8 -*-

class Log:
	def __init__(self):
		self.messages=""
	def log(self,message):
		self.messages+=message+"\n"
	def clear(self):
		self.messages=""
	def print(self):
		print(self.messages)
