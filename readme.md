# KFS_1 – Grub, boot and screen

## 1. Introduction

Ce projet correspond au premier module de **Kernel From Scratch (KFS_1)**. L’objectif est de créer un noyau minimal, entièrement indépendant de tout système hôte, capable de démarrer via **GRUB** et d’afficher le texte **"42"** à l’écran.

Ce projet constitue une introduction concrète au développement noyau. Il ne s’agit pas de développer un système d’exploitation complet, mais de comprendre et maîtriser la chaîne minimale suivante :

* chargement du noyau par un bootloader,
* initialisation basique du processeur,
* exécution de code kernel sans bibliothèque standard,
* accès direct au matériel (ici l’écran via la mémoire VGA).

Aucun bonus n’est implémenté. Le projet respecte strictement les exigences de la partie obligatoire.

---

## 2. Objectifs du projet

À la fin de ce projet, le noyau doit :

* être **bootable via GRUB** (multiboot),
* fonctionner sur une architecture **i386 (x86 32 bits)**,
* être compilé **sans libc ni bibliothèques système**,
* utiliser un **linker script personnalisé**,
* afficher **uniquement "42"** à l’écran,
* produire une image bootable de taille inférieure à **10 MB**.

---

## 3. Arborescence du projet

```
KFS_1/
├── Makefile
├── linker.ld
├── boot/
│   └── boot.asm
├── kernel/
│   └── kernel.c
└── iso/
    └── boot/
        └── grub/
            └── grub.cfg
```

Chaque fichier a un rôle précis, décrit ci-dessous.

---

## 4. Chaîne de démarrage

### 4.1 GRUB

GRUB agit comme bootloader. Il est responsable de :

* passer le processeur en mode protégé 32 bits,
* charger le noyau en mémoire,
* fournir un environnement minimal conforme à la spécification **Multiboot**,
* transférer l’exécution au point d’entrée du noyau.

Le noyau est chargé grâce à un **header multiboot**, ce qui évite d’avoir à écrire un bootloader complet.

---

### 4.2 Configuration GRUB

Fichier : `iso/boot/grub/grub.cfg`

Ce fichier définit une entrée GRUB minimale :

* aucun timeout,
* un seul menu,
* chargement du noyau via la directive `multiboot`.

GRUB affiche brièvement un message de démarrage, puis passe la main au noyau.

---

## 5. Code ASM – boot.asm

Fichier : `boot/boot.asm`

Ce fichier contient :

* le **header multiboot**, requis par GRUB,
* le point d’entrée réel du noyau (`start`),
* l’initialisation de la pile,
* l’appel de la fonction C `kernel_main`.

Points importants :

* architecture i386 (32 bits),
* aucune interruption active (`cli`),
* pile allouée manuellement,
* aucun appel BIOS ou système.

Ce fichier constitue le lien entre GRUB et le code C du noyau.

---

## 6. Code C – kernel.c

Fichier : `kernel/kernel.c`

Le noyau est écrit en C minimal, sans aucun `#include`.

Fonctionnalités :

* nettoyage complet de l’écran VGA,
* écriture directe du texte "42" dans la mémoire vidéo,
* boucle infinie avec instruction `hlt`.

### 6.1 Accès à l’écran

L’affichage se fait via la mémoire VGA text mode située à l’adresse :

```
0xB8000
```

Chaque caractère occupe 2 octets :

* 1 octet pour le caractère ASCII,
* 1 octet pour la couleur (attribut).

Le noyau écrit directement dans cette zone mémoire, sans intermédiaire.

### 6.2 Nettoyage de l’écran

GRUB ayant déjà écrit dans la mémoire VGA, le noyau efface explicitement les 80×25 caractères avant d’afficher "42". Cela garantit un écran propre et contrôlé uniquement par le noyau.

---

## 7. Linker script

Fichier : `linker.ld`

Un linker script personnalisé est obligatoire afin d’éviter toute dépendance au système hôte.

Rôles du linker script :

* définir le point d’entrée (`start`),
* positionner le noyau à l’adresse **1 MB** en mémoire,
* organiser les sections (`.text`, `.data`, `.bss`).

L’utilisation de `ld` est autorisée, mais aucun script système n’est utilisé.

---

## 8. Makefile

Le `Makefile` orchestre l’ensemble de la compilation :

* assemblage du code ASM avec **nasm**,
* compilation du C avec **gcc** en mode freestanding,
* édition de liens avec **ld** et le linker script personnalisé,
* création d’une image ISO bootable avec **grub-mkrescue**.

### 8.1 Flags importants

Les flags suivants garantissent l’absence de dépendances :

* `-ffreestanding`
* `-nostdlib`
* `-nodefaultlibs`
* `-fno-builtin`
* `-fno-stack-protector`

Ils sont indispensables pour un noyau fonctionnel.

---

## 9. Compilation et exécution

### 9.1 Compilation

```
make
```

Cela produit :

* `kernel.bin` : le noyau,
* `kfs.iso` : image bootable GRUB.

### 9.2 Exécution (exemple avec QEMU)

```
qemu-system-i386 -cdrom kfs.iso
```

Résultat attendu :

* écran noir,
* affichage unique de "42" en haut à gauche,
* système figé (boucle `hlt`).

---

## 10. Conformité au sujet

Ce projet respecte strictement toutes les contraintes :

* architecture i386,
* GRUB + multiboot,
* aucun bonus,
* aucune bibliothèque externe,
* taille inférieure à 10 MB,
* affichage correct de "42".

Il constitue une base saine et minimale pour les projets KFS suivants.

---

## 11. Conclusion

KFS_1 pose les fondations essentielles du développement noyau :

* compréhension du boot,
* contrôle total de l’exécution,
* interaction directe avec le matériel.

Ce noyau minimal servira de socle pour l’ajout progressif de fonctionnalités dans les prochains modules de Kernel From Scratch.
