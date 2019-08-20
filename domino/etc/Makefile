include ../common.mak
all:

clean:

install:
	if [ ! -d $(DOMINO_ETC)/alias ] ; then \
	mkdir -p $(DOMINO_ETC)/alias; fi 
	if [ ! -f $(DOMINO_ETC)/alias/local ] ; then \
	cp alias/local $(DOMINO_ETC)/alias/local; fi 
	cp domino.cfg $(DOMINO_ETC)/domino.cfg
	cp alias/main $(DOMINO_ETC)/alias/main
	cp alias/main.* $(DOMINO_ETC)/alias/
	cp common.xml $(DOMINO_ETC)/common.xml
	if [ ! -f $(DOMINO_ETC)/dclubd.conf ] ; then \
	cp dclubd.conf $(DOMINO_ETC)/dclubd.conf ; fi 
	if [ ! -f $(DOMINO_ETC)/exchanged.conf ] ; then \
	cp exchanged.conf $(DOMINO_ETC)/exchanged.conf ; fi 
	if [ ! -f $(DOMINO_ETC)/syncd.conf ] ; then \
	cp syncd.conf $(DOMINO_ETC)/syncd.conf ; fi 
	if [ ! -f $(DOMINO_ETC)/domino.conf ] ; then \
	cp domino.conf $(DOMINO_ETC)/domino.conf ; fi 
	if [ ! -f $(DOMINO_ETC)/doshadow ] ; then \
	cp doshadow $(DOMINO_ETC)/doshadow; chmod 644 $(DOMINO_ETC)/doshadow; chown $(USERNAME):$(GROUPNAME) $(DOMINO_ETC)/doshadow;fi 
	cp send-error.conf $(DOMINO_ETC)/send-error.conf 
	cp syncd-document.conf $(DOMINO_ETC)/syncd-document.conf
	if [ ! -f $(DOMINO_ETC)/alias.xml ] ; then \
        cp alias.xml $(DOMINO_ETC)/alias.xml ; fi
	if [ ! -f $(DOMINO_ETC)/grlspl.xml ] ; then \
        cp grlspl.xml $(DOMINO_ETC)/grlspl.xml ; fi
	if [ ! -f $(DOMINO_ETC)/handbook-text.conf ] ; then \
        cp handbook-text.conf $(DOMINO_ETC)/handbook-text.conf ; fi

uninstall:
	rm -f $(DOMINO_ETC)/alias/local; \
	rm -f $(DOMINO_ETC)/domino.cfg; \
	rm -f $(DOMINO_ETC)/domino.conf; \
	rm -f $(DOMINO_ETC)/dclubd.conf; \
	rm -f $(DOMINO_ETC)/send_error.conf; \
	rm -f $(DOMINO_ETC)/exchanged.conf; \
	rm -f $(DOMINO_ETC)/doshadow; \
	rm -f $(DOMINO_ETC)/syncd-document.conf; \
	rm -f $(DOMINO_ETC)/alias.xml; \
	if [ -d $(DOMINO_ETC)/alias ] ; then \
	rmdir -p $(DOMINO_ETC)/alias; fi
    


