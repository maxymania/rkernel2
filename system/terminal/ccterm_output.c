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
#include <sysplatform/console.h>
#include "ccterm_output.h"
#include "ansi_charmap.h"

static void ccterm_std_ops_consume (struct ccterm_buffer* buf){
	u_int16_t i = 0;
	u_int8_t curb;
	char cmc;
	for(;;){
		if(CCTB_EOB(buf,i)) break;
		curb = CCTB_IDX(buf,i);
		cmc = ansi_charmap[curb];
		if(!cmc) cmc = (curb<0x20)?CM_IGNORE:CM_PRINT;
		switch(cmc){
		case CM_PRINT:
			console_putchar(curb);
			break;
		case CM_CR:
			console_carriage_return();
			break;
		case CM_NL:
			console_newline();
			break;
		//case CM_TAB:
		//case CM_BACKSPACE:
		}
		++i;
	}
	buf->o_begin = CCTB_CUT(buf->o_begin+i);
}


static void ccterm_lite_ops_consume (struct ccterm_buffer* buf){
	u_int16_t i = 0;
	u_int8_t curb;
	char cmc;
	for(;;){
		if(CCTB_EOB(buf,i)) break;
		curb = CCTB_IDX(buf,i);
		cmc = ansi_charmap[curb];
		if(!cmc) cmc = (curb<0x20)?CM_IGNORE:CM_PRINT;
		switch(cmc){
		case CM_PRINT:
			console_putchar(curb);
			break;
		case CM_NL:
			console_carriage_return();
			console_newline();
			break;
		}
		++i;
	}
	buf->o_begin = CCTB_CUT(buf->o_begin+i);
}

const struct ccterm_ops ccterm_std_ops = { ccterm_std_ops_consume };

const struct ccterm_ops ccterm_lite_ops = { ccterm_lite_ops_consume };


