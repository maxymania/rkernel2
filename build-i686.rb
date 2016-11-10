#
# Copyright (c) 2016 Simon Schmidt
#
# This software is provided 'as-is', without any express or implied
# warranty. Permission is granted to anyone to use this software for
# any purpose, including commercial applications, and to alter it and
# redistribute it freely.
#

require './kernbuild.rb'

require "./system/arch/i686/toolchain.rb"

require "./system/arch/i686/build.rb"
require "./system/platform/i686-pc/build.rb"

Compiler.incls "./include"
Compiler.incls "./system/include"

Makefile.open('Makefile')
Makefile.project("","KBuild-i686/")

Makefile.rule "all",["kernel.i686"],[]

MKList.add "kernel", Makefile.glob("system/kern/*.c")
MKList.add "kernel", Makefile.glob("system/terminal/*.c")


add_mod "kernel.i686" , *(MKList.get ["archdep","kernel"] )

Makefile.close

