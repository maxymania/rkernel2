/*
 * Copyright (c) 2017 Simon Schmidt
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
#pragma once

#include <utils/compiler.h>

struct list_node;
typedef struct list_node list_node_s, *list_node_t;

struct list_node{
	list_node_t next,prev;
};

static inline void list_init(list_node_t lst){
	lst->prev = lst->next = lst;
}

static inline void list_clear(list_node_t lst){
	lst->prev = lst->next = 0;
}

static inline int list_is_in_list(list_node_t node){
	return ((node->prev!=0) && (node->next!=0));
}

static inline void list_add_after(list_node_t a, list_node_t b){
	/* Before: a<->c */
	list_node_t c = a->next;
	
	/* a->b->c */
	a->next = b;
	b->next = c;
	/* a<-b<-c */
	c->prev = b;
	b->prev = a;
	
	/* Now: a<->b<->c */
}

static inline void list_add_before(list_node_t c, list_node_t b){
	/* Before: a<->c */
	list_node_t a = c->next;
	
	/* a->b->c */
	a->next = b;
	b->next = c;
	/* a<-b<-c */
	c->prev = b;
	b->prev = a;
	
	/* Now: a<->b<->c */
}

#define list_push_tail(list,item) list_add_before(list,item)

#define list_push_head(list,item) list_add_after(list,item)

static inline void list_item_remove(list_node_t elem){
	list_node_t a,c;
	/* a <-> elem <-> c */
	a=elem->prev;
	c=elem->next;
	
	/* a <-> c */
	a->next = c;
	c->prev = a;
}

static inline list_node_t list_pop_tail(list_node_t list){
	list_node_t ret = list->prev;
	if(ret==list||!ret) return (list_node_t)0;
	list_item_remove(ret);
	return ret;
}

static inline list_node_t list_pop_head(list_node_t list){
	list_node_t ret = list->next;
	if(ret==list||!ret) return (list_node_t)0;
	list_item_remove(ret);
	return ret;
}

#define list_foreach(list,node) for(node = (list)->next; node!=(list); node = (node)->next)

#define list_foreach_type(type,member,list,node) \
	for(	node = containerof(list->next,type,member);  \
		&((node)->member)!=(list);                   \
		node = (node)->member.next )


