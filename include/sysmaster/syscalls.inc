/*
 * 
 * Copyright (c) 2016 Simon Schmidt
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * Included from other files to define a syscall api.
 * 
 * Users of this file need to declare an implementation of the following macro:
 * 
 *  - DEF_SYSCALL(syscall #, return type, name, arguments...)
 *  - SYSCALL_ARG(arg #, arg type) , or SCA(arg #, arg type [, arg name])
 */

#ifndef SCA

#ifdef SYSCALL_ARG
#define SCA(num,typ,...) SYSCALL_ARG(num,typ)
#else // SYSCALL_ARG
#define SCA(num,typ,...) typ arg ## num
#endif // SYSCALL_ARG

#endif // SCA

#ifndef DEF_SYSCALL
#error DEF_SYSCALL not defined
#endif

DEF_SYSCALL(  0 ,int  ,nosys  )
DEF_SYSCALL(  1 ,int  ,exit   , SCA(0,int, rval) )
DEF_SYSCALL(  2 ,int  ,fork   )
DEF_SYSCALL(  3 ,int  ,read   , SCA(0,int, fd), SCA(1,void*), SCA(2,size_t) )
DEF_SYSCALL(  4 ,int  ,write  , SCA(0,int, fd), SCA(1,const void*), SCA(2,size_t) )
DEF_SYSCALL(  5 ,int  ,open   , SCA(0,const char*), SCA(1,int), SCA(2,int) )
DEF_SYSCALL(  6 ,int  ,close  , SCA(0,int) )
DEF_SYSCALL(  7 ,pid_t,waitpid, SCA(0,pid_t, pid), SCA(1,int*, status), SCA(2,int, options))

#undef DEF_SYSCALL
#undef SCA

#ifdef SYSCALL_ARG
#undef SYSCALL_ARG
#endif
