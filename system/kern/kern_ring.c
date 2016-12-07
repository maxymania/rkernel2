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
#include <kern/ring.h>

void linked_ring_insert(linked_ring_t list,linked_ring_t elem,int after){
	linked_ring_t a,b,c;
	if(after) a = list, c = list->next;
	else      a = list->prev,c = list;
	b = elem;
	/* a->b->c */
	a->next = b;
	b->next = c;
	/* a<-b<-c */
	c->prev = b;
	b->prev = a;
}

void linked_ring_remove(linked_ring_t elem){
	linked_ring_t a,c;
	/* a <-> elem <-> c */
	a=elem->prev;
	c=elem->next;
	
	/* a <-> c */
	a->next = c;
	c->prev = a;
}

int  linked_ring_empty(linked_ring_t list){
	return (list->next == list) ? -1 : 0;
}

void linked_ring_init(linked_ring_t list){
	list->prev = list->next = list;
}

