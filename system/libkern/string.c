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
#include <machine/types.h>

size_t strlen(const char* str){
	size_t i=0;
	while(*str)str++,i++;
	return i;
}

char *strcpy( char* dest, const char* src ) {
	char* __restrict__ o = dest;
	const char* __restrict__ i = src;
	for(;(*o = *i); ++o,++i);
	return dest;
}

char *strncpy( char* dest, const char* src ,size_t n) {
	char* __restrict__ o = dest;
	const char* __restrict__ i = src;
	for(;n && (*o = (*i)); ++o,++i,--n);
	return dest;
}

char *strcat( char* __restrict__ dest, const char* src ) {
	char* __restrict__ o = dest;
	const char* __restrict__ i = src;
	for(; *o; ++o);
	for(; (*o = (*i)); ++o,++i);
	return dest;
}

char *strncat( char* dest, const char* src ,size_t n) {
	char* __restrict__ o = dest;
	const char* __restrict__ i = src;
	for(;n && *o; ++o,--n);
	for(;n && (*o = (*i)); ++o,++i,--n);
	return dest;
}

int strcmp(const char* string1, const char* string2) {
	const unsigned char* __restrict__ a = (const unsigned char*)string1;
	const unsigned char* __restrict__ b = (const unsigned char*)string2;
	for(;*a==*b && *a; ++a,++b);
	return *a-*b;
}

void *memset( void *dest, int ch, size_t count ){
	u_int8_t* o = dest;
	u_int8_t c = (u_int8_t)ch;
	for(;count;--count,++o)*o=c;
	return dest;
}

void* memcpy( void *dest, const void *src, size_t n ) {
	char* __restrict__ o = dest;
	const char* __restrict__ i = src;
	for(;n; ++o,++i,--n) *o = *i;
	return dest;
}

int memcmp( const void* lhs, const void* rhs, size_t n ) {
	const unsigned char* __restrict__ a = lhs;
	const unsigned char* __restrict__ b = rhs;
	for(;n && *a==*b; ++a,++b,--n);
	if(!n)return 0;
	return *a-*b;
}

void* memmove( void* dest, const void* src, size_t n ){
	char* __restrict__ o = dest;
	const char* __restrict__ i = src;
	if(i>o) for(;n; ++i,++o,--n) *o = *i;
	else{
		i+=n-1;
		o+=n-1;
		for(;n; --i,--o,--n) *o = *i;
	}
	return dest;
}

void* memchr( const void* ptr, int ch, size_t n ){
	const u_int8_t* __restrict__ i = ptr;
	const u_int8_t c = ch;
	for(;n;++i,--n) if(*i == c) return (void*)i;
	return 0;
}


