/*
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
#pragma once
#include <machine/types.h>

struct bintree_node {
	u_intptr_t K;
	void*      V;
	struct bintree_node* left ;
	struct bintree_node* right;
	struct bintree_node* recycle; /* Recycle-time Linked list*/
	u_int32_t  depth;
};

void bt_insert(struct bintree_node **node,struct bintree_node **it);
void bt_remove(struct bintree_node **node,struct bintree_node **it);
struct bintree_node** bt_lookup(struct bintree_node **node,u_intptr_t K);
struct bintree_node** bt_floor(struct bintree_node **node,u_intptr_t K);
struct bintree_node** bt_ceiling(struct bintree_node **node,u_intptr_t K);

