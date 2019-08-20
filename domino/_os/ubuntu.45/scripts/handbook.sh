#!/bin/bash

export DOMINO_ROOT=/usr/local/domino
export PATH=/usr/local/domino/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games
export LD_LIBRARY_PATH=/usr/local/domino/lib:/usr/local/psql/lib:/usr/lib
export LD_BIND_NOW=1

/usr/local/domino/bin/domino  --hidecontrol --fullscreen -u domino -p domino /MainMenu/Handbook/ProductExtended