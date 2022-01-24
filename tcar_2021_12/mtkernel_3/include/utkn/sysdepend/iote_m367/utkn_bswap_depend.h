/*
 *----------------------------------------------------------------------
 *    UCT 6LoWPAN Protocol Stack
 *
 *    Copyright (c) 2016-2019 UC Technology. All Rights Reserved.
 *----------------------------------------------------------------------
 *
 *    Version:   1.00.00
 *    Released by UC Technology (http://www.uctec.com/) at 2016/09/30.
 *    Released by UC Technology (http://www.uctec.com/) at 2019/01/11.
 *
 *----------------------------------------------------------------------
 */
#ifndef __UTKN_BSWAP_DEPEND_H__
#define __UTKN_BSWAP_DEPEND_H__

Inline UH __bswap16(register UH x)
{
	register UH y;
	Asm("rev16 %0, %1" : "=l"(y) : "l"(x) );
	return y;
}

Inline UW __bswap32(register UW x)
{
	register UW y;
	Asm("rev %0, %1" : "=l"(y) : "l"(x) );
	return y;
}

#endif /* __UTKN_BSWAP_DEPEND_H__ */
