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
#include <vm/tree.h>

#define m64(i) ((i)&63)

static u_int32_t bt_max(u_int32_t a,u_int32_t b){
	return a>b?a:b;
}

static u_int32_t bt_depth(struct bintree_node *node){
	if(node)return node->depth;
	return 0;
}

static u_int32_t bt_calcdepth(struct bintree_node *node){
	if(!node) return 0;
	node->depth = bt_max(bt_depth(node->left),bt_depth(node->right))+1;
}

static void bt_rotleft(struct bintree_node **node){
	struct bintree_node * h = *node;
	struct bintree_node * x = h->right;
	h->right = x->left;
	x->left  = h;
	bt_calcdepth(h);
	bt_calcdepth(x);
	*node    = x;
}

static void bt_rotright(struct bintree_node **node){
	struct bintree_node * h = *node;
	struct bintree_node * x = h->left;
	h->left  = x->right;
	x->right = h;
	bt_calcdepth(h);
	bt_calcdepth(x);
	*node    = x;
}

/* balance a node, and calculate it's depth. */
static void bt_balance(struct bintree_node **node){
	struct bintree_node * h = *node;
	if(!h)return;
	u_int32_t l,r;
	l = bt_depth(h->left);
	r = bt_depth(h->right);
	if(l>r && ((l-r)>1))      bt_rotright(node);
	else if(r>l && ((r-l)>1)) bt_rotleft (node);
	else bt_calcdepth(h);
}

void bt_insert(struct bintree_node **node,struct bintree_node **it){
	struct bintree_node *n;
	u_intptr_t K = (*it)->K;
	u_intptr_t N;
	(*it)->left = (*it)->right = 0;
	struct bintree_node **narr[64];
	int i;
	for(i=0;i<64;++i)narr[i]=0;
	narr[m64(i)]=node;
	
	for(;;){
		n = *node;
		if(!n){
			*node = *it;
			*it = 0;
			break;
		}
		N = n->K;
		if(K<N){
			node = &(n->left);
			narr[m64(++i)]=node;
			continue;
		}
		if(N<K){
			node = &(n->right);
			narr[m64(++i)]=node;
			continue;
		}
		n->V = (*it)->V;
		break;
	}
	/* Backtracking. */
	for(;narr[m64(i)];--i){
		bt_balance(narr[m64(i)]);
		narr[m64(i)] = 0;
	}
}

void bt_remove(struct bintree_node **node,struct bintree_node **it){
	u_int32_t l,r;
	struct bintree_node *n;
	struct bintree_node **narr[64];
	int i;
	for(i=0;i<64;++i)narr[i]=0;
	narr[m64(i)]=node;
	
	for(;;){
		n = *node;
		if(!n) break;
		if(!(n->left)){
			*it = n;
			*node = n->right;
			break;
		}
		if(!(n->right)){
			*it = n;
			*node = n->left;
			break;
		}
		l = bt_depth(n->left );
		r = bt_depth(n->right);
		if(l<=r){
			bt_rotleft (node);
			node = &((*node)->left);
			narr[m64(++i)]=node;
		}else{
			bt_rotright(node);
			node = &((*node)->right);
			narr[m64(++i)]=node;
		}
	}
	
	for(;narr[m64(i)];--i){
		if(*narr[m64(i)])bt_balance(narr[m64(i)]);
		narr[m64(i)] = 0;
	}
}

struct bintree_node** bt_lookup(struct bintree_node **node,u_intptr_t K){
	struct bintree_node* n;
	u_intptr_t N;
	for(;;){
		n = *node;
		if(!n)return 0;
		N = n->K;
		if(K<N){
			node = &(n->left);
			continue;
		}
		if(N<K){
			node = &(n->right);
			continue;
		}
		return node;
	}
	return 0;
}

struct bintree_node** bt_floor(struct bintree_node **node,u_intptr_t K){
	struct bintree_node* n;
	struct bintree_node** lowermost = 0;
	u_intptr_t N;
	for(;;){
		n = *node;
		if(!n)return lowermost;
		N = n->K;
		if(K<N){
			node = &(n->left);
			continue;
		}
		if(N<K){
			lowermost = node;
			node = &(n->right);
			continue;
		}
		return node;
	}
	return lowermost;
}

struct bintree_node** bt_ceiling(struct bintree_node **node,u_intptr_t K){
	struct bintree_node* n;
	struct bintree_node** uppermost = 0;
	u_intptr_t N;
	for(;;){
		n = *node;
		if(!n)return uppermost;
		N = n->K;
		if(K<N){
			uppermost = node;
			node = &(n->left);
			continue;
		}
		if(N<K){
			node = &(n->right);
			continue;
		}
		return node;
	}
	return uppermost;
}
