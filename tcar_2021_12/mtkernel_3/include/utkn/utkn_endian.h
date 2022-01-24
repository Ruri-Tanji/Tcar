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

#ifndef __UTKN_ENDIAN_H__
#define __UTKN_ENDIAN_H__

#include <tk/tkernel.h>
#include <utkn/utkn_bswap.h>

#if TK_BIGENDIAN

#define htobe16(x) (x)
#define htole16(x) bswap16(x)
#define be16toh(x) (x)
#define le16toh(x) bswap16(x)
#define htobe32(x) (x)
#define htole32(x) bswap32(x)
#define be32toh(x) (x)
#define le32toh(x) bswap32(x)

#else

#define htobe16(x) bswap16(x)
#define htole16(x) (x)
#define be16toh(x) bswap16(x)
#define le16toh(x) (x)
#define htobe32(x) bswap32(x)
#define htole32(x) (x)
#define be32toh(x) bswap32(x)
#define le32toh(x) (x)

#endif

#define htonl htobe32
#define htons htobe16
#define ntohl be32toh
#define ntohs be16toh

#endif /* __UTKN_ENDIAN_H__ */
