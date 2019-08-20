#!/bin/bash
# -*- coding: utf-8 -*-

local_mail=/linuxcash/cash/mail
net_mail=/linuxcash/net/domino/domino/mail/from_kas
path=/usr/local/domino/mail/cl

echo "cd /usr/local/domino/bin; ./upload-sale.py $1 2>../log/upload-sale.err" | su - domino || exit 1
mv -f $path/* $net_mail/ || mv -f $path/* $local_mail/


