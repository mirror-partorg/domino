#!/bin/sh

echo "cd /usr/local/domino/bin; ./crntsale&" | su - domino || exit 1
if [ -f /usr/local/domino/bin/pgsale ] 
then
/usr/local/domino/bin/pgsale&
fi
