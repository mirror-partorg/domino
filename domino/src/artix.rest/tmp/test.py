#!/bin/env python


import json
from restful_lib import Connection

import urllib2

json.load(urllib2.urlopen("kassa-99:5672"))

#print(json.dumps(['foo', {'bar': ('baz', None, 1.0, 2)}]))