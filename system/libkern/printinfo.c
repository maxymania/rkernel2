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
#include <string.h>

#include <sys/iopipe.h>
#include <sys/kterm.h>
#include <machine/stdarg.h>
#include <machine/types.h>
#include <machine/stdtypes.h>
#include <sysarch/paddr.h>

#define NUM  "0123456789"
#define LOW  "abcdef"
#define HIGH "ABCDEF"

static char hexlow[] = NUM LOW;
static char hexhig[] = NUM HIGH;

static char* render_phys_low(paddr_t paddr,char* buffer){
	const int n = 2*sizeof(paddr);
	int i;
	buffer+=98;
	*buffer = 0;
	for(i=0;i<n;++i){
		*--buffer = hexlow[paddr&0xF];
		paddr>>=4;
	}
	return buffer;
}

static char* render_phys_high(paddr_t paddr,char* buffer){
	const int n = 2*sizeof(paddr);
	int i;
	buffer+=98;
	*buffer = 0;
	for(i=0;i<n;++i){
		*--buffer = hexhig[paddr&0xF];
		paddr>>=4;
	}
	return buffer;
}

static char* render_virt_low(u_intptr_t paddr,char* buffer){
	const int n = 2*sizeof(paddr);
	int i;
	buffer+=98;
	*buffer = 0;
	for(i=0;i<n;++i){
		*--buffer = hexlow[paddr&0xF];
		paddr>>=4;
	}
	return buffer;
}

static char* render_virt_high(u_intptr_t paddr,char* buffer){
	const int n = 2*sizeof(paddr);
	int i;
	buffer+=98;
	*buffer = 0;
	for(i=0;i<n;++i){
		*--buffer = hexhig[paddr&0xF];
		paddr>>=4;
	}
	return buffer;
}

static char* u2a(unsigned int i,char* buffer){
	buffer+=98;
	*buffer=0;
	do{
		*--buffer = '0' + (i%10);
		i /= 10;
	}while(i);
	return buffer;
}

void iopipe_vprintinfo(struct iopipe* iopipe,const char* fmt,va_list ap){
	const char* last = fmt;
	char numbuf[100];
	const char* nb;
	while(*fmt){
		if(*fmt!='~'){ ++fmt; continue; }
		if(fmt>last)iopipe_write(iopipe,last,fmt-last);
		++fmt;
		switch(*fmt){
		case 'h':
			nb = render_phys_low(va_arg(ap, paddr_t),numbuf);
			iopipe_write(iopipe,nb,strlen(nb));
			break;
		case 'H':
			nb = render_phys_high(va_arg(ap, paddr_t),numbuf);
			iopipe_write(iopipe,nb,strlen(nb));
			break;
		case 'p':
			nb = render_virt_low(va_arg(ap, u_intptr_t),numbuf);
			iopipe_write(iopipe,nb,strlen(nb));
			break;
		case 'P':
			nb = render_virt_high(va_arg(ap, u_intptr_t),numbuf);
			iopipe_write(iopipe,nb,strlen(nb));
			break;
		case 'u':
		case 'i':
		case 'd':
			nb = u2a(va_arg(ap,unsigned int),numbuf);
			iopipe_write(iopipe,nb,strlen(nb));
			break;
		}
		fmt++;
		last = fmt;
	}
	if(fmt>last)iopipe_write(iopipe,last,fmt-last);
}

void iopipe_printinfo(struct iopipe* iopipe,const char* fmt,...){
	va_list ap;
	va_start(ap, fmt); 
	iopipe_vprintinfo(iopipe,fmt,ap);
	va_end(ap);
}

void printinfo(const char* fmt,...){
	va_list ap;
	va_start(ap, fmt); 
	iopipe_vprintinfo(kterm_instance,fmt,ap);
	va_end(ap);
}

