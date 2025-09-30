DIR_TMP=../../var/MSYS_NT-10.0/i686/$(NAME)
LIB_DIR=../../var/MSYS_NT-10.0/i686/.libs
all: $(NAME)
$(NAME):
	$(CC) -shared  -fPIC $(AM_FLAGS) $(CFLAGS) $(SRCS) -o $(NAME) -Wl,--out-implib,$(NAME).a $(LDFLAGS)
install:
	install  $(LIB_DIR)/$(NAME).$(GET_VERSION) $(DOMINO_LIB)/
	rm -f $(DOMINO_LIB)/$(NAME).$(VER)
	ln -s $(DOMINO_LIB)/$(NAME).$(GET_VERSION) $(DOMINO_LIB)/$(NAME).$(VER)
	rm -f $(DOMINO_LIB)/$(NAME)
	ln -s $(DOMINO_LIB)/$(NAME).$(GET_VERSION) $(DOMINO_LIB)/$(NAME)
	$(INSTALL)
clean:
	rm -fR $(DIR_TMP)
	rm -fR $(DIR_TMP1)
	rm -f $(LIB_DIR)/$(NAME).$(GET_VERSION)
	rm -f $(LIB_DIR)/$(NAME).$(VER)
	rm -f $(LIB_DIR)/$(NAME)
	$(CLEAN)
uninstall:
	rm -f $(DOMINO_LIB)/$(NAME).$(VER)
	rm -f $(DOMINO_LIB)/$(NAME)
	rm -f $(DOMINO_LIB)/$(NAME).$(GET_VERSION)
