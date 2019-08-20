#!/bin/bash
# -*- coding: utf-8 -*-

echo "cd /usr/local/domino/bin; ./load-handbook.py >../log/load.log 2>../log/load.err" | su - domino || exit 1