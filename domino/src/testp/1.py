#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import domino
#import cStringIO

#s=cStringIO.StringIO("d")
alias=domino.Alias("local.tta")
alias.connect()

print(alias,alias.get_name(),alias.get_host(),alias.connected())
