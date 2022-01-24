/*
 *----------------------------------------------------------------------
 *    UCT 6LoWPAN Protocol Stack
 *
 *    Copyright (c) 2014 UC Technology. All Rights Reserved.
 *----------------------------------------------------------------------
 *
 *    Version:   1.00.00
 *    Released by UC Technology (http://www.uctec.com/) at 2014/08/27.
 *
 *----------------------------------------------------------------------
 */

#ifndef __UTKN_BSWAP_H__
#define __UTKN_BSWAP_H__

#include <tk/tkernel.h>
#include <utkn/sysdepend/iote_m367/utkn_bswap_depend.h>>

#define __bswap16_constant(x)					\
	((UH)((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)))

#ifdef __GNUC__
#define bswap16(x)							\
	(__builtin_constant_p(x) ? __bswap16_constant(x) : __bswap16(x))
#else /* __GNUC__ */
#define bswap16(x) __bswap16(x)
#endif /* __GNUC__ */

#define __bswap32_constant(x)						\
	((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) |	\
	 (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

#ifdef __GNUC__
#define bswap32(x)							\
	(__builtin_constant_p(x) ? __bswap32_constant(x) : __bswap32(x))
#else /* __GNUC__ */
#define bswap32(x) __bswap32(x)
#endif /* __GNUC__ */

#endif /* __UTKN_BSWAP_H__ */
