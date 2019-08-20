#!/bin/bash
umask 022
export PVSW_ROOT=/usr/local/psql
export DOMINO_ROOT=/usr/local/domino
export PATH=/usr/local/psql/bin:/usr/local/domino/bin:/usr/local/sbin:/usr/sbin:/sbin:/usr/local/bin:/usr/bin:/bin:/usr/X11R6/bin:/usr/games:/usr/lib/java/bin:/usr/lib/java/jre/bin
export LD_LIBRARY_PATH=/usr/local/psql/lib:/usr/local/domino/lib:/usr/lib
export LD_BIND_NOW=1
export ESCDELAY=0
while true ; do
/usr/local/domino/bin/handbook
done  