include Makefile.src

obj-m := cos.o
cos-objs := hijack.o

default: module

module: 
	$(MAKE) -C $(LDIR) SUBDIRS=$(shell pwd) modules

cp:
	cp cos.ko $(TRANS_DIR)

#loc:
#	@ wc -l *.c *.S *.h include/*.h $(LINKER_DIR)/*.c $(LINKER_DIR)/t/*.S $(LINKER_DIR)/t/*.c $(LINKER_DIR)/t/*.h $(NET_DIR)/*.c $(NET_DIR)/util/*.c | tail -n 1 | awk '{print $1}'

clean:
	rm -rf *.ko *.mod.c *.o *.mod.o .*.cmd .tmp_versions test Module.symvers
	rm -rf $(KDIR)/*.o $(KDIR)/.*.o.cmd

fresh: clean default
