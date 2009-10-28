
OBJS = 	src/memmapper.c	\
	src/io_func.c		\
	src/lib_func.c		\
	src/mem_func.c		\
	src/pci_func.c

GCC=gcc
LDFLAGS=-lm

all:
	@echo -n Compiling memmapper...
	@$(GCC) $(LDFLAGS) $(OBJS) -o memmapper
	@echo DONE!

disassemble:
	@echo -n Compiling memmapper with DISASSEMBLER...
	@$(GCC) -DDISASM $(LDFLAGS) $(OBJS) /usr/lib/libudis86.a -o memmapper
	@echo DONE!

debug:
	@echo -n Compiling memmapper DEBUG version...
	@$(GCC) -DDEBUG $(LDFLAGS) $(OBJS) -o memmapper
	@echo DONE!

clean:
	@echo -n Cleaning all...
	@rm -f memmapper
	@echo DONE!
