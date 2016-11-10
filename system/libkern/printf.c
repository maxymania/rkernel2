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
#include <stdio.h>
#include <string.h>
#include <sys/iopipe.h>
#include <sys/kterm.h>
#include <machine/stdarg.h>
#include <machine/types.h>
#include <machine/stdtypes.h>

#define NUM  "0123456789"
#define LOW  "abcdef"
#define HIGH "ABCDEF"

static char hexlow[] = NUM LOW;
static char hexhig[] = NUM HIGH;

static char* i2a(int i,char* buffer){
	char pre = 0;
	buffer+=98;
	*buffer=0;
	if(i<0){
		i = -i;
		pre = '-';
	}
	while(i){
		*--buffer = '0' + (i%10);
		i /= 10;
	}
	if(pre) *--buffer = pre;
	return buffer;
}

static char* u2a(unsigned int i,char* buffer){
	buffer+=98;
	*buffer=0;
	while(i){
		*--buffer = '0' + (i%10);
		i /= 10;
	}
	return buffer;
}

static char* x2a(unsigned int i,char* buffer){
	buffer+=98;
	*buffer=0;
	while(i){
		*--buffer = hexlow[i&0xf];
		i >>= 4;
	}
	return buffer;
}

static char* X2a(unsigned int i,char* buffer){
	buffer+=98;
	*buffer=0;
	while(i){
		*--buffer = hexhig[i&0xf];
		i >>= 4;
	}
	return buffer;
}

static char* o2a(unsigned int i,char* buffer){
	buffer+=98;
	*buffer=0;
	while(i){
		*--buffer = '0' + (i&0x7);
		i >>= 4;
	}
	return buffer;
}
static char* p2a(void* ptr,char* buffer){
	buffer+=98;
	*buffer=0;
	u_intptr_t i = (u_intptr_t)ptr;
	*--buffer = '0';
	*--buffer = 'x';
	while(i){
		*--buffer = hexlow[i&0xf];
		i >>= 4;
	}
	return buffer;
}

/* -------------------------------------------------------------------- */

static char* li2a(long int i,char* buffer){
	char pre = 0;
	buffer+=98;
	*buffer=0;
	if(i<0){
		i = -i;
		pre = '-';
	}
	while(i){
		*--buffer = '0' + (i%10);
		i /= 10;
	}
	if(pre) *--buffer = pre;
	return buffer;
}

static char* lu2a(unsigned long int i,char* buffer){
	buffer+=98;
	*buffer=0;
	while(i){
		*--buffer = '0' + (i%10);
		i /= 10;
	}
	return buffer;
}

static char* lx2a(unsigned long int i,char* buffer){
	buffer+=98;
	*buffer=0;
	while(i){
		*--buffer = hexlow[i&0xf];
		i >>= 4;
	}
	return buffer;
}

static char* lX2a(unsigned long int i,char* buffer){
	buffer+=98;
	*buffer=0;
	while(i){
		*--buffer = hexhig[i&0xf];
		i >>= 4;
	}
	return buffer;
}

static char* lo2a(unsigned long int i,char* buffer){
	buffer+=98;
	*buffer=0;
	while(i){
		*--buffer = '0' + (i&0x7);
		i >>= 4;
	}
	return buffer;
}

/* -------------------------------------------------------------------- */

static char* lli2a(long long int i,char* buffer){
	char pre = 0;
	buffer+=98;
	*buffer=0;
	if(i<0){
		i = -i;
		pre = '-';
	}
	while(i){
		*--buffer = '0' + (i%10);
		i /= 10;
	}
	if(pre) *--buffer = pre;
	return buffer;
}

static char* llu2a(unsigned long long int i,char* buffer){
	buffer+=98;
	*buffer=0;
	while(i){
		*--buffer = '0' + (i%10);
		i /= 10;
	}
	return buffer;
}

static char* llx2a(unsigned long long int i,char* buffer){
	buffer+=98;
	*buffer=0;
	while(i){
		*--buffer = hexlow[i&0xf];
		i >>= 4;
	}
	return buffer;
}

static char* llX2a(unsigned long long int i,char* buffer){
	buffer+=98;
	*buffer=0;
	while(i){
		*--buffer = hexhig[i&0xf];
		i >>= 4;
	}
	return buffer;
}

static char* llo2a(unsigned long long int i,char* buffer){
	buffer+=98;
	*buffer=0;
	while(i){
		*--buffer = '0' + (i&0x7);
		i >>= 4;
	}
	return buffer;
}

/* -------------------------------------------------------------------- */

static char* zi2a(ssize_t i,char* buffer){
	char pre = 0;
	buffer+=98;
	*buffer=0;
	if(i<0){
		i = -i;
		pre = '-';
	}
	while(i){
		*--buffer = '0' + (i%10);
		i /= 10;
	}
	if(pre) *--buffer = pre;
	return buffer;
}

static char* zu2a(size_t i,char* buffer){
	buffer+=98;
	*buffer=0;
	while(i){
		*--buffer = '0' + (i%10);
		i /= 10;
	}
	return buffer;
}

static char* zx2a(size_t i,char* buffer){
	buffer+=98;
	*buffer=0;
	while(i){
		*--buffer = hexlow[i&0xf];
		i >>= 4;
	}
	return buffer;
}

static char* zX2a(size_t i,char* buffer){
	buffer+=98;
	*buffer=0;
	while(i){
		*--buffer = hexhig[i&0xf];
		i >>= 4;
	}
	return buffer;
}

static char* zo2a(size_t i,char* buffer){
	buffer+=98;
	*buffer=0;
	while(i){
		*--buffer = '0' + (i&0x7);
		i >>= 4;
	}
	return buffer;
}

/* -------------------------------------------------------------------- */

static char* ti2a(ptrdiff_t i,char* buffer){
	char pre = 0;
	buffer+=98;
	*buffer=0;
	if(i<0){
		i = -i;
		pre = '-';
	}
	while(i){
		*--buffer = '0' + (i%10);
		i /= 10;
	}
	if(pre) *--buffer = pre;
	return buffer;
}

static char* tu2a(u_intptr_t i,char* buffer){
	buffer+=98;
	*buffer=0;
	while(i){
		*--buffer = '0' + (i%10);
		i /= 10;
	}
	return buffer;
}

static char* tx2a(u_intptr_t i,char* buffer){
	buffer+=98;
	*buffer=0;
	while(i){
		*--buffer = hexlow[i&0xf];
		i >>= 4;
	}
	return buffer;
}

static char* tX2a(u_intptr_t i,char* buffer){
	buffer+=98;
	*buffer=0;
	while(i){
		*--buffer = hexhig[i&0xf];
		i >>= 4;
	}
	return buffer;
}

static char* to2a(u_intptr_t i,char* buffer){
	buffer+=98;
	*buffer=0;
	while(i){
		*--buffer = '0' + (i&0x7);
		i >>= 4;
	}
	return buffer;
}

/* -------------------------------------------------------------------- */


static int iopipe_vioprintf(struct iopipe* iopipe,const char* fmt,va_list ap){
	int count = 0;
	const char* last = fmt;
	char numbuf[100];
	const char* nb;
	size_t siz;
	while(*fmt){
		if(*fmt!='%'){fmt++;continue;}
		switch(fmt[1]){
		case '%':
			fmt++;
			count += fmt-last;
			if(fmt>last)iopipe->iop_ops->io_write(iopipe,last,fmt-last);
			break;
		default:
			count += fmt-last;
			if(fmt>last)iopipe->iop_ops->io_write(iopipe,last,fmt-last);
			fmt++;
		}
		
		switch(*fmt){
		case 'z':
			fmt++;
			switch(*fmt){
			case 'd':
			case 'i':
				nb = zi2a(va_arg(ap, ssize_t),numbuf);
				iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
				break;
			case 'u':
				nb = zu2a(va_arg(ap, size_t),numbuf);
				iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
				break;
			case 'x':
				nb = zx2a(va_arg(ap, size_t),numbuf);
				iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
				break;
			case 'X':
				nb = zX2a(va_arg(ap, size_t),numbuf);
				iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
				break;
			case 'o':
				nb = zo2a(va_arg(ap, size_t),numbuf);
				iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
				break;
			}
			break;
		case 't':
			fmt++;
			switch(*fmt){
			case 'd':
			case 'i':
				nb = ti2a(va_arg(ap, ptrdiff_t),numbuf);
				iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
				break;
			case 'u':
				nb = tu2a(va_arg(ap, u_intptr_t),numbuf);
				iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
				break;
			case 'x':
				nb = tx2a(va_arg(ap, u_intptr_t),numbuf);
				iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
				break;
			case 'X':
				nb = tX2a(va_arg(ap, u_intptr_t),numbuf);
				iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
				break;
			case 'o':
				nb = to2a(va_arg(ap, u_intptr_t),numbuf);
				iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
				break;
			}
			break;
		case 'l':
			fmt++;
			switch(*fmt){
			case 'l':
				fmt++;
				switch(*fmt){
				case 'd':
				case 'i':
					nb = lli2a(va_arg(ap, long long int),numbuf);
					iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
					break;
				case 'u':
					nb = llu2a(va_arg(ap, unsigned long long int),numbuf);
					iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
					break;
				case 'x':
					nb = llx2a(va_arg(ap, unsigned long long int),numbuf);
					iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
					break;
				case 'X':
					nb = llX2a(va_arg(ap, unsigned long long int),numbuf);
					iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
					break;
				case 'o':
					nb = llo2a(va_arg(ap, unsigned long long int),numbuf);
					iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
					break;
				}
				break;
			case 'd':
			case 'i':
				nb = li2a(va_arg(ap, long int),numbuf);
				iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
				break;
			case 'u':
				nb = lu2a(va_arg(ap, unsigned long int),numbuf);
				iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
				break;
			case 'x':
				nb = lx2a(va_arg(ap, unsigned long int),numbuf);
				iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
				break;
			case 'X':
				nb = lX2a(va_arg(ap, unsigned long int),numbuf);
				iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
				break;
			case 'o':
				nb = lo2a(va_arg(ap, unsigned long int),numbuf);
				iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
				break;
			}
			break;
		case 'd':
		case 'i':
			nb = i2a(va_arg(ap, int),numbuf);
			iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
			break;
		case 'u':
			nb = u2a(va_arg(ap, unsigned int),numbuf);
			iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
			break;
		case 'x':
			nb = x2a(va_arg(ap, unsigned int),numbuf);
			iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
			break;
		case 'X':
			nb = X2a(va_arg(ap, unsigned int),numbuf);
			iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
			break;
		case 'o':
			nb = o2a(va_arg(ap, unsigned int),numbuf);
			iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
			break;
		case 'p':
			nb = p2a(va_arg(ap, void*),numbuf);
			iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
			break;
		case 'c':
			numbuf[0] = va_arg(ap,int);
			iopipe->iop_ops->io_write(iopipe,numbuf,1); count++;
			break;
		case 's':
			nb = va_arg(ap,const char*);
			if(!nb) nb = "(null)";
			iopipe->iop_ops->io_write(iopipe,nb,siz=strlen(nb) ); count += siz;
			break;
		}
		fmt++;
		last = fmt;
	}
	count += fmt-last;
	if(fmt>last)iopipe->iop_ops->io_write(iopipe,last,fmt-last);
	return count;
}


int printf(const char* fmt,...){
	va_list ap;
	va_start(ap, fmt); 
	int ret = iopipe_vioprintf(kern_instance,fmt,ap);
	va_end(ap);
	return ret;
}
