C_OBJS=cos_net.o
COMPONENT=tnet.o
INTERFACES=torrent
DEPENDENCIES=sched mem_mgr_large printc lock timed_blk evt net_internet net_portns valloc cbuf_c torrent
IF_LIB=./net_stack.o
FN_PREPEND=parent_

pre: first all
first:
	$(info |     [CP]   copying network stack from lwip)
	@cp $(LWIPOBJ) .

include ../../Makefile.subsubdir

