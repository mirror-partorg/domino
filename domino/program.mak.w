DIR_TMP=$(dir $(lastword $(MAKEFILE_LIST)))/$(TMP_DIR)/$(APP)
DIR_PRG=$(dir $(lastword $(MAKEFILE_LIST)))/$(TMP_DIR)/.progs
all: $(APP) $(APP_1)
$(APP):
	OBJS=""
	mkdir -p $(DIR_TMP)
	windres -o $(DIR_TMP)/$(OBJ) $(APP).rc
	for f in $(SRCS) ; do \
		n=$(DIR_TMP)/$$(basename $$f).o ; \
		if [[ $$f -nt $$n ]] ; \
		then \
			echo $$f ; \
			$(CC) $(AM_FLAGS) $(OPT_FLAGS) $(CFLAGS) -c $$f -o $$n; \
		fi ;\
	done
	for f in $(SRCS) ; do OBJS="$$OBJS $(DIR_TMP)/$$(basename $$f).o"; done ;\
	export LD_LIBRARY_PATH=/usr/local/psql/lib:/usr/local/psql/lib64:/usr/local/domino/lib && \
	g++ $(AM_FLAGS) $(OPT_FLAGS) $(CFLAGS) $(DIR_TMP)/$(OBJ) $$OBJS  -o $(DIR_PRG)/$(APP) $(LDFLAGS)
$(APP_1):
	OBJS=""
	mkdir -p $(DIR_TMP)
	for f in $(SRCS_1) ; do \
		n=$(DIR_TMP)/$$(basename $$f).o ; \
		if [[ $$f -nt $$n ]] ; \
		then \
			echo $$f ; \
			$(CC) $(AM_FLAGS) $(OPT_FLAGS) $(CFLAGS) -c $$f -o $$n; \
		fi ;\
	done
	for f in $(SRCS_1) ; do OBJS="$$OBJS $(DIR_TMP)/$$(basename $$f).o"; done ;\
	export LD_LIBRARY_PATH=/usr/local/psql/lib:/usr/local/psql/lib64:/usr/local/domino/lib && \
	g++ $(AM_FLAGS) $(OPT_FLAGS) $(CFLAGS) $$OBJS -o $(DIR_PRG)/$(APP_1) $(LDFLAGS)
install:
	install  $(DIR_PRG)/$(APP) $(DOMINO_BIN)/
	if [ "X$(APP_1)" != "X" ] ; then cp -f $(DIR_PRG)/$(APP_1) $(DOMINO_BIN)/$(APP_1) ; fi
	$(INSTALL)
clean:
	rm -fR $(DIR_TMP)
	rm -f $(DIR_PRG)/$(APP)
	if [ "X$(APP_1)" != "X" ] ; then rm -f $(DIR_PRG)/$(APP_1) ; fi
uninstall:
	rm -f $(DOMINO_BIN)/$(APP)
	if [  "X$(APP_1)" != "X" ] ; then rm -f $(DOMINO_BIN)/$(APP_1) ; fi
	$(UNINSTALL)
