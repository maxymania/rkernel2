#
# Copyright (c) 2016 Simon Schmidt
#
# This software is provided 'as-is', without any express or implied
# warranty. Permission is granted to anyone to use this software for
# any purpose, including commercial applications, and to alter it and
# redistribute it freely.
#

Compiler.add "c"   , cc:"i686-elf-gcc" , cflags:"-std=gnu99 -ffreestanding -O2 -Wall -Wextra"
Compiler.add "c++" , cc:"i686-elf-g++" , cflags:"-std=gnu99 -ffreestanding -O2 -Wall -Wextra"
Compiler.add "s"   , cc:"i686-elf-as"  , cflags:""
Linker.add ld:"i686-elf-gcc -T system/arch/i686new/linker.ld", ldflags:"-ffreestanding -O2 -nostdlib", ldlibs:"-lgcc"
Compiler.cdef "__ARCH_EL__=1"


