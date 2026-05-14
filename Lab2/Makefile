PROG_NAME   = test
RISCV64_GCC = riscv64-elf-gcc
GCC_FLAGS   = -march=rv64g -mabi=lp64 -static -mcmodel=medany \
              -fvisibility=hidden -fno-builtin \
              -nostdlib -nostartfiles -g

LD_FLAGS = -T linker.ld

all: $(PROG_NAME)

$(PROG_NAME): start.o test.o
	$(RISCV64_GCC) $(GCC_FLAGS) start.o test.o $(LD_FLAGS) -o $@

start.o: start.S
	$(RISCV64_GCC) $(GCC_FLAGS) -c start.S -o start.o

test.o: test.c
	$(RISCV64_GCC) $(GCC_FLAGS) -c test.c -o test.o

clean:
	rm -f *.o $(PROG_NAME)
