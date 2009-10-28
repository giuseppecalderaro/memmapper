EXTRA_CFLAGS += -I$(PWD)/include
memmapper-krn-objs := memmapper-main.o
obj-m := memmapper-krn.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	rm -rf *.o *.ko *~ *.symvers *.mod.c *.order *markers .memmapper* .tmp_*
