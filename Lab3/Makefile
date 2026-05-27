CROSS = riscv64-unknown-elf-
CC = $(CROSS)gcc

CFLAGS = -march=rv64imac_zicsr_zifencei -mabi=lp64 -nostdlib -O2 \
         -ffunction-sections -fdata-sections -DMAX_SOFT_TIMERS=8 \
         -mcmodel=medany -fno-pie
LDFLAGS = -T linker.ld -Wl,--gc-sections

OBJS = start.o sw_timer.o main.o

.PHONY: all clean

all: lab3.elf

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

lab3.elf: $(OBJS) linker.ld
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -f *.o lab3.elf