DIR_TMP=$(TMP_DIR)/$(NAME)
LIB_DIR=$(TMP_DIR)/.libs
compile:
	mkdir -p $(DIR_TMP)
	if [[ "X$(PYTHON_VERSION_2)" != "X" ]] ; \
	then \
		ver=$(PYTHON_VERSION_2) ; \
		m="0" ; \
		echo for python-$$ver ; \
		for f in `cat var/files.lst` ; \
		do \
			n=$(DIR_TMP)/$$(basename $$f).o.$$ver ; \
			if [[ $$f -nt $$n ]] ; \
			then \
				echo $$f ; \
				m="1" ; \
				$(CC) -shared -fPIC -Wall \
				$(PYTHON_CFLAGS_2) $(PYTHON_LIBS_2) \
                         $(AM_FLAGS) $(CFLAGS) $(OPT_FLAGS) \
                         -c $$f -o $$n || exit 1; \
			fi ; \
		done ; \
		if [[ "$$m" == "1" || ! -f $(NAME)-$$ver ]] ; \
		then \
			OBJS="" ; \
			for f in `cat var/files.lst` ; \
			do \
				n=$(DIR_TMP)/$$(basename $$f).o.$$ver ; \
				OBJS="$$OBJS $$n"; \
			done ; \
			echo $$OBJS ; \
			$(LINK) -shared -fPIC -Wall  $(AM_FLAGS) $(CFLAGS) $(OPT_FLAGS) \
                  $$OBJS -o $(LIB_DIR)/$(NAME)-$$ver $(LDFLAGS) \
                  -Wl,-soname,$(LIB_DIR)/$(NAME)-$$ver ; \
		fi ; \
	fi
	if [[ "X$(PYTHON_VERSION_3)" != "X" ]] ; \
	then \
		ver=$(PYTHON_VERSION_3) ; \
		m="0" ; \
		echo for python-$$ver ; \
		for f in `cat var/files.lst` ; \
		do \
			n=$(DIR_TMP)/$$(basename $$f).o.$$ver ; \
			echo $$f; \
			echo $$n; \
			if [[ 1 -eq 1 ]] ; \
			then \
				echo $$f ; \
				m="1" ; \
				$(CC) -shared -fPIC -Wall \
				$(PYTHON_CFLAGS_3) \
                         $(AM_FLAGS) $(CFLAGS) $(OPT_FLAGS) $(PYTHON_LIBS_3) \
                         -c $$f -o $$n || exit 1; \
			fi ; \
		done ; \
		if [[ "$$m" == "1" || ! -f $(NAME)-$$ver ]] ; \
		then \
			OBJS="" ; \
			for f in `cat var/files.lst` ; \
			do \
				n=$(DIR_TMP)/$$(basename $$f).o.$$ver ; \
				OBJS="$$OBJS $$n"; \
			done ; \
			echo $$OBJS ; \
			$(LINK) -shared -fPIC -Wall  $(AM_FLAGS) $(CFLAGS) $(OPT_FLAGS) \
                  $$OBJS -o $(LIB_DIR)/$(NAME)-$$ver $(LDFLAGS) \
                  -Wl,-soname,$(LIB_DIR)/$(NAME)-$$ver $(PYTHON_LIBS_3) ; \
		fi ; \
	fi
install:
	for i in "$(PYTHON_VERSION_2) $(PYTHON_DIR_2)" "$(PYTHON_VERSION_3) $(PYTHON_DIR_3)" "$(PYTHON_VERSION_4) $(PYTHON_DIR_4)"; \
	   do if [[ "X$$i" != "X " ]] ; then  \
	      install -T $(LIB_DIR)/$(NAME)-$$i/$(NAME) ; \
	      fi ; \
	   done 
uninstall:
	for dir in $(PYTHON_DIR_2) $(PYTHON_DIR_3) $(PYTHON_DIR_4) ; \
	   do  \
	      rm -f $$dir/$(NAME) ; \
	   done 
clean:
	rm -fR $(DIR_TMP)
	for i in "$(PYTHON_VERSION_2) $(PYTHON_DIR_2)" "$(PYTHON_VERSION_3) $(PYTHON_DIR_3)" "$(PYTHON_VERSION_4) $(PYTHON_DIR_4)"; \
	   do if [[ "X$$i" != "X " ]] ; then  \
	      rm -f $(LIB_DIR)/$(NAME)-$$i/$(NAME) ; \
	      fi ; \
	   done 
