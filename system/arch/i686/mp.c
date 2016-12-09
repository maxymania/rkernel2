/*
 * 
 * Copyright (c) 2006-2016 Frans Kaashoek, Robert Morris, Russ Cox,
 *                         Massachusetts Institute of Technology
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
#include <x86/mp.h>

struct ioapic;

/* GLOBAL VARIABLES: */
volatile u_int32_t*     __i686_local_apic;
volatile u_int8_t       __i686_ioapic_has;
volatile u_int8_t       __i686_ioapic_id;
volatile struct ioapic* __i686_ioapic;
u_int32_t __i686_ncpu;
u_int32_t __i686_boot_cpu;
u_int8_t  __i686_cpu_apics[256];


void _i686_get_mp(paddr_t *addrs);
u_intptr_t __i686_mp_map_range(paddr_t pa,int n);
void __i686_mp_unmap_range(u_intptr_t va,int n);

/* LOCAL VARIABLES: */

static u_intptr_t g_mpptr;
static int        g_mplen;

static u_intptr_t g_ctptr;
static int        g_ctlen;

static u_intptr_t g_cbptr;
static int        g_cblen;
static void*      g_conf_table_body;

#define GETINT(x)   *((const u_int32_t*)(x))
static int compare4(const void* a,const void* b){
	return GETINT(a)==GETINT(b);
}

static u_int8_t sum(u_int8_t *addr, int len)
{
	int i;
	u_int8_t sum;
	sum = 0;
	for(i=0; i<len; i++)
		sum += addr[i];
	return sum;
}

static struct mp*
mpsearch2(void* va, int len)
{
	void *e, *p;
	e = va+len;
	for(p = va; p < e; p += sizeof(struct mp))
		if(compare4(p,"_MP_") && sum(p, sizeof(struct mp)) == 0)
			return (struct mp*)p;
	return 0;
}

/*
 * Look for an MP structure in the len bytes at addr.
 */
static struct mp*
mpsearch1(paddr_t pa, int len)
{
	struct mp* mp;
	u_intptr_t ptr = __i686_mp_map_range(pa,len);
	if(!ptr) return 0; /* Just in case. */
	
	mp = mpsearch2((void*)ptr,len);
	if(!mp){
		__i686_mp_unmap_range(ptr,len);
		return 0;
	}
	g_mpptr = ptr;
	g_mplen = len;
	return mp;
}

/*
 * Search for the MP Floating Pointer Structure, which according to the
 * spec is in one of the following three locations:
 * 1) in the first KB of the EBDA;
 * 2) in the last KB of system base memory;
 * 3) in the BIOS ROM between 0xE0000 and 0xFFFFF.
 */
static struct mp*
mpsearch(void)
{
	paddr_t addr[2];
	struct mp* mp;
	_i686_get_mp(addr);
	if(addr[0]){
		if((mp = mpsearch1(addr[0], 1024))) return mp;
	}
	if(addr[1]>=1024){
		if((mp = mpsearch1(addr[1]-1024, 1024))) return mp;
	}
	return mpsearch1(0xF0000, 0x10000);
}

static u_intptr_t endpage(u_intptr_t va,int n){
	return (va + n - 1) & ~0xFFF;
}

/*
 * Search for an MP configuration table.  For now,
 * don't accept the default configurations (physaddr == 0).
 * Check for correct signature, calculate the checksum and,
 * if correct, check the version.
 *
 * TODO: check extended table checksum.
 */
static struct mpconf*
mpconfig(struct mp **pmp)
{
	paddr_t pa;
	struct mpconf *conf;
	struct mp *mp;
	*pmp = mp = mpsearch();
	//int natspace = 0;
	u_int8_t checksum;
	
	if((mp) == 0 || mp->physaddr == 0)
		return 0;
	pa = mp->physaddr;
	
	g_ctlen = sizeof(struct mpconf);
	g_ctptr = __i686_mp_map_range(pa,g_ctlen);
	if(!g_ctptr) return 0;
	
	conf = (struct mpconf*) g_ctptr;
	
	
	if(!compare4(conf, "PCMP"))
		return 0;
	
	if(conf->version != 1 && conf->version != 4)
		return 0;
	
	/*
	 * We initialize this variable, since we will need it.
	 */
	g_cblen = conf->length - sizeof(struct mpconf);
	
	if( endpage((u_intptr_t)conf,sizeof(struct mpconf)) < endpage((u_intptr_t)conf,conf->length) ) {
		/*
		 * TODO: Test this stuff.
		 */
		checksum = sum((u_int8_t*)conf,sizeof(struct mpconf));
		g_cbptr = __i686_mp_map_range(pa+sizeof(struct mpconf),g_cblen);
		if(!g_cbptr) return 0;
		checksum += sum((void*)g_cbptr,g_cblen);
		if(checksum) return 0;
		g_conf_table_body = (void*)g_cbptr;
	} else {
		g_cbptr = 0;
		if(sum((u_int8_t*)conf, conf->length) != 0)
			return 0;
		g_conf_table_body = &conf[1];
	}
	return conf;
}

static int mpinit(){
	int ismp = 1;
	u_int8_t *p, *e;
	struct mpproc *proc;
	struct mpioapic *ioapic;
	p=g_conf_table_body;
	e=g_conf_table_body+g_cblen;
	
	__i686_ioapic = 0;
	while(p<e){
		switch(*p){
			case MPPROC:
				proc = (struct mpproc *)p;
				if(__i686_ncpu<256){
					__i686_cpu_apics[__i686_ncpu] = proc->apicid;
					if((proc->flags)&MPBOOT)
						__i686_boot_cpu = __i686_ncpu;
				}
				__i686_ncpu++;
				p += sizeof(struct mpproc);
				continue;
			case MPIOAPIC:
				ioapic = (struct mpioapic*)p;
				__i686_ioapic_has = 0;
				__i686_ioapic_id  = ioapic->apicno;
				__i686_ioapic     = (struct ioapic*)(ioapic->addr);
				p += sizeof(struct mpioapic);
				continue;
			case MPBUS:
			case MPIOINTR:
			case MPLINTR:
				p += 8;
				continue;
		}
		ismp = 0;
		break;
	}
	/* No CPU had been found. */
	if(__i686_ncpu<1) ismp = 0;
	return ismp;
}

#define P2I(x) ((u_intptr_t)(x))

void _i686_initmp(){
	int ismp;
	struct mp* mp;
	struct mpconf *conf;
	g_mpptr = 0;
	g_mplen = 0;
	
	conf = mpconfig(&mp);
	
	__i686_ioapic_has = 0;
	__i686_ioapic = 0;
	__i686_boot_cpu = 256;
	if(conf){
		ismp = mpinit();
		__i686_local_apic = conf->lapicaddr;
		if(!__i686_ioapic    ) ismp = 0;
		if(!__i686_local_apic) ismp = 0;
		if(P2I(__i686_ioapic    )<0xfe000000) ismp = 0;
		if(P2I(__i686_local_apic)<0xfe000000) ismp = 0;
	}else ismp = 0;
	
	if(!ismp){
		__i686_local_apic = 0;
		__i686_boot_cpu = 0;
		__i686_ncpu = 1;
		__i686_ioapic_has = 0;
		__i686_ioapic = 0;
	}
}

