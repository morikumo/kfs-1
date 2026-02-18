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
	grub-mkrescue -o kfs.iso iso

clean:
	rm -f *.o kernel.bin kfs.iso

re: clean all
