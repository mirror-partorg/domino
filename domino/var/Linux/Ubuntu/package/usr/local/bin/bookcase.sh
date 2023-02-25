#!/bin/bash

xmodmap -e "keycode 77 = Num_Lock"

umask 022
export DOMINO_ROOT=/usr/local
export PATH=/usr/local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games
export LD_LIBRARY_PATH=/lib/i386-linux-gnu:/usr/lib/i386-linux-gnu:/usr/lib:/usr/local/lib:/usr/local/psql/lib
export LD_BIND_NOW=1

/usr/local/bin/bookcase --hidecontrol --fullscreen GoodsView
