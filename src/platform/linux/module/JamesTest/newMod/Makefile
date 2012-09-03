obj-m += base.o
obj-m += hello.o

all:
	make -C /lib/modules/2.6.36/build M=$(PWD) modules

clean:
	make -C /lib/modules/2.6.36/build M=$(PWD) clean
