GCC=gcc
LDFLAGS=-lm
INCLUDE_DIR=include
OBJS= 				\
	memmapper.o 		\
	io_func.o 		\
	lib_func.o		\
	mem_func.o		\
	pci_func.o

all: $(OBJS)
	@echo -n Compiling memmapper...
	@$(GCC) -I$(INCLUDE_DIR) $(LDFLAGS) -o memmapper $(OBJS) 
	@-rm -f *.o 
	@echo DONE!

disassemble: $(OBJS)
	@echo -n Compiling memmapper with DISASSEMBLER...
	@$(GCC) -DDISASM -I$(INCLUDE_DIR) $(LDFLAGS) $(OBJS) /usr/lib/libudis86.a -o memmapper
	@-rm -f *.o
	@echo DONE!

debug: $(OBJS)
	@echo -n Compiling memmapper DEBUG version...
	@$(GCC) -DDEBUG -I$(INCLUDE_DIR) $(LDFLAGS) $(OBJS) -o memmapper
	@-rm -f *.o
	@echo DONE!


.PHONY: clean
clean:
	@echo -n Cleaning all...
	@rm -f memmapper *.o
	@echo DONE!

# Rule
%.o: src/%.c
	@$(GCC) -c -I$(INCLUDE_DIR) $< -o $@
