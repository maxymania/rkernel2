#
# Copyright (c) 2016 Simon Schmidt
#
# This software is provided 'as-is', without any express or implied
# warranty. Permission is granted to anyone to use this software for
# any purpose, including commercial applications, and to alter it and
# redistribute it freely.
#

require './kernbuild.rb'

require "./system/arch/i686new/toolchain.rb"

require "./system/arch/i686new/build.rb"
require "./system/platform/i686-pc/build.rb"

Compiler.incls "./include"
Compiler.incls "./system/include"

Makefile.open('Makefile')
Makefile.project("","KBuild-i686new/")

Makefile.rule "all",["kernel.i686new"],[]

Makefile.rule "qemu",["kernel.i686new"],["qemu-system-i386 -kernel kernel.i686new"]

#MKList.add "kernel", Makefile.glob("system/kern/*.c")

add_mod "kernel.i686new" , *(MKList.get ["archdep","kernel"] )

Makefile.close

