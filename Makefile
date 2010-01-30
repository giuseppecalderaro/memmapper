OBJS = 	src/memmapper.c	\
	src/io_func.c		\
	src/lib_func.c		\
	src/mem_func.c		\
	src/pci_func.c		\
	src/disasm_func.c

GCC=gcc
LDFLAGS=-lm
INCLUDE_DIR=include

all:
	@echo -n Compiling memmapper...
	@$(GCC) -I$(INCLUDE_DIR) $(LDFLAGS) $(OBJS) -o memmapper
	@echo DONE!

disassemble:
	@echo -n Compiling memmapper with DISASSEMBLER...
	@$(GCC) -DDISASM -I$(INCLUDE_DIR) $(LDFLAGS) $(OBJS) /usr/lib/libudis86.a -o memmapper
	@echo DONE!

debug:
	@echo -n Compiling memmapper DEBUG version...
	@$(GCC) -DDEBUG -I$(INCLUDE_DIR) $(LDFLAGS) $(OBJS) -ggdb -o memmapper
	@echo DONE!

ddebug:
	@echo -n Compiling memmapper DISASSEMBLER DEBUG version...
	@$(GCC) -DDEBUG -DDISASM -I$(INCLUDE_DIR) $(LDFLAGS) $(OBJS) -ggdb /usr/lib/libudis86.a -o memmapper
	@echo DONE!

clean:
	@echo -n Cleaning all...
	@rm -f memmapper
	@echo DONE!
