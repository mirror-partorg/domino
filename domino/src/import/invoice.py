#!/bin/env python
#-*- coding: UTF-8 -*-

class Invoice:
	def __init__(self):
		self.firm_code=None
		self.firm_name=None
		self.number=None
		self.date=None
		self.doc_datetime=None
		self.payment_date=None
		self.payment_days=None
		self.sum_2=None
		self.sum_vat=None
		self.orders=[]
class InvoiceOrder:
	def __init__(self):
		self.product_code=None
		self.product_barcode=None
		self.product_name=None
		self.product_manufactor=None
		self.product_coutry=None
		self.unit=None
		self.unit_name=None
		self.quant=None
		self.vat=None
		self.price_40=None
		self.sum_2=None
		self.sum_vat=None
		self.series=None
		self.lifetime=None
		self.certificate=None
		self.certificate_date=None
		self.certificate_owner=None
		self.certificate_lifetime=None
