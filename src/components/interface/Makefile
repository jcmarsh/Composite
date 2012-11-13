include Makefile.src Makefile.comp

INTERFACES=$(wildcard */)

.PHONY: all
all:
	$(info ***********************************************)
	$(info ************[ Building interfaces ]************)
	$(info ***********************************************)
#	@for dir in $(INTERFACES) ; do \
#		$(MAKE) -C $$dir ; \
#	done
	$(MAKE) -C initf
	$(MAKE) -C stkmgr
	$(MAKE) -C sched
	$(MAKE) -C sched_hier
	$(MAKE) -C pgfault
	$(MAKE) -C cgraph

.PHONY: clean
clean:
	$(info |     [RM]   Cleaning up interface directories)
	@for dir in $(INTERFACES) ; do \
		$(MAKE) -C $$dir clean ; \
	done

.PHONY: cp
cp:

.PHONY: init
init: clean all
