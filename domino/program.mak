DIR_TMP=$(TMP_DIR)/$(APP)
DIR_PRG=$(TMP_DIR)/.progs
all: $(APP) $(APP_1)
$(APP):
	OBJS=""
	$(SET_PATH)
	mkdir -p $(DIR_TMP)
	mkdir -p $(DIR_PRG)
	for f in $(SRCS) ; do \
		n=$(DIR_TMP)/$$(basename $$f).o ; \
		if [[ $$f -nt $$n ]] ; \
		then \
			echo $$f ; \
			set path=C:/Dev/mingw/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/bin;%PATH% ; \
			$(CC) $(AM_FLAGS) $(OPT_FLAGS) $(CFLAGS) -c $$f -o $$n; \
		fi ;\
	done
	$(RESOURCES)
	for f in $(SRCS) ; do OBJS="$$OBJS $(DIR_TMP)/$$(basename $$f).o"; done ;\
	export LD_LIBRARY_PATH=/usr/local/psql/lib:/usr/local/psql/lib64:/usr/local/domino/lib && \
	$(LINK) $(AM_FLAGS) $(OPT_FLAGS) $(CFLAGS) $$OBJS $(OBJ) -o $(DIR_PRG)/$(APP) $(LDFLAGS)
$(APP_1):
	OBJS=""
	$(SET_PATH)
	mkdir -p $(DIR_TMP)
	for f in $(SRCS_1) ; do \
		n=$(DIR_TMP)/$$(basename $$f).o ; \
		if [[ $$f -nt $$n ]] ; \
		then \
			echo $$f ; \
			$(CC) $(AM_FLAGS) $(OPT_FLAGS) $(CFLAGS) -c $$f -o $$n; \
		fi ;\
	done
	$(RESOURCES_1)
	for f in $(SRCS_1) ; do OBJS="$$OBJS $(DIR_TMP)/$$(basename $$f).o"; done ;\
	export LD_LIBRARY_PATH=/usr/local/psql/lib:/usr/local/psql/lib64:/usr/local/domino/lib && \
	$(LINK) $(AM_FLAGS) $(OPT_FLAGS) $(CFLAGS) $$OBJS $(OBJ_1) -o $(DIR_PRG)/$(APP_1) $(LDFLAGS)
$(APP_D):
	OBJS=""
	$(SET_PATH)
	mkdir -p $(DIR_TMP)
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
	$(LINK) $(AM_FLAGS) $(OPT_FLAGS) $(CFLAGS) $$OBJS $OBJ -o $(APP_D) $(LDFLAGS)
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
start:
	$(DOMINO_BIN)/$(APP)