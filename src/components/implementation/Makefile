include Makefile.src Makefile.comp
SUBDIRS=$(wildcard */)
MAKEFLAGS=-I$(shell pwd)

.PHONY: all
all: 
	$(info ***********************************************)
	$(info *********[ Building Implementations ]**********)
	$(info ***********************************************)
#	@for dir in $(SUBDIRS) ; do \
#		$(MAKE) $(MAKEFLAGS) -C $$dir ; \
# 	done
	$(MAKE) $(MAKEFLAGS) -C no_interface
	$(MAKE) $(MAKEFLAGS) -C initf
#	$(MAKE) $(MAKEFLAGS) -C stkmgr
#	$(MAKE) $(MAKEFLAGS) -C sched
#	$(MAKE) $(MAKEFLAGS) -C sched_hier // Should be in sched -jcm
#	$(MAKE) $(MAKEFLAGS) -C pgfault
#	$(MAKE) $(MAKEFLAGS) -C cgraph // Can't find -jcm

.PHONY: cp
cp:
	$(info |     [CP]   Copying all components to $(TRANS_DIR))
#	@for dir in $(SUBDIRS) ; do \
#		$(MAKE) $(MAKEFLAGS) -C $$dir cp ; \
#	done
	$(MAKE) $(MAKEFLAGS) -C no_interface cp
	$(MAKE) $(MAKEFLAGS) -C initf cp
#	$(MAKE) $(MAKEFLAGS) -C stkmgr cp
#	$(MAKE) $(MAKEFLAGS) -C sched cp
#	$(MAKE) $(MAKEFLAGS) -C sched_hier cp
#	$(MAKE) $(MAKEFLAGS) -C pgfault cp
#	$(MAKE) $(MAKEFLAGS) -C cgraph cp


.PHONY: clean
clean:
	$(info |     [RM]   Cleaning up implementation directories)
#	@for dir in $(SUBDIRS) ; do \
#		$(MAKE) $(MAKEFLAGS) -C $$dir clean ; \
#	done
	$(MAKE) $(MAKEFLAGS) -C no_interface clean
	$(MAKE) $(MAKEFLAGS) -C initf clean
#	$(MAKE) $(MAKEFLAGS) -C stkmgr clean
#	$(MAKE) $(MAKEFLAGS) -C sched clean
#	$(MAKE) $(MAKEFLAGS) -C sched_hier clean
#	$(MAKE) $(MAKEFLAGS) -C pgfault clean
#	$(MAKE) $(MAKEFLAGS) -C cgraph clean

.PHONY: init
init: clean all
