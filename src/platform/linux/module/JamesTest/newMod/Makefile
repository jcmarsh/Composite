CODE_DIR=/home/jcmarsh/research
TOP_DIR=$(CODE_DIR)/composite/src/
LDIR=$(CODE_DIR)/linux-2.6.36/

obj-m := base.o
base-objs := kern_entry64.o ipc64.o enter.o

all:
	make -C $(LDIR) SUBDIRS=$(shell pwd) modules

clean:
	make -C /lib/modules/2.6.36/build M=$(PWD) clean
