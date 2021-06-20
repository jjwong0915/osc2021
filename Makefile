CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
CP = aarch64-linux-gnu-objcopy
SRCS_C = $(shell find -name '*.c' ! -path './user/*' ! -name 'syscall.c')
SRCS_S = $(shell find -name '*.s' ! -path './user/*')
OBJS = $(SRCS_C:.c=.o) $(SRCS_S:.s=.o)
CFLAGS = \
	-I . \
	-DPRINTF_DISABLE_SUPPORT_FLOAT \
	-ffreestanding \
	-mgeneral-regs-only \
	-ffixed-x19 \
	-ffixed-x20 \
	-ffixed-x21 \
	-ffixed-x22 \
	-ffixed-x23 \
	-ffixed-x24 \
	-ffixed-x25 \
	-ffixed-x26 \
	-ffixed-x27 \
	-ffixed-x28

all: kernel/kernel8.img initfs

%.o: %.c
	$(CC) -c -g -Wall $(CFLAGS) -o $@ $<

%.o: %.s
	$(CC) -c -g -Wall -o $@ $<

kernel/kernel8.img: $(OBJS)
	$(LD) -nostdlib -Ttext=0x80000 -o kernel/kernel8.elf $(OBJS)
	$(CP) -O binary kernel/kernel8.elf kernel/kernel8.img 

user/argv_test: user/start.o user/argv_test.o printf/printf.o syscall/syscall.o
	$(LD) -nostdlib -Ttext=0x80000 -o user/argv_test.elf user/start.o user/argv_test.o printf/printf.o syscall/syscall.o
	$(CP) -O binary user/argv_test.elf user/argv_test

user/fork_test: user/start.o user/fork_test.o printf/printf.o syscall/syscall.o
	$(LD) -nostdlib -Ttext=0x80000 -o user/fork_test.elf user/start.o user/fork_test.o printf/printf.o syscall/syscall.o
	$(CP) -O binary user/fork_test.elf user/fork_test

initfs: user/argv_test user/fork_test
	echo 'argv_test\nfork_test' | cpio -o -H newc -D 'user' > initfs

run: all
	qemu-system-aarch64 \
		-machine raspi3 \
		-kernel kernel/kernel8.img \
		-initrd initfs \
		-display none \
		-serial null -serial stdio

debug: all
	qemu-system-aarch64 \
		-machine raspi3 \
		-kernel kernel/kernel8.img \
		-initrd initfs \
		-display none \
		-serial null -serial stdio \
		-s -S

attach:
	gdb-multiarch kernel/kernel8.elf -ex 'target remote :1234'

clean:
	rm */*.o */*.elf kernel/kernel8.img initfs `find -regex '\./[^./]+/[^./]+'`
