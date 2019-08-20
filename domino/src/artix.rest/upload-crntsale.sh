#!/bin/bash
# -*- coding: utf-8 -*-

#echo `date` >>/linuxcash/net/domino/last
#cd /usr/local/domino/bin; ./load-handbook.py >../log/load.log 2>../log/load.err" | su - domino || exit 1
echo "cd /usr/local/domino/bin; ./upload-crntsale.py >../log/upload.log 2>../log/upload.err" | su - domino || exit 1