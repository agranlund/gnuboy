
#ifndef __FASTMEM_H__
#define __FASTMEM_H__


#include "defs.h"
#include "mem.h"


static byte readb(int a)
{
	byte *p = mbc.rmap[a>>12];
	if (p) return p[a];
	else return mem_read(a);
}

static void writeb(int a, byte b)
{
	byte *p = mbc.wmap[a>>12];
	if (p) p[a] = b;
	else mem_write(a, b);
}

#if 1

#define writeb_romram(a,b) writeb(a,b)

static inline byte readb_romram(int a)
{
    byte *p = mbc.rfmap[a>>12];
    return p[a];
}

static int readw_romram(int a)
{
    byte *p = mbc.rfmap[a>>12];
    word w = *(word *)(p+a);
#ifdef IS_LITTLE_ENDIAN
    return w;
#else
    return __builtin_bswap16(w);
#endif
}

static void writew_romram(int a, int b)
{
    byte *p = mbc.wfmap[a>>12];
#ifdef IS_LITTLE_ENDIAN
    *(word *)(p+a) = (word)b;
#else
    *(word *)(p+a) = __builtin_bswap16((word)b);
#endif    
}

#else
#define readb_romram(a) readb(a)
#define readw_romram(a) readw(a)
#define writeb_romram(a,b) writeb(a,b)
#define writew_romram(a,b) writew(a,b)
#endif


static int readw(int a)
{
	if ((a+1) & 0xfff)
	{
		byte *p = mbc.rmap[a>>12];
		if (p)
		{
#ifdef IS_LITTLE_ENDIAN
#ifndef ALLOW_UNALIGNED_IO
			if (a&1) return p[a] | (p[a+1]<<8);
#endif
			return *(word *)(p+a);
#else
			return p[a] | (p[a+1]<<8);
#endif
		}
	}
	return mem_read(a) | (mem_read(a+1)<<8);
}

static void writew(int a, int w)
{
	if ((a+1) & 0xfff)
	{
		byte *p = mbc.wmap[a>>12];
		if (p)
		{
#ifdef IS_LITTLE_ENDIAN
#ifndef ALLOW_UNALIGNED_IO
			if (a&1)
			{
				p[a] = w;
				p[a+1] = w >> 8;
				return;
			}
#endif
			*(word *)(p+a) = w;
			return;
#else
			p[a] = w;
			p[a+1] = w >> 8;
			return;
#endif
		}
	}
	mem_write(a, w);
	mem_write(a+1, w>>8);
}

static byte readhi(int a)
{
	return readb(a | 0xff00);
}

static void writehi(int a, byte b)
{
	writeb(a | 0xff00, b);
}

#if 0
static byte readhi(int a)
{
	byte (*rd)() = hi_read[a];
	return rd ? rd(a) : (ram.hi[a] | himask[a]);
}

static void writehi(int a, byte b)
{
	byte (*wr)() = hi_write[a];
	if (wr) wr(a, b);
	else ram.hi[a] = b & ~himask[a];
}
#endif


#endif
