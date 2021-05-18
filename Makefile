CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
CP = aarch64-linux-gnu-objcopy
SRCS_C = $(shell find -name '*.c' ! -name 'main.c')
SRCS_S = $(shell find -name '*.s' ! -name 'start.s')
OBJS = $(SRCS_C:.c=.o) $(SRCS_S:.s=.o)

all: boot/kernel8.img initfs

%.o: %.c
	$(CC) -c -g -Wall -I . -DPRINTF_DISABLE_SUPPORT_FLOAT -ffreestanding -mgeneral-regs-only -o $@ $<

%.o: %.s
	$(CC) -c -g -Wall -o $@ $<

boot/kernel8.img: kernel/start.o boot/main.o $(OBJS)
	$(LD) -nostdlib -Ttext=0x80000 -o boot/kernel8.elf kernel/start.o boot/main.o $(OBJS)
	$(CP) -O binary boot/kernel8.elf boot/kernel8.img 

hello/hello: user/start.o hello/main.o $(OBJS)
	$(LD) -nostdlib -Ttext=0x80000 -o hello/hello.elf user/start.o hello/main.o $(OBJS)
	$(CP) -O binary hello/hello.elf hello/hello

initfs: hello/hello
	echo 'hello' | cpio -o -H newc -D 'hello' > initfs

run: boot/kernel8.img initfs
	qemu-system-aarch64 \
		-machine raspi3 \
		-kernel boot/kernel8.img \
		-initrd initfs \
		-display none \
		-serial null -serial stdio

clean:
	rm */*.o */*.elf boot/kernel8.img initfs `find -regex '\./[^./]+/[^./]+'`
