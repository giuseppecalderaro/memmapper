GCC=gcc
LDFLAGS=-lm
INCLUDE_DIR=include
OBJS= 				\
	memmapper.o 		\
	io_func.o 		\
	lib_func.o		\
	mem_func.o		\
	pci_func.o		\
	disasm_func.o

ifeq ($(DISASM),y)
DISASM_DEFS=-DDISASM
DISASM_LIBS=/usr/lib/libudis86.a
else
DISASM_DEFS=
DISASM_LIBS=
endif

ifeq ($(DEBUG),y)
DEBUG_DEFS=-DDEBUG
else
DEBUG_DEFS=
endif

all: clean $(OBJS)
	@echo -n Compiling memmapper...
	@$(GCC) $(LDFLAGS) $(OBJS) $(DISASM_LIBS) -o memmapper
	@-rm -f *.o
	@echo DONE!

.PHONY: clean
clean:
	@echo -n Cleaning all...
	@rm -f memmapper *.o
	@echo DONE!

# Rule
%.o: src/%.c
	@$(GCC) $(DEBUG_DEFS) $(DISASM_DEFS) -c -I$(INCLUDE_DIR) $< -o $@
