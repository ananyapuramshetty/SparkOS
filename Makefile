AS=nasm
CC=gcc
LD=ld

CFLAGS=-m32 -ffreestanding -fno-pie -fno-stack-protector -nostdlib
LDFLAGS=-m elf_i386 -T linker.ld

all: sparkos.iso

boot.o: src/boot.asm
	$(AS) -f elf32 src/boot.asm -o boot.o

kernel.o: src/kernel.c
	$(CC) $(CFLAGS) -c src/kernel.c -o kernel.o

keyboard.o: src/keyboard.c
	$(CC) $(CFLAGS) -c src/keyboard.c -o keyboard.o

shell.o: src/shell.c
	$(CC) $(CFLAGS) -c src/shell.c -o shell.o

process.o: src/process/process.c src/process/process.h
	$(CC) $(CFLAGS) -c src/process/process.c -o process.o

scheduler.o: src/process/scheduler.c src/process/scheduler.h src/process/process.h
	$(CC) $(CFLAGS) -c src/process/scheduler.c -o scheduler.o

sparkos.bin: boot.o kernel.o keyboard.o shell.o process.o scheduler.o
	$(LD) $(LDFLAGS) -o sparkos.bin boot.o kernel.o keyboard.o shell.o process.o scheduler.o

sparkos.iso: sparkos.bin
	mkdir -p iso/boot
	cp sparkos.bin iso/boot/sparkos.bin
	grub-mkrescue -o sparkos.iso iso

run: sparkos.iso
	qemu-system-i386 -cdrom sparkos.iso

clean:
	rm -f *.o sparkos.bin sparkos.iso
	rm -f iso/boot/sparkos.bin
