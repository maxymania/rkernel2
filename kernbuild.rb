#
# Copyright (c) 2016 Simon Schmidt
#
# This software is provided 'as-is', without any express or implied
# warranty. Permission is granted to anyone to use this software for
# any purpose, including commercial applications, and to alter it and
# redistribute it freely.
#

######################################################################
#
# This software is Kind-of a minimal CMAKE-workalike, but optimized
# for Bare-Metal / Kernel developement.
#
######################################################################

module Language
	EXT = {"cc"=>"c++", "asm"=>"s"}
	def Language.reduce(name)
		ext = (name.downcase.match(/\.([a-z0-9_]+)/) or [nil,"txt"])[1]
		while Language::EXT[ext] do ext = Language::EXT[ext] end
		ext
	end
end

module Compiler
	FLAGS = [""]
	STEPX = {
		"c"   => ["%{cc} -c %%{src} -o %%{dest} %{cflags} %%{flags}"],
		"c++" => ["%{cc} -c %%{src} -o %%{dest} %{cflags} %%{flags}"],
		"s"   => ["%{cc}    %%{src} -o %%{dest} %{cflags}"]
	}
	STEP = {}
	def Compiler.add(l,conf)
		if STEPX[l]
			STEP[l] = STEPX[l].map{|a| a % conf}
		end
	end
	def Compiler.flags(f)
		FLAGS[0]=f
	end
	def Compiler.incls(ic)
		FLAGS[0]="#{FLAGS[0]} -I#{Makefile::SRCPREFIX[0]}#{ic}"
	end
	def Compiler.cdef(df)
		FLAGS[0]="#{FLAGS[0]} -D#{df}"
	end
	def Compiler.compile(name,dest)
		ext = Language.reduce(name)
		(Compiler::STEP[ext] or ["false"]).map{|a| a % {src: name,dest: dest,flags: FLAGS[0]} }
	end
end

module Linker
	STEPX = ["%{ld} -o %%{dest} %{ldflags} %%{parts} %{ldlibs}"] # -lgcc
	STEP = [["false"]]
	def Linker.add(conf)
		STEP[0] = STEPX.map{|a| a % conf }
	end
	def Linker.link(dest,parts)
		steps = STEP[0].map{|a| a % {dest: dest,parts: parts.join(" ")} }
		Makefile.rule(dest,parts,steps)
	end
end

module Makefile
	OUT = [$stdout]
	HAS = {}
	REPL = {"/"=>"_1","_"=>"_2"}
	REGX = /[\/\_]/
	SRCPREFIX = [""]
	DSTPREFIX = [""]
	def Makefile.rule(name,need,steps)
		a = steps.map{|a| "\t#{a}\n"}.join("\n")
		b = "#{name}: #{need.join(" ")}\n#{a}\n"
		Makefile::OUT[0].write(b)
	end
	def Makefile.need(src)
		if HAS[src]
			return HAS[src]
		end
		srcx = src[SRCPREFIX[0].length..-1].gsub(REGX,REPL)
		dest = "#{DSTPREFIX[0]}#{srcx}.o"
		Makefile.rule(dest,[src],Compiler.compile(src,dest))
		HAS[src] = dest
		dest
	end
	def Makefile.open(x)
		Makefile::OUT[0] = File.open(x,'w')
	end
	def Makefile.close
		if Makefile::OUT[0] == $stdout
			return
		end
		Makefile::OUT[0].close
		Makefile::OUT[0] = $stdout
	end
	def Makefile.project(src,dst="KBCache/")
		begin
			Dir.mkdir(dst)
		rescue
		end
		SRCPREFIX[0]=src
		DSTPREFIX[0]=dst
	end
	def Makefile.glob(path)
		Dir.glob("#{SRCPREFIX[0]}#{path}")
	end
end

module MKList
	LISTS = {}
	def MKList.add(name,data)
		if data
			LISTS[name] = (LISTS[name] or []) + data
		end
	end
	def MKList.get(names)
		names.map{|a| LISTS[a] or []}.reduce{|a,b|a+b}
	end
end

def add_mod(m,*depsrc)
	Linker.link(m,depsrc.map{|a| Makefile.need(a) })
end

