#
# Copyright (c) 2016 Simon Schmidt
#
# This software is provided 'as-is', without any express or implied
# warranty. Permission is granted to anyone to use this software for
# any purpose, including commercial applications, and to alter it and
# redistribute it freely.
#

Compiler.incls "system/arch/include"

MKList.add "archdep", [
	"system/arch/i686/boot.s",
	"system/arch/i686/crti.s",
	"system/arch/i686/crtn.s"
]


