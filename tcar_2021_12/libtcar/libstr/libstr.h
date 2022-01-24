/*
 *----------------------------------------------------------------------
 *    micro T-Kernel 2.0 Software Package
 *
 *    Copyright (C) 2006-2014 by Ken Sakamura.
 *    This software is distributed under the T-License 2.0.
 *----------------------------------------------------------------------
 *
 *    Released by T-Engine Forum(http://www.t-engine.org/) at 2014/09/01.
 *
 *----------------------------------------------------------------------
 *    Modified by UC Technology at 2014/02/28.
 *    Modified by UC Technology at 2015/06/04.
 *
 *    UCT micro T-Kernel Version 2.00.12
 *    Copyright (c) 2014-2015 UC Technology. All Rights Reserved.
 *----------------------------------------------------------------------
 */

/*
 *	@(#)libstr.h
 *
 *	Standard library for kernel link
 *
 */

#ifndef	__LIBSTR_H__
#define __LIBSTR_H__


extern void* memset( void *s, int c, size_t n );
extern int memcmp( const void *s1, const void *s2, size_t n );
extern void* memcpy( void *dst, const void *src, size_t n );
extern void* memmove( void *dst, const void *src, size_t n );
extern void bzero( void *s, size_t n );

extern size_t strlen( const char *s );
extern int strcmp( const char *s1, const char *s2 );
extern int strncmp( const char *s1, const char *s2, size_t n );
extern char* strcpy( char *dst, const char *src );
extern char* strncpy( char *dst, const char *src, size_t n );
extern char* strcat( char *dst, const char *src );
extern char* strncat( char *dst, const char *src, size_t n );

extern long int strtol( const char *nptr, char **endptr, int base );

#endif /* __LIBSTR_H__ */
