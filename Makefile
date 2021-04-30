CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
CP = aarch64-linux-gnu-objcopy
SRCS = $(shell find -name '*.c' ! -name 'main.c')
OBJS = $(SRCS:.c=.o)

all: boot/kernel8.img initramfs

%.o: %.c
	$(CC) -c -g -Wall -I . -DPRINTF_DISABLE_SUPPORT_FLOAT -ffreestanding -mgeneral-regs-only -o $@ $<

%.o: %.s
	$(CC) -c -g -Wall -o $@ $<

boot/kernel8.img: kernel/start.o boot/main.o $(OBJS)
	$(LD) -nostdlib -Ttext=0x80000 -o boot/kernel8.elf kernel/start.o boot/main.o $(OBJS)
	$(CP) -O binary boot/kernel8.elf boot/kernel8.img 

hello/hello: hello/main.o $(OBJS)
	$(LD) -nostdlib -e main -o hello/hello.elf hello/main.o $(OBJS)
	$(CP) -O binary hello/hello.elf hello/hello

initramfs: hello/hello
	echo 'hello' | cpio -o -H newc -D 'hello' > initramfs

run: boot/kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel boot/kernel8.img -display none -serial null -serial stdio

clean:
	rm */*.o */*.elf boot/kernel8.img initramfs `find -regex '\./[^./]+/[^./]+'`
