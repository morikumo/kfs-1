NAME = kernel.bin

ASM = nasm
CC = gcc
LD = ld

ASMFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -fno-builtin -fno-stack-protector -nostdlib -nodefaultlibs
LDFLAGS = -m elf_i386 -T linker.ld

SRC_ASM = boot/boot.asm
SRC_C = kernel/kernel.c

OBJ_ASM = boot.o
OBJ_C = kernel.o

all: iso

$(OBJ_ASM):
	$(ASM) $(ASMFLAGS) $(SRC_ASM) -o $(OBJ_ASM)

$(OBJ_C):
	$(CC) $(CFLAGS) -c $(SRC_C) -o $(OBJ_C)

$(NAME): $(OBJ_ASM) $(OBJ_C)
	$(LD) $(LDFLAGS) $(OBJ_ASM) $(OBJ_C) -o $(NAME)

iso: $(NAME)
	mkdir -p iso/boot
	cp $(NAME) iso/boot/kernel.bin
	rm $(NAME)
	grub-mkrescue -o kfs.iso iso

clean:
	rm -f *.o kernel.bin
	rm -f iso/boot/kernel.bin

docker-iso:
	rm kfs.iso
	docker run --rm --platform linux/amd64 -v $(PWD):/kfs -w /kfs debian:latest bash -c \
		"apt-get update -qq && apt-get install -y -qq nasm gcc make grub-pc-bin grub-common xorriso mtools >/dev/null 2>&1 && make"

re: clean all
