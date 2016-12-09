#
# Copyright (c) 2016 Simon Schmidt
#
# This software is provided 'as-is', without any express or implied
# warranty. Permission is granted to anyone to use this software for
# any purpose, including commercial applications, and to alter it and
# redistribute it freely.
#

MKList.add "kernel", Makefile.glob("system/kern/*.c")
MKList.add "kernel", Makefile.glob("system/physmem/*.c")
MKList.add "kernel", Makefile.glob("system/vm/*.c")
MKList.add "kernel", Makefile.glob("system/xcpu/*.c")
MKList.add "kernel", Makefile.glob("system/libkern/*.c")
MKList.add "kernel", Makefile.glob("system/terminal/*.c")
MKList.add "kernel", Makefile.glob("system/iopipe/*.c")

