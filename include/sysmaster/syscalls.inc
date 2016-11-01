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

DEF_SYSCALL(  0 ,int  ,nosys )
DEF_SYSCALL(  1 ,int  ,exit  , SCA(0,int, rval) )
DEF_SYSCALL(  2 ,int  ,fork  )
DEF_SYSCALL(  3 ,int  ,read  , SCA(0,int, fd), SCA(1,void*), SCA(2,size_t) )
DEF_SYSCALL(  4 ,int  ,write , SCA(0,int, fd), SCA(1,const void*), SCA(2,size_t) )
DEF_SYSCALL(  5 ,int  ,open  , SCA(0,const char*), SCA(1,int), SCA(2,int) )
DEF_SYSCALL(  6 ,int  ,close , SCA(0,int) )
DEF_SYSCALL(  7 ,int  ,wait4 , SCA(0,int, pid), SCA(1,int*, status), SCA(2,int, options), SCA(3,struct rusage*) )

/* 8 : creat(const char* path,int mode) */

DEF_SYSCALL(  9 ,int  ,link  , SCA(0,const char*,  path), SCA(1,const char*,  link) )
DEF_SYSCALL( 10 ,int  ,unlink, SCA(0,const char*,  path) )
/* 11 : obsolete execv() */
DEF_SYSCALL( 12 ,int  ,chdir , SCA(0,const char*,  path) )
DEF_SYSCALL( 13 ,int  ,fchdir, SCA(0,int,  fd) )
DEF_SYSCALL( 14 ,int  ,mknod , SCA(0,const char*,  path), SCA(1,int, mode), SCA(2,int, dev) )
DEF_SYSCALL( 15 ,int  ,chmod , SCA(0,const char*,  path), SCA(1,int, mode) )
DEF_SYSCALL( 16 ,int  ,chown , SCA(0,const char*,  path), SCA(1,int, uid), SCA(2,int, gid) )
DEF_SYSCALL( 17 ,int  ,obreak, SCA(0,char*,  nsize) )
DEF_SYSCALL( 18 ,int  ,getfsstat, SCA(0,struct statfs*, buf), SCA(1,int, bufsize), SCA(2,int, flags) )
/* 19 : lseek(int fd, long offset, int whence) */
DEF_SYSCALL( 20 ,pid_t,getpid )
DEF_SYSCALL( 21 ,int  ,mount , SCA(0,const char*,  source), SCA(1,const char*, dest), SCA(2,const char*, fstype),
		SCA(3,const long, mountflags), SCA(4,const void*, data) )
DEF_SYSCALL( 22 ,int  ,umount, SCA(0,const char*,path),SCA(1,int, umountflags) )
DEF_SYSCALL( 23 ,int  ,setuid, SCA(0,uid_t) )
DEF_SYSCALL( 24 ,uid_t,getuid )
/* BEGIN preliminary system calls */
DEF_SYSCALL( 1001 ,int  ,setgid, SCA(0,gid_t) )
DEF_SYSCALL( 1002 ,gid_t,getgid )
/* END preliminary system calls */
