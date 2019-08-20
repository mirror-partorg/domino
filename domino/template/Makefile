include ../common.mak

all:

clean:

install:
	if [ ! -d $(DOMINO_TEMPLATE) ] ; then \
	mkdir -p $(DOMINO_TEMPLATE); fi 
	cp *.html $(DOMINO_TEMPLATE)

uninstall:
	rm -f $(DOMINO_TEMPLATE)/*.html; \
	if [ -d $(DOMINO_TEMPLATE) ] ; then \
	rmdir -p $(DOMINO_TEMPLATE); fi
    


