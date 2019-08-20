include common.mak
all:
	$(MAKE) $(AM_MAKEFLAGS) all-recursive

install:
	$(MAKE) $(AM_MAKEFLAGS) all-recursive install-recursive

clean:
	$(MAKE) $(AM_MAKEFLAGS) clean-recursive
	
uninstall: 
	$(MAKE) $(AM_MAKEFLAGS) uninstall-recursive

RECURSIVE_TARGETS = all-recursive install-recursive clean-recursive uninstall-recursive

$(RECURSIVE_TARGETS):
	@failcom='exit 1'; \
	target=`echo $@ | sed s/-recursive//`; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	echo "Making $$target in $$subdir"; \
	local_target="$$target"; \
	(cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$local_target) \
	|| eval $$failcom; \
	done; \
	test -z "$$fail"


