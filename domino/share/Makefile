include ../common.mak

all:

clean:

install:
	if [ ! -d $(DOMINO_SHARE)/domino ] ; then \
	mkdir -p $(DOMINO_SHARE)/domino/; fi 
	cp ./domino/*.ui   $(DOMINO_SHARE)/domino/

uninstall:
	rm -f $(DOMINO_SHARE)/domino/*; \
	if [ -d $(DOMINO_SHARE) ] ; then \
	rmdir -p $(DOMINO_SHARE); fi
    


