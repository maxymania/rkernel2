#
# Copyright (c) 2016 Simon Schmidt
#
# This software is provided 'as-is', without any express or implied
# warranty. Permission is granted to anyone to use this software for
# any purpose, including commercial applications, and to alter it and
# redistribute it freely.
#

Compiler.incls "./system/arch/i686/include"

MKList.add "archdep", [
	"system/arch/i686/boot.s",
	"system/arch/i686/crti.s",
	"system/arch/i686/crtn.s",
	"system/arch/i686/interrupt.s",
	"system/arch/i686/intvec.s",
	"system/arch/i686/switch.s"
]

MKList.add "archdep", Makefile.glob("system/arch/i686/*.c")

