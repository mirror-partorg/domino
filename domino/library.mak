DIR_TMP=$(TMP_DIR)/$(NAME)
LIB_DIR=$(TMP_DIR)/.libs
all: $(NAME)
$(NAME):
	$(PRE_NAME)
	OBJS=""
	mkdir -p $(DIR_TMP)
	mkdir -p $(LIB_DIR)
	for f in $(SRCS) ; do \
		n=$(DIR_TMP)/$$(basename $$f).o ; \
		if [[ $$f -nt $$n ]] ; \
		then \
			echo $$f ; \
			$(CC) -shared -fPIC $(AM_FLAGS) $(OPT_FLAGS) $(CFLAGS) -c $$f -o $$n; \
		fi ;\
	done
	for f in $(SRCS) ; do OBJS="$$OBJS $(DIR_TMP)/$$(basename $$f).o"; done 
	if [[ "$(MAKEDLL)" == "1" ]] ; \
	then \
		$(LINK) -shared -fPIC -Wall  $(AM_FLAGS) $(CFLAGS) $(OPT_FLAGS) \
                  $$OBJS -o $(LIB_DIR)/$(DLL) $(LDFLAGS) \
                  -Wl,-soname,$(NAME).$(VER) ; \
	else  \
		$(LINK) -shared -fPIC -Wall  $(AM_FLAGS) $(CFLAGS) $(OPT_FLAGS) \
                  $$OBJS -o $(LIB_DIR)/$(NAME).$(GET_VERSION)  $(LDFLAGS) \
                  -Wl,-soname,$(NAME).$(VER) ; \
		ln -fs $(NAME).$(GET_VERSION) $(LIB_DIR)/$(NAME).$(VER) ; \
		ln -fs $(NAME).$(GET_VERSION) $(LIB_DIR)/$(NAME) ; \
	fi
	$(POST_NAME)
install:
	install  $(LIB_DIR)/$(NAME).$(GET_VERSION) $(DOMINO_LIB)/
	rm -f $(DOMINO_LIB)/$(NAME).$(VER)
	ln -s $(DOMINO_LIB)/$(NAME).$(GET_VERSION) $(DOMINO_LIB)/$(NAME).$(VER)
	rm -f $(DOMINO_LIB)/$(NAME)
	ln -s $(DOMINO_LIB)/$(NAME).$(GET_VERSION) $(DOMINO_LIB)/$(NAME)
	$(INSTALL)
clean:
	rm -fR $(DIR_TMP)
	rm -f $(LIB_DIR)/$(NAME).$(GET_VERSION)
	rm -f $(LIB_DIR)/$(NAME).$(VER)
	rm -f $(LIB_DIR)/$(NAME)
	$(CLEAN)
uninstall:
	rm -f $(DOMINO_LIB)/$(NAME).$(VER)
	rm -f $(DOMINO_LIB)/$(NAME)
	rm -f $(DOMINO_LIB)/$(NAME).$(GET_VERSION)
