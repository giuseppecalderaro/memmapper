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

.PHONY: clean
clean:
	@echo -n Cleaning all...
	@rm -f memmapper *.o
	@echo DONE!

# Rule
$(OBJS): %.o: src/%.c
	$(GCC) -c -I$(INCLUDE_DIR) $(LDFLAGS) $< -o $@
