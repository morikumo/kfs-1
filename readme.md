# KFS_1 - Mon premier kernel

## C'est quoi ce projet ?

On a code un **kernel** (noyau). C'est le tout premier programme qui se lance quand un ordi demarre - avant meme Windows, Linux, ou quoi que ce soit.

Le but : allumer un PC (virtuel), et afficher **"42"** a l'ecran. C'est tout. Mais le truc, c'est qu'on a **aucune aide**. Pas de `printf`, pas de `#include`, pas de libc, pas de systeme d'exploitation. Rien. On parle directement au processeur et a l'ecran.

---

## Mais attends, pourquoi pas de printf ?

`printf` vient de la librairie C standard (`libc`). Cette librairie existe **grace au systeme d'exploitation** (Linux, macOS...). Or, nous, on EST le systeme d'exploitation. Il n'y a personne en dessous de nous pour nous fournir `printf`.

C'est comme si tu devais construire une maison, mais t'as meme pas le sol. Faut d'abord couler les fondations.

---

## Comment ca marche, etape par etape

Quand tu lances le projet avec QEMU (le PC virtuel), voici ce qui se passe :

```
1. Le BIOS demarre (c'est le firmware de base du PC)
2. Le BIOS trouve notre CD virtuel (le fichier .iso)
3. GRUB se lance (c'est le bootloader, on l'a pas code lui)
4. GRUB lit notre kernel, verifie le "mot de passe multiboot"
5. GRUB passe le CPU en mode 32 bits et lance notre code ASM
6. Notre code ASM prepare la pile et appelle kernel_main()
7. kernel_main() efface l'ecran et ecrit "42"
8. Le CPU se met en pause. Fin.
```

---

## Les fichiers du projet

```
kfs-1/
├── boot/
│   └── boot.asm        <- Le tout premier code qui s'execute
├── kernel/
│   └── kernel.c        <- Le coeur du kernel (affiche "42")
├── iso/
│   └── boot/
│       └── grub/
│           └── grub.cfg   <- La config de GRUB
├── linker.ld           <- Dit au linker comment assembler le tout
└── Makefile            <- Compile tout avec une seule commande
```

---

## boot.asm - Le demarreur

C'est de l'assembleur (ASM). C'est le langage le plus proche du processeur. On l'utilise parce que certains trucs ne sont **pas faisables en C**.

### Le header multiboot

```asm
dd 0x1BADB002    ; Le "mot de passe" que GRUB cherche
dd 0x00          ; Des options (on met rien)
dd -(0x1BADB002) ; Un checksum pour verifier que c'est correct
```

GRUB scanne notre fichier binaire a la recherche du nombre magique `0x1BADB002`. S'il le trouve et que le checksum est bon, il accepte de lancer notre kernel. Sinon, il refuse. C'est comme un videur de boite qui verifie ton bracelet.

### Le point d'entree

```asm
start:
    cli                  ; Desactive les interruptions (on veut pas etre derange)
    mov esp, stack_top   ; Place le pointeur de pile tout en haut de notre pile
    call kernel_main     ; Appelle notre fonction C
.hang:
    hlt                  ; Met le CPU en pause
    jmp .hang            ; Si jamais il se reveille, re-pause
```

**`cli`** = "shut up tout le monde, je demarre". Ca empeche le clavier, la souris, etc. d'interrompre le CPU pendant l'init.

**`mov esp, stack_top`** = On donne au CPU un espace memoire pour sa pile (la pile c'est ce qui permet d'appeler des fonctions, stocker des variables locales, etc.). Sans ca, `call kernel_main` crasherait immediatement.

**`hlt`** = Le CPU se met en veille. On a fini, plus rien a faire.

### La pile

```asm
stack_bottom:
    resb 16384       ; Reserve 16 KB de memoire vide
stack_top:
```

On reserve 16 KB pour la pile. C'est petit (un programme normal a 1-8 MB de pile), mais pour notre kernel qui fait presque rien, c'est largement suffisant.

**Pourquoi `stack_top` est en bas dans le code mais en "haut" de la pile ?** Parce que la pile x86 grandit **vers le bas** en memoire. Le CPU commence en haut et descend. C'est un choix de design d'Intel depuis les annees 70.

---

## kernel.c - Le cerveau

C'est ici qu'on ecrit le vrai code du kernel, en C.

### Pas de #include

```c
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
```

Comme on n'a pas de librairie standard, on definit nos propres types. `uint16_t` c'est juste un entier de 16 bits (2 octets), `uint8_t` c'est 8 bits (1 octet).

### L'ecran VGA

```c
static uint16_t* const VGA_MEMORY = (uint16_t*)0xB8000;
```

**C'est la ligne la plus importante du projet.**

L'ecran en mode texte a une zone de memoire dediee qui commence a l'adresse `0xB8000`. Si tu ecris un octet a cette adresse, ca apparait **directement a l'ecran**. Pas besoin de driver, pas besoin d'API. C'est cable en dur dans le hardware depuis les premiers IBM PC en 1981.

L'ecran fait 80 colonnes x 25 lignes = 2000 caracteres. Chaque caractere prend **2 octets** :

```
[ octet 1 : le caractere ASCII ] [ octet 2 : la couleur ]
```

Par exemple, pour ecrire un 'A' blanc sur fond noir a la position 0 :
```
Adresse 0xB8000 = 'A' (0x41)
Adresse 0xB8001 = 0x0F (blanc sur noir)
```

Dans notre code, on combine les deux en un seul `uint16_t` :
```c
VGA_MEMORY[0] = (0x0F << 8) | '4';   // '4' en blanc
VGA_MEMORY[1] = (0x0F << 8) | '2';   // '2' en blanc
```

### clear_screen()

```c
void clear_screen(void)
{
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        VGA_MEMORY[i] = ((uint16_t)VGA_COLOR << 8) | ' ';
    cursor_pos = 0;
}
```

GRUB laisse des trucs ecrits a l'ecran avant de nous passer la main. Donc on remplit tout l'ecran avec des espaces blancs pour "effacer" ce qu'il y avait avant.

### printstr()

```c
void printstr(const char *str)
{
    while (*str && cursor_pos < VGA_WIDTH * VGA_HEIGHT)
    {
        VGA_MEMORY[cursor_pos] = ((uint16_t)VGA_COLOR << 8) | *str;
        cursor_pos++;
        str++;
    }
}
```

Notre mini-printf a nous. Ca ecrit une string caractere par caractere dans la memoire VGA. Le `cursor_pos < 2000` empeche d'ecrire en dehors de l'ecran (ce qui corromprait la memoire).

### kernel_main()

```c
void kernel_main(void)
{
    clear_screen();
    printstr("42");
    while (1)
        __asm__("hlt");
}
```

C'est la. Tout le kernel tient la-dedans : on efface l'ecran, on ecrit "42", et on dort pour toujours.

---

## linker.ld - Le plan d'assemblage

```
ENTRY(start)         <- Le programme commence a "start" (dans boot.asm)

SECTIONS
{
    . = 1M;          <- On place notre kernel a 1 Mo en memoire

    .text : { ... }  <- Le code executable
    .rodata : { ... } <- Les donnees en lecture seule (strings, etc.)
    .data : { ... }  <- Les variables initialisees
    .bss : { ... }   <- Les variables non-initialisees (mises a 0)
}
```

**Pourquoi 1 Mo ?** Les premiers 640 Ko de la RAM sont reserves au BIOS, a la memoire video, etc. De 640 Ko a 1 Mo c'est une zone "no man's land". Donc le premier endroit safe pour mettre notre kernel, c'est a partir de 1 Mo. C'est une convention standard en OS dev.

**Pourquoi un linker script custom ?** Le linker par defaut de ton PC est configure pour Linux/macOS. Il ajouterait des dependances au systeme hote. Notre script dit au linker exactement ou placer chaque morceau du kernel, sans rien supposer sur l'environnement.

---

## Makefile - La recette

```bash
make        # Compile tout et genere kfs.iso
make clean  # Supprime les fichiers generes
make re     # Clean + recompile
```

Le Makefile fait 4 choses dans l'ordre :
1. **nasm** compile `boot.asm` en `boot.o` (code machine 32 bits)
2. **gcc** compile `kernel.c` en `kernel.o` (avec les flags qui virent toute dependance)
3. **ld** lie `boot.o` + `kernel.o` en `kernel.bin` (en suivant `linker.ld`)
4. **grub-mkrescue** emballe `kernel.bin` + GRUB dans un fichier `kfs.iso` bootable

### Les flags de compilation

| Flag | Ca fait quoi |
|---|---|
| `-m32` | Compile en 32 bits (notre kernel est i386) |
| `-ffreestanding` | Dit a GCC "y'a pas de systeme en dessous" |
| `-fno-builtin` | Empeche GCC de remplacer notre code par des appels libc |
| `-fno-stack-protector` | Desactive la protection de pile (on a pas l'infra pour) |
| `-nostdlib` | Ne lie pas la librairie C standard |
| `-nodefaultlibs` | Ne lie aucune librairie par defaut |

---

## Lancer le projet

```bash
# Compiler
make

# Lancer avec QEMU (emulateur de PC)
qemu-system-i386 -cdrom kfs.iso
```

Tu devrais voir un ecran noir avec **42** en haut a gauche, en blanc. C'est tout. C'est notre kernel.

---

## Resume visuel

```
  ┌─────────────────────────────────────────┐
  │               BIOS                      │
  │  "Je demarre, je cherche un CD..."      │
  └──────────────────┬──────────────────────┘
                     v
  ┌─────────────────────────────────────────┐
  │               GRUB                      │
  │  "J'ai trouve kernel.bin, je le charge" │
  │  "Header multiboot OK, je lance !"      │
  └──────────────────┬──────────────────────┘
                     v
  ┌─────────────────────────────────────────┐
  │           boot.asm (start)              │
  │  cli -> setup pile -> call kernel_main  │
  └──────────────────┬──────────────────────┘
                     v
  ┌─────────────────────────────────────────┐
  │         kernel.c (kernel_main)          │
  │  clear_screen() -> printstr("42")       │
  │  -> hlt (dodo)                          │
  └─────────────────────────────────────────┘
```
