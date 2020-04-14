SHELL = /bin/sh
ARM = aarch64-linux-gnu
CC = $(ARM)-gcc
LD = $(ARM)-ld
IDIR = inc
SDIR = src
BDIR = build
CFLAGS = -Wall -I $(IDIR) -O0
OBJCOPY = $(ARM)-objcopy
S_SRCS = $(wildcard $(SDIR)/*.S)
C_SRCS = $(wildcard $(SDIR)/*.c)
S_OBJS = $(S_SRCS:$(SDIR)/%.S=$(BDIR)/%.o)
C_OBJS = $(C_SRCS:$(SDIR)/%.c=$(BDIR)/%.o)

all: clean kernel8.img

kernel8.img: kernel8.elf
	$(OBJCOPY) -O binary kernel8.elf kernel8.img

kernel8.elf: $(S_OBJS) linker.ld $(C_OBJS)
	$(LD) -T linker.ld -o kernel8.elf $(S_OBJS) $(C_OBJS)

$(C_OBJS): $(C_SRCS)
	$(CC) $(CFLAGS) -c $< -o $@

$(S_OBJS): $(S_SRCS)
	$(CC) -c $< -o $@

clean:
	rm -f $(BDIR)/*.o kernel8.elf kernel8.img

run: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial mon:stdio

tty: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial "pty"

debug: all
	terminator -e "qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial mon:stdio -s -S" --new-tab
	terminator -e "aarch64-linux-gnu-gdb -x debug.txt" --new-tab
